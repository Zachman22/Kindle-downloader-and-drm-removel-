#ifndef KINDLEDOWNLOADER_H
#define KINDLEDOWNLOADER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QNetworkAccessManager>
#include <QNetworkReply>

struct BookInfo {
    QString asin;
    QString title;
    QString author;
    QString url;
};

class KindleDownloader : public QObject
{
    Q_OBJECT

public:
    explicit KindleDownloader(const QString &email,
                             const QString &password,
                             const QString &region,
                             const QString &downloadDir,
                             QObject *parent = nullptr);
    ~KindleDownloader();

    void login(bool interactive = false);
    void fetchKindleLibrary();
    void downloadBook(const BookInfo &book);
    void downloadAllBooks(const QStringList &specificTitles = QStringList());

signals:
    void loginSuccess();
    void loginFailed(const QString &error);
    void libraryFetched(const QList<BookInfo> &books);
    void bookDownloaded(const QString &filepath);
    void bookDownloadFailed(const QString &bookTitle, const QString &error);
    void progressUpdate(const QString &message);

private slots:
    void onLoadFinished(bool ok);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadFinished();

private:
    QString m_email;
    QString m_password;
    QString m_region;
    QString m_downloadDir;
    QString m_domain;
    QString m_baseUrl;

    QWebEngineView *m_webView;
    QWebEnginePage *m_webPage;
    QNetworkAccessManager *m_networkManager;

    QList<BookInfo> m_books;
    BookInfo m_currentBook;

    static QMap<QString, QString> s_amazonDomains;

    void setupWebView(bool interactive);
    QString getDomainForRegion(const QString &region);
    void extractBooksFromPage();
    void saveBookMetadata(const BookInfo &book, const QString &filepath);
};

#endif // KINDLEDOWNLOADER_H
