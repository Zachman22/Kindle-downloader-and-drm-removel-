#ifndef EBOOKCONVERTER_H
#define EBOOKCONVERTER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QProcess>

class EbookConverter : public QObject
{
    Q_OBJECT

public:
    explicit EbookConverter(const QString &outputDir,
                           const QStringList &formats,
                           const QString &quality,
                           QObject *parent = nullptr);
    ~EbookConverter();

    QMap<QString, QString> convert(const QString &inputFile,
                                    const QStringList &outputFormats = QStringList());
    QMap<QString, QMap<QString, QString>> batchConvert(const QStringList &inputFiles,
                                                        const QStringList &outputFormats = QStringList(),
                                                        int maxWorkers = 3);

    bool checkCalibre();
    QMap<QString, QString> getMetadata(const QString &filepath);

signals:
    void progressUpdate(const QString &message);
    void conversionComplete(const QString &inputFile, const QString &format, const QString &outputFile);
    void conversionFailed(const QString &inputFile, const QString &format, const QString &error);

private:
    QString m_outputDir;
    QStringList m_formats;
    QString m_quality;

    static const QStringList SUPPORTED_FORMATS;

    QString convertToFormat(const QString &inputFile,
                          const QString &outputDir,
                          const QString &outputFormat);
    QStringList getFormatOptions(const QString &outputFormat);
    void embedMetadata(const QMap<QString, QString> &files,
                      const QMap<QString, QString> &metadata);
};

#endif // EBOOKCONVERTER_H
