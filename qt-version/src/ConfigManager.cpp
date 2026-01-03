#include "ConfigManager.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
{
    setDefaults();
}

ConfigManager::~ConfigManager()
{
}

void ConfigManager::setDefaults()
{
    m_amazonRegion = "US";
    m_downloadDir = "./downloads";
    m_outputDir = "./output";
    m_tempDir = "./temp";
    m_conversionFormats = QStringList() << "epub" << "pdf" << "mobi";
    m_conversionQuality = "high";
}

bool ConfigManager::loadConfig(const QString &configPath)
{
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open config file:" << configPath;
        return false;
    }

    QByteArray fileData = file.readAll();
    file.close();

    // Simple YAML parser (for basic key-value pairs)
    // For production, consider using a proper YAML library like yaml-cpp
    QTextStream stream(fileData);
    QString section;

    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();

        // Skip empty lines and comments
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }

        // Detect sections
        if (line.endsWith(':') && !line.contains(' ')) {
            section = line.left(line.length() - 1);
            continue;
        }

        // Parse key-value pairs
        QStringList parts = line.split(':', QString::SkipEmptyParts);
        if (parts.size() >= 2) {
            QString key = parts[0].trimmed();
            QString value = parts[1].trimmed();
            value = value.remove('"').remove('\'');

            // Amazon settings
            if (section == "amazon") {
                if (key == "email") m_amazonEmail = value;
                else if (key == "password") m_amazonPassword = value;
                else if (key == "region") m_amazonRegion = value;
            }
            // Directory settings
            else if (section == "directories") {
                if (key == "download") m_downloadDir = value;
                else if (key == "output") m_outputDir = value;
                else if (key == "temp") m_tempDir = value;
            }
            // DRM settings
            else if (section == "drm") {
                if (key == "kindle_serial") m_kindleSerial = value;
                else if (key == "pid") m_kindlePID = value;
            }
            // Conversion settings
            else if (section == "conversion") {
                if (key == "quality") m_conversionQuality = value;
            }
        }

        // Handle format lists
        if (section == "conversion" && line.startsWith('-')) {
            QString format = line.mid(1).trimmed();
            if (!format.isEmpty()) {
                m_conversionFormats.append(format);
            }
        }
    }

    // Create directories if they don't exist
    QDir().mkpath(m_downloadDir);
    QDir().mkpath(m_outputDir);
    QDir().mkpath(m_tempDir);

    qDebug() << "Configuration loaded successfully";
    return true;
}
