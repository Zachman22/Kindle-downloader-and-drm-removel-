#include "KindleDownloader.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QWebEngineProfile>
#include <QWebEngineCookieStore>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>
#include <QRegularExpression>

QMap<QString, QString> KindleDownloader::s_amazonDomains = {
    {"US", "amazon.com"},
    {"UK", "amazon.co.uk"},
    {"DE", "amazon.de"},
    {"FR", "amazon.fr"},
    {"ES", "amazon.es"},
    {"IT", "amazon.it"},
    {"JP", "amazon.co.jp"},
    {"CA", "amazon.ca"},
    {"AU", "amazon.com.au"}
};

KindleDownloader::KindleDownloader(const QString &email,
                                   const QString &password,
                                   const QString &region,
                                   const QString &downloadDir,
                                   QObject *parent)
    : QObject(parent)
    , m_email(email)
    , m_password(password)
    , m_region(region)
    , m_downloadDir(downloadDir)
    , m_webView(nullptr)
    , m_webPage(nullptr)
    , m_networkManager(nullptr)
{
    m_domain = getDomainForRegion(region);
    m_baseUrl = QString("https://www.%1").arg(m_domain);

    QDir().mkpath(m_downloadDir);

    m_networkManager = new QNetworkAccessManager(this);
}

KindleDownloader::~KindleDownloader()
{
    if (m_webView) {
        delete m_webView;
    }
}

QString KindleDownloader::getDomainForRegion(const QString &region)
{
    return s_amazonDomains.value(region, "amazon.com");
}

void KindleDownloader::setupWebView(bool interactive)
{
    if (m_webView) {
        delete m_webView;
    }

    m_webView = new QWebEngineView();
    m_webPage = m_webView->page();

    if (!interactive) {
        // Headless mode - hide the window
        m_webView->setWindowFlags(Qt::FramelessWindowHint);
        m_webView->resize(1, 1);
    } else {
        m_webView->resize(1024, 768);
        m_webView->show();
    }

    connect(m_webPage, &QWebEnginePage::loadFinished,
            this, &KindleDownloader::onLoadFinished);
}

void KindleDownloader::login(bool interactive)
{
    qDebug() << "Logging in to Amazon" << m_region << "...";
    emit progressUpdate(QString("Logging in to Amazon %1...").arg(m_region));

    setupWebView(interactive);

    // Navigate to Kindle library
    QString libraryUrl = QString("%1/hz/mycd/digital-console/contentlist/booksAll/dateDsc/")
                         .arg(m_baseUrl);
    m_webPage->load(QUrl(libraryUrl));
}

void KindleDownloader::onLoadFinished(bool ok)
{
    if (!ok) {
        emit loginFailed("Page load failed");
        return;
    }

    QString currentUrl = m_webPage->url().toString();
    qDebug() << "Page loaded:" << currentUrl;

    // Check if we need to login
    if (currentUrl.contains("signin", Qt::CaseInsensitive) ||
        currentUrl.contains("login", Qt::CaseInsensitive)) {

        // Fill in email
        QString emailScript = QString(R"(
            var emailField = document.getElementById('ap_email') ||
                           document.querySelector('input[name="email"]');
            if (emailField) {
                emailField.value = '%1';
                var continueBtn = document.getElementById('continue');
                if (continueBtn) continueBtn.click();
            }
        )").arg(m_email);

        m_webPage->runJavaScript(emailScript);

        // Wait a bit, then fill password
        QTimer::singleShot(2000, this, [this]() {
            QString passwordScript = QString(R"(
                var passwordField = document.getElementById('ap_password');
                if (passwordField) {
                    passwordField.value = '%1';
                    var signInBtn = document.getElementById('signInSubmit');
                    if (signInBtn) signInBtn.click();
                }
            )").arg(m_password);

            m_webPage->runJavaScript(passwordScript);
        });

    } else if (currentUrl.contains("digital-console") || currentUrl.contains("myk")) {
        // Successfully logged in
        qDebug() << "Login successful!";
        emit loginSuccess();
        emit progressUpdate("Login successful!");

        // Now fetch the library
        fetchKindleLibrary();
    }
}

void KindleDownloader::fetchKindleLibrary()
{
    qDebug() << "Fetching Kindle library...";
    emit progressUpdate("Fetching Kindle library...");

    // Scroll to load all books (lazy loading)
    QString scrollScript = R"(
        var lastHeight = document.body.scrollHeight;
        function scrollDown() {
            window.scrollTo(0, document.body.scrollHeight);
            setTimeout(function() {
                var newHeight = document.body.scrollHeight;
                if (newHeight > lastHeight) {
                    lastHeight = newHeight;
                    scrollDown();
                } else {
                    // Done scrolling, extract books
                    window.qt_scrollingComplete = true;
                }
            }, 2000);
        }
        scrollDown();
    )";

    m_webPage->runJavaScript(scrollScript);

    // Check periodically if scrolling is complete
    QTimer *checkTimer = new QTimer(this);
    connect(checkTimer, &QTimer::timeout, this, [this, checkTimer]() {
        m_webPage->runJavaScript("window.qt_scrollingComplete;", [this, checkTimer](const QVariant &result) {
            if (result.toBool()) {
                checkTimer->stop();
                checkTimer->deleteLater();
                extractBooksFromPage();
            }
        });
    });
    checkTimer->start(1000);
}

void KindleDownloader::extractBooksFromPage()
{
    QString extractScript = R"(
        var books = [];
        var bookElements = document.querySelectorAll('[data-asin]');
        for (var i = 0; i < bookElements.length; i++) {
            var elem = bookElements[i];
            var asin = elem.getAttribute('data-asin');
            if (!asin) continue;

            var titleElem = elem.querySelector('.library-item-title, h2');
            var title = titleElem ? titleElem.innerText.trim() : '';

            var authorElem = elem.querySelector('.library-item-author, .author');
            var author = authorElem ? authorElem.innerText.trim() : 'Unknown';

            if (title && asin) {
                books.push({
                    asin: asin,
                    title: title,
                    author: author
                });
            }
        }
        JSON.stringify(books);
    )";

    m_webPage->runJavaScript(extractScript, [this](const QVariant &result) {
        QString jsonStr = result.toString();
        QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
        QJsonArray booksArray = doc.array();

        m_books.clear();
        for (const QJsonValue &value : booksArray) {
            QJsonObject obj = value.toObject();
            BookInfo book;
            book.asin = obj["asin"].toString();
            book.title = obj["title"].toString();
            book.author = obj["author"].toString();
            book.url = QString("%1/dp/%2").arg(m_baseUrl, book.asin);

            m_books.append(book);
            qDebug() << "Found book:" << book.title << "by" << book.author;
        }

        qDebug() << "Found" << m_books.size() << "books in library";
        emit progressUpdate(QString("Found %1 books").arg(m_books.size()));
        emit libraryFetched(m_books);
    });
}

void KindleDownloader::downloadBook(const BookInfo &book)
{
    qDebug() << "Downloading:" << book.title;
    emit progressUpdate(QString("Downloading: %1").arg(book.title));

    m_currentBook = book;

    // Download URL
    QString downloadUrl = QString("%1/hz/mycd/ajax/downloadDeviceReaderBook?asin=%2")
                         .arg(m_baseUrl, book.asin);

    QNetworkRequest request(downloadUrl);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                     "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");

    QNetworkReply *reply = m_networkManager->get(request);

    connect(reply, &QNetworkReply::downloadProgress,
            this, &KindleDownloader::onDownloadProgress);
    connect(reply, &QNetworkReply::finished,
            this, &KindleDownloader::onDownloadFinished);
}

void KindleDownloader::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        int percentage = (bytesReceived * 100) / bytesTotal;
        emit progressUpdate(QString("Downloading %1: %2%")
                           .arg(m_currentBook.title)
                           .arg(percentage));
    }
}

void KindleDownloader::onDownloadFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();

        // Determine file extension
        QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
        QString ext = ".azw";
        if (contentType.contains("azw3")) ext = ".azw3";
        else if (contentType.contains("mobi")) ext = ".mobi";

        // Sanitize filename
        QString safeTitle = m_currentBook.title;
        safeTitle.replace(QRegularExpression("[<>:\"/\\\\|?*]"), "");
        safeTitle = safeTitle.trimmed();

        QString filename = QString("%1%2").arg(safeTitle, ext);
        QString filepath = QString("%1/%2").arg(m_downloadDir, filename);

        // Save file
        QFile file(filepath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(data);
            file.close();

            qDebug() << "Downloaded:" << filepath;
            emit progressUpdate(QString("Downloaded: %1").arg(filename));
            emit bookDownloaded(filepath);

            // Save metadata
            saveBookMetadata(m_currentBook, filepath);
        } else {
            emit bookDownloadFailed(m_currentBook.title, "Could not save file");
        }
    } else {
        qWarning() << "Download failed:" << reply->errorString();
        emit bookDownloadFailed(m_currentBook.title, reply->errorString());
    }

    reply->deleteLater();
}

void KindleDownloader::saveBookMetadata(const BookInfo &book, const QString &filepath)
{
    QJsonObject obj;
    obj["asin"] = book.asin;
    obj["title"] = book.title;
    obj["author"] = book.author;
    obj["url"] = book.url;

    QJsonDocument doc(obj);

    QString metadataPath = filepath;
    metadataPath.replace(QRegularExpression("\\.[^.]*$"), ".json");

    QFile file(metadataPath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}

void KindleDownloader::downloadAllBooks(const QStringList &specificTitles)
{
    QList<BookInfo> booksToDownload;

    if (specificTitles.isEmpty()) {
        booksToDownload = m_books;
    } else {
        for (const BookInfo &book : m_books) {
            if (specificTitles.contains(book.title)) {
                booksToDownload.append(book);
            }
        }
    }

    qDebug() << "Downloading" << booksToDownload.size() << "books...";

    // Download sequentially to avoid overwhelming the server
    for (const BookInfo &book : booksToDownload) {
        downloadBook(book);

        // Wait for download to complete
        QEventLoop loop;
        connect(this, &KindleDownloader::bookDownloaded, &loop, &QEventLoop::quit);
        connect(this, &KindleDownloader::bookDownloadFailed, &loop, &QEventLoop::quit);
        QTimer::singleShot(30000, &loop, &QEventLoop::quit); // 30 second timeout
        loop.exec();

        // Be nice to Amazon's servers
        QThread::sleep(2);
    }
}
