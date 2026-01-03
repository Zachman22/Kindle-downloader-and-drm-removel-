#include "EbookConverter.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QThreadPool>
#include <QtConcurrent>

const QStringList EbookConverter::SUPPORTED_FORMATS = {
    "epub", "pdf", "mobi", "azw3", "txt", "html", "rtf", "odt", "docx",
    "fb2", "lit", "lrf", "pdb", "pml", "rb", "snb", "tcr"
};

EbookConverter::EbookConverter(const QString &outputDir,
                              const QStringList &formats,
                              const QString &quality,
                              QObject *parent)
    : QObject(parent)
    , m_outputDir(outputDir)
    , m_formats(formats)
    , m_quality(quality)
{
    QDir().mkpath(m_outputDir);

    if (!checkCalibre()) {
        qWarning() << "Calibre is not installed or not in PATH!";
    }
}

EbookConverter::~EbookConverter()
{
}

bool EbookConverter::checkCalibre()
{
    QProcess process;
    process.start("ebook-convert", QStringList() << "--version");
    process.waitForFinished(5000);

    if (process.exitCode() == 0) {
        QString output = process.readAllStandardOutput();
        qDebug() << "Calibre found:" << output.trimmed();
        emit progressUpdate(QString("Calibre found: %1").arg(output.trimmed()));
        return true;
    }

    qWarning() << "Calibre not found. Please install Calibre.";
    return false;
}

QMap<QString, QString> EbookConverter::convert(const QString &inputFile,
                                               const QStringList &outputFormats)
{
    QFileInfo fileInfo(inputFile);
    if (!fileInfo.exists()) {
        qWarning() << "Input file not found:" << inputFile;
        return QMap<QString, QString>();
    }

    QStringList formats = outputFormats.isEmpty() ? m_formats : outputFormats;

    qDebug() << "Converting" << fileInfo.fileName() << "to formats:" << formats.join(", ");
    emit progressUpdate(QString("Converting %1 to: %2")
                       .arg(fileInfo.fileName(), formats.join(", ")));

    // Create output directory for this book
    QString bookDir = QString("%1/%2").arg(m_outputDir, fileInfo.baseName());
    QDir().mkpath(bookDir);

    // Convert to each format
    QMap<QString, QString> convertedFiles;
    for (const QString &format : formats) {
        QString outputFile = convertToFormat(inputFile, bookDir, format);
        if (!outputFile.isEmpty()) {
            convertedFiles[format] = outputFile;
            emit conversionComplete(inputFile, format, outputFile);
        } else {
            emit conversionFailed(inputFile, format, "Conversion failed");
        }
    }

    qDebug() << QString("Successfully converted to %1/%2 formats")
                .arg(convertedFiles.size())
                .arg(formats.size());

    return convertedFiles;
}

QString EbookConverter::convertToFormat(const QString &inputFile,
                                       const QString &outputDir,
                                       const QString &outputFormat)
{
    QString format = outputFormat.toLower();
    if (format.startsWith('.')) {
        format = format.mid(1);
    }

    if (!SUPPORTED_FORMATS.contains(format)) {
        qWarning() << "Unsupported format:" << format;
        return QString();
    }

    QFileInfo fileInfo(inputFile);
    QString outputFile = QString("%1/%2.%3")
                        .arg(outputDir, fileInfo.baseName(), format);

    qDebug() << "Converting to" << format.toUpper() << "...";
    emit progressUpdate(QString("Converting to %1...").arg(format.toUpper()));

    // Build ebook-convert command
    QStringList args;
    args << inputFile << outputFile;

    // Add format-specific options
    args.append(getFormatOptions(format));

    QProcess process;
    process.start("ebook-convert", args);
    process.waitForFinished(300000); // 5 minute timeout

    if (process.exitCode() == 0 && QFile::exists(outputFile)) {
        qDebug() << "Successfully converted to" << format.toUpper() << ":" << outputFile;
        emit progressUpdate(QString("Converted to %1").arg(format.toUpper()));
        return outputFile;
    } else {
        QString error = process.readAllStandardError();
        qWarning() << "Conversion to" << format << "failed:" << error;
        return QString();
    }
}

QStringList EbookConverter::getFormatOptions(const QString &outputFormat)
{
    QStringList options;

    // Common options
    options << "--chapter" << "/"
           << "--page-breaks-before" << "/"
           << "--preserve-cover-aspect-ratio";

    // Format-specific options
    if (outputFormat == "epub") {
        options << "--epub-version" << "3"
               << "--flow-size" << "0"
               << "--no-default-epub-cover";
    }
    else if (outputFormat == "pdf") {
        if (m_quality == "high") {
            options << "--paper-size" << "letter"
                   << "--pdf-page-numbers"
                   << "--pdf-sans-family" << "Arial"
                   << "--pdf-serif-family" << "Times New Roman"
                   << "--pdf-mono-family" << "Courier New";
        }
    }
    else if (outputFormat == "mobi") {
        options << "--mobi-file-type" << "both"
               << "--no-inline-toc";
    }
    else if (outputFormat == "azw3") {
        options << "--mobi-file-type" << "new";
    }
    else if (outputFormat == "txt") {
        options << "--txt-output-formatting" << "markdown"
               << "--max-line-length" << "0"
               << "--txt-output-encoding" << "utf-8";
    }
    else if (outputFormat == "docx") {
        options << "--docx-page-size" << "letter";
    }

    return options;
}

QMap<QString, QMap<QString, QString>> EbookConverter::batchConvert(
    const QStringList &inputFiles,
    const QStringList &outputFormats,
    int maxWorkers)
{
    qDebug() << "Batch converting" << inputFiles.size() << "files...";
    emit progressUpdate(QString("Batch converting %1 files...").arg(inputFiles.size()));

    QMap<QString, QMap<QString, QString>> results;

    // Convert each file sequentially (parallel conversion can be added with QtConcurrent)
    for (const QString &inputFile : inputFiles) {
        QMap<QString, QString> converted = convert(inputFile, outputFormats);
        results[inputFile] = converted;
    }

    int successful = 0;
    for (const auto &fileResults : results) {
        if (!fileResults.isEmpty()) {
            successful++;
        }
    }

    qDebug() << QString("Batch conversion complete: %1/%2 successful")
                .arg(successful)
                .arg(inputFiles.size());

    return results;
}

QMap<QString, QString> EbookConverter::getMetadata(const QString &filepath)
{
    QMap<QString, QString> metadata;

    QProcess process;
    process.start("ebook-meta", QStringList() << filepath);
    process.waitForFinished(10000);

    if (process.exitCode() == 0) {
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split('\n');

        for (const QString &line : lines) {
            if (line.contains(':')) {
                QStringList parts = line.split(':', QString::SkipEmptyParts);
                if (parts.size() >= 2) {
                    QString key = parts[0].trimmed().toLower();
                    QString value = parts[1].trimmed();
                    metadata[key] = value;
                }
            }
        }
    }

    return metadata;
}

void EbookConverter::embedMetadata(const QMap<QString, QString> &files,
                                  const QMap<QString, QString> &metadata)
{
    for (auto it = files.begin(); it != files.end(); ++it) {
        const QString &filepath = it.value();

        qDebug() << "Embedding metadata in" << QFileInfo(filepath).fileName();

        QStringList args;
        args << filepath;

        if (metadata.contains("title")) {
            args << "--title" << metadata["title"];
        }
        if (metadata.contains("author")) {
            args << "--authors" << metadata["author"];
        }
        if (metadata.contains("publisher")) {
            args << "--publisher" << metadata["publisher"];
        }
        if (metadata.contains("date")) {
            args << "--date" << metadata["date"];
        }
        if (metadata.contains("language")) {
            args << "--language" << metadata["language"];
        }
        if (metadata.contains("tags")) {
            args << "--tags" << metadata["tags"];
        }

        QProcess process;
        process.start("ebook-meta", args);
        process.waitForFinished(10000);

        if (process.exitCode() != 0) {
            qWarning() << "Could not embed metadata in" << filepath;
        }
    }
}
