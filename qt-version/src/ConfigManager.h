#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager();

    bool loadConfig(const QString &configPath);

    // Amazon settings
    QString amazonEmail() const { return m_amazonEmail; }
    QString amazonPassword() const { return m_amazonPassword; }
    QString amazonRegion() const { return m_amazonRegion; }

    // Directory settings
    QString downloadDirectory() const { return m_downloadDir; }
    QString outputDirectory() const { return m_outputDir; }
    QString tempDirectory() const { return m_tempDir; }

    // DRM settings
    QString kindleSerial() const { return m_kindleSerial; }
    QString kindlePID() const { return m_kindlePID; }

    // Conversion settings
    QStringList conversionFormats() const { return m_conversionFormats; }
    QString conversionQuality() const { return m_conversionQuality; }

private:
    QString m_amazonEmail;
    QString m_amazonPassword;
    QString m_amazonRegion;

    QString m_downloadDir;
    QString m_outputDir;
    QString m_tempDir;

    QString m_kindleSerial;
    QString m_kindlePID;

    QStringList m_conversionFormats;
    QString m_conversionQuality;

    void setDefaults();
};

#endif // CONFIGMANAGER_H
