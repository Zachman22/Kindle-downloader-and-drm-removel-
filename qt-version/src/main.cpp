#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QTimer>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <iostream>

#include "ConfigManager.h"
#include "KindleDownloader.h"
#include "DRMRemover.h"
#include "EbookConverter.h"

class KindleDRMConverter : public QObject
{
    Q_OBJECT

public:
    KindleDRMConverter(QObject *parent = nullptr)
        : QObject(parent)
        , m_configManager(nullptr)
        , m_downloader(nullptr)
        , m_drmRemover(nullptr)
        , m_converter(nullptr)
    {
    }

    ~KindleDRMConverter()
    {
        cleanup();
    }

    void run(const QCommandLineParser &parser)
    {
        printHeader();

        // Load configuration
        QString configPath = parser.value("config");
        m_configManager = new ConfigManager(this);

        if (!m_configManager->loadConfig(configPath)) {
            std::cerr << "Error: Could not load config file: "
                     << configPath.toStdString() << std::endl;
            std::cerr << "Please create a config.yaml file (see config.yaml.example)" << std::endl;
            QCoreApplication::exit(1);
            return;
        }

        // Override config with command line options
        QStringList formats;
        if (parser.isSet("formats")) {
            QString formatsStr = parser.value("formats");
            formats = formatsStr.split(',', QString::SkipEmptyParts);
            for (QString &fmt : formats) {
                fmt = fmt.trimmed();
            }
        } else {
            formats = m_configManager->conversionFormats();
        }

        // Determine operation mode
        bool downloadAll = parser.isSet("all");
        bool downloadOnly = parser.isSet("download-only");
        bool hasInput = parser.isSet("input");
        bool specificBook = parser.isSet("book");
        bool interactive = parser.isSet("interactive");
        bool useCalibreDRM = parser.isSet("use-calibre-drm");
        int batchSize = parser.value("batch-size").toInt();

        QStringList filesToProcess;

        // Mode 1: Download from Kindle
        if (downloadAll || downloadOnly || specificBook) {
            std::cout << "Connecting to Amazon Kindle..." << std::endl;

            m_downloader = new KindleDownloader(
                m_configManager->amazonEmail(),
                m_configManager->amazonPassword(),
                m_configManager->amazonRegion(),
                m_configManager->downloadDirectory(),
                this
            );

            connect(m_downloader, &KindleDownloader::progressUpdate,
                   this, &KindleDRMConverter::onProgress);
            connect(m_downloader, &KindleDownloader::loginSuccess,
                   this, &KindleDRMConverter::onLoginSuccess);
            connect(m_downloader, &KindleDownloader::loginFailed,
                   this, &KindleDRMConverter::onLoginFailed);

            // Start login
            m_downloader->login(interactive);

            // The rest will be handled by signals/slots
        }
        // Mode 2: Process existing files
        else if (hasInput) {
            QString inputPath = parser.value("input");
            QFileInfo fileInfo(inputPath);

            if (!fileInfo.exists()) {
                std::cerr << "Error: Input path not found: "
                         << inputPath.toStdString() << std::endl;
                QCoreApplication::exit(1);
                return;
            }

            if (fileInfo.isFile()) {
                if (isKindleFormat(inputPath)) {
                    filesToProcess << inputPath;
                } else {
                    std::cerr << "Error: Not a Kindle format: "
                             << inputPath.toStdString() << std::endl;
                    QCoreApplication::exit(1);
                    return;
                }
            } else {
                // Directory - find all Kindle files
                QStringList kindleExts = {"*.azw", "*.azw3", "*.mobi", "*.kfx"};
                QDir dir(inputPath);

                for (const QString &ext : kindleExts) {
                    filesToProcess << dir.entryList(QStringList() << ext,
                                                   QDir::Files,
                                                   QDir::Name);
                }
            }

            std::cout << "Found " << filesToProcess.size()
                     << " Kindle file(s) to process" << std::endl;

            if (!filesToProcess.isEmpty()) {
                processFiles(filesToProcess, formats, useCalibreDRM);
            }
        }
        else {
            std::cerr << "Error: Please specify --all, --download-only, --input, or --book"
                     << std::endl;
            std::cerr << "Run with --help for usage information" << std::endl;
            QCoreApplication::exit(1);
        }
    }

private slots:
    void onProgress(const QString &message)
    {
        std::cout << message.toStdString() << std::endl;
    }

    void onLoginSuccess()
    {
        std::cout << "✓ Login successful" << std::endl;
    }

    void onLoginFailed(const QString &error)
    {
        std::cerr << "✗ Login failed: " << error.toStdString() << std::endl;
        QCoreApplication::exit(1);
    }

private:
    ConfigManager *m_configManager;
    KindleDownloader *m_downloader;
    DRMRemover *m_drmRemover;
    EbookConverter *m_converter;

    void printHeader()
    {
        std::cout << "============================================================" << std::endl;
        std::cout << "Kindle DRM Converter (Qt C++ Version)" << std::endl;
        std::cout << "============================================================" << std::endl;
        std::cout << std::endl;
    }

    bool isKindleFormat(const QString &filepath)
    {
        QStringList kindleFormats = {".azw", ".azw3", ".azw4", ".mobi", ".prc", ".kfx"};
        QFileInfo fileInfo(filepath);
        QString ext = "." + fileInfo.suffix().toLower();
        return kindleFormats.contains(ext);
    }

    void processFiles(const QStringList &files,
                     const QStringList &formats,
                     bool useCalibreDRM)
    {
        std::cout << "\nStarting DRM removal and conversion..." << std::endl;
        std::cout << "Output formats: " << formats.join(", ").toStdString() << std::endl;
        std::cout << std::endl;

        // Initialize processors
        m_drmRemover = new DRMRemover(
            m_configManager->kindleSerial(),
            m_configManager->kindlePID(),
            m_configManager->tempDirectory(),
            this
        );

        m_converter = new EbookConverter(
            m_configManager->outputDirectory(),
            formats,
            m_configManager->conversionQuality(),
            this
        );

        connect(m_drmRemover, &DRMRemover::progressUpdate,
               this, &KindleDRMConverter::onProgress);
        connect(m_converter, &EbookConverter::progressUpdate,
               this, &KindleDRMConverter::onProgress);

        int totalFiles = files.size();
        int drmRemoved = 0;
        int converted = 0;
        int failed = 0;

        // Process each file
        for (int i = 0; i < files.size(); i++) {
            const QString &filepath = files[i];

            std::cout << "\n============================================================" << std::endl;
            std::cout << "Processing: " << QFileInfo(filepath).fileName().toStdString() << std::endl;
            std::cout << "============================================================" << std::endl;

            // Step 1: Remove DRM
            QString drmFreeFile = m_drmRemover->removeDRM(filepath);

            if (drmFreeFile.isEmpty()) {
                std::cout << "⚠ DRM removal failed for "
                         << QFileInfo(filepath).fileName().toStdString() << std::endl;
                failed++;
                continue;
            }

            drmRemoved++;
            std::cout << "✓ DRM removed successfully" << std::endl;

            // Step 2: Convert to formats
            QMap<QString, QString> convertedFiles = m_converter->convert(drmFreeFile, formats);

            if (!convertedFiles.isEmpty()) {
                converted++;
                std::cout << "✓ Converted to " << convertedFiles.size()
                         << " format(s)" << std::endl;

                for (auto it = convertedFiles.begin(); it != convertedFiles.end(); ++it) {
                    std::cout << "  - " << it.key().toUpper().toStdString()
                             << ": " << it.value().toStdString() << std::endl;
                }
            } else {
                std::cout << "⚠ Conversion failed" << std::endl;
                failed++;
            }
        }

        // Print summary
        std::cout << "\n============================================================" << std::endl;
        std::cout << "SUMMARY" << std::endl;
        std::cout << "============================================================" << std::endl;
        std::cout << "Total files processed: " << totalFiles << std::endl;
        std::cout << "DRM removed: " << drmRemoved << std::endl;
        std::cout << "Successfully converted: " << converted << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << std::endl;
        std::cout << "Output directory: "
                 << m_configManager->outputDirectory().toStdString() << std::endl;
        std::cout << "============================================================" << std::endl;

        QCoreApplication::exit(0);
    }

    void cleanup()
    {
        if (m_downloader) delete m_downloader;
        if (m_drmRemover) delete m_drmRemover;
        if (m_converter) delete m_converter;
        if (m_configManager) delete m_configManager;
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("Kindle DRM Converter");
    QCoreApplication::setApplicationVersion("1.0.0");

    QCommandLineParser parser;
    parser.setApplicationDescription(
        "Kindle DRM Converter - Download, remove DRM, and convert Kindle books\n\n"
        "Examples:\n"
        "  # Process all Kindle books\n"
        "  kindle-converter --all\n\n"
        "  # Download only\n"
        "  kindle-converter --download-only\n\n"
        "  # Process existing files\n"
        "  kindle-converter --input /path/to/kindle/files\n\n"
        "  # Convert to specific formats\n"
        "  kindle-converter --all --formats epub,pdf,mobi"
    );

    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOption({{"a", "all"}, "Download, remove DRM, and convert all Kindle books"});
    parser.addOption({{"d", "download-only"}, "Only download books without DRM removal or conversion"});
    parser.addOption({{"i", "input"}, "Process existing Kindle files from this directory or file", "path"});
    parser.addOption({{"o", "output"}, "Output directory for converted books", "path"});
    parser.addOption({{"f", "formats"}, "Comma-separated list of output formats (e.g., epub,pdf,mobi)", "formats"});
    parser.addOption({{"b", "book"}, "Process specific book by title", "title"});
    parser.addOption({{"batch-size"}, "Number of books to process simultaneously", "size", "3"});
    parser.addOption({{"c", "config"}, "Path to configuration file", "path", "config.yaml"});
    parser.addOption({{"keep-drm"}, "Keep original DRM-protected files"});
    parser.addOption({{"interactive"}, "Run in interactive mode (for CAPTCHA/2FA)"});
    parser.addOption({{"v", "verbose"}, "Enable verbose logging"});
    parser.addOption({{"use-calibre-drm"}, "Use Calibre DeDRM plugin for DRM removal"});

    parser.process(app);

    KindleDRMConverter converter;
    QTimer::singleShot(0, [&]() {
        converter.run(parser);
    });

    return app.exec();
}

#include "main.moc"
