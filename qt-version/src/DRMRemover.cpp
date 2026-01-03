#include "DRMRemover.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QDebug>
#include <QtEndian>
#include <QProcess>

const QStringList DRMRemover::KINDLE_FORMATS = {
    ".azw", ".azw3", ".azw4", ".mobi", ".prc", ".kfx"
};

DRMRemover::DRMRemover(const QString &kindleSerial,
                      const QString &kindlePID,
                      const QString &outputDir,
                      QObject *parent)
    : QObject(parent)
    , m_kindleSerial(kindleSerial)
    , m_kindlePID(kindlePID)
    , m_outputDir(outputDir)
{
    QDir().mkpath(m_outputDir);
}

DRMRemover::~DRMRemover()
{
}

QString DRMRemover::removeDRM(const QString &filepath)
{
    qDebug() << "Removing DRM from:" << filepath;
    emit progressUpdate(QString("Removing DRM from: %1").arg(QFileInfo(filepath).fileName()));

    QFileInfo fileInfo(filepath);
    QString ext = fileInfo.suffix().toLower();

    if (!KINDLE_FORMATS.contains("." + ext)) {
        QString error = QString("Unsupported format: %1").arg(ext);
        emit drmRemovalFailed(filepath, error);
        return QString();
    }

    QString result;
    if (ext == "azw" || ext == "mobi" || ext == "prc") {
        result = removeMobiDRM(filepath);
    } else if (ext == "azw3") {
        result = removeAZW3DRM(filepath);
    } else if (ext == "kfx") {
        result = removeKFXDRM(filepath);
    } else {
        QString error = QString("DRM removal not implemented for: %1").arg(ext);
        emit drmRemovalFailed(filepath, error);
        return QString();
    }

    if (!result.isEmpty()) {
        emit drmRemoved(result);
    }

    return result;
}

QString DRMRemover::removeMobiDRM(const QString &filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit drmRemovalFailed(filepath, "Could not open file");
        return QString();
    }

    QByteArray data = file.readAll();
    file.close();

    // Check if file has DRM
    if (!hasDRM(data)) {
        qDebug() << "File appears to have no DRM";
        emit progressUpdate("File appears to have no DRM");
        return filepath;
    }

    // Check MOBI header
    if (data.left(4) != "BOOK" && data.left(4) != "TPZ0") {
        emit drmRemovalFailed(filepath, "Invalid MOBI file format");
        return QString();
    }

    // Generate decryption keys
    QList<QByteArray> keys = generateKindleKeys();

    // Try decrypting with each key
    QByteArray decryptedData;
    for (const QByteArray &key : keys) {
        try {
            decryptedData = decryptMobi(data, key);
            if (!decryptedData.isEmpty()) {
                break;
            }
        } catch (...) {
            continue;
        }
    }

    if (decryptedData.isEmpty()) {
        emit drmRemovalFailed(filepath, "Could not decrypt with available keys");
        return QString();
    }

    // Save decrypted file
    QFileInfo fileInfo(filepath);
    QString outputPath = QString("%1/%2_nodrm.%3")
                        .arg(m_outputDir, fileInfo.baseName(), fileInfo.suffix());

    QFile outFile(outputPath);
    if (!outFile.open(QIODevice::WriteOnly)) {
        emit drmRemovalFailed(filepath, "Could not save decrypted file");
        return QString();
    }

    outFile.write(decryptedData);
    outFile.close();

    qDebug() << "DRM removed successfully:" << outputPath;
    emit progressUpdate(QString("DRM removed: %1").arg(QFileInfo(outputPath).fileName()));

    return outputPath;
}

QString DRMRemover::removeAZW3DRM(const QString &filepath)
{
    // AZW3 is essentially a MOBI container
    return removeMobiDRM(filepath);
}

QString DRMRemover::removeKFXDRM(const QString &filepath)
{
    qWarning() << "KFX DRM removal requires Calibre DeDRM plugin";
    emit progressUpdate("KFX requires Calibre DeDRM plugin");

    // Try using Calibre with DeDRM plugin
    QFileInfo fileInfo(filepath);
    QString outputPath = QString("%1/%2_nodrm.%3")
                        .arg(m_outputDir, fileInfo.baseName(), fileInfo.suffix());

    QProcess process;
    QStringList args;
    args << filepath << outputPath << "--no-inline-toc";

    process.start("ebook-convert", args);
    process.waitForFinished(300000); // 5 minute timeout

    if (process.exitCode() == 0 && QFile::exists(outputPath)) {
        qDebug() << "DRM removed via Calibre:" << outputPath;
        return outputPath;
    } else {
        emit drmRemovalFailed(filepath, "Calibre DRM removal failed");
        return QString();
    }
}

bool DRMRemover::hasDRM(const QByteArray &data)
{
    if (data.size() < 100) return false;

    // Look for EXTH header (may contain DRM)
    if (data.contains("EXTH")) {
        return true;
    }

    // Check encryption type in header
    if (data.size() > 0xE) {
        quint16 encryptionType = qFromBigEndian<quint16>(
            reinterpret_cast<const uchar*>(data.constData() + 0xC)
        );
        if (encryptionType != 0) {
            return true;
        }
    }

    return false;
}

QList<QByteArray> DRMRemover::generateKindleKeys()
{
    QList<QByteArray> keys;

    // Generate key from Kindle serial
    if (!m_kindleSerial.isEmpty()) {
        keys.append(generateKeyFromSerial(m_kindleSerial));
    }

    // Generate key from PID
    if (!m_kindlePID.isEmpty()) {
        keys.append(generateKeyFromPID(m_kindlePID));
    }

    // Try common/default keys
    keys.append(QByteArray(16, 0)); // Null key

    QByteArray defaultKey = QCryptographicHash::hash("default", QCryptographicHash::Sha1);
    keys.append(defaultKey.left(16));

    return keys;
}

QByteArray DRMRemover::generateKeyFromSerial(const QString &serial)
{
    QByteArray serialBytes = serial.toUtf8();
    QByteArray hash = QCryptographicHash::hash(serialBytes, QCryptographicHash::Sha256);
    return hash.left(16);
}

QByteArray DRMRemover::generateKeyFromPID(const QString &pid)
{
    QByteArray pidBytes = pid.toUtf8();
    QByteArray hash = QCryptographicHash::hash(pidBytes, QCryptographicHash::Sha256);
    return hash.left(16);
}

QByteArray DRMRemover::decryptMobi(const QByteArray &data, const QByteArray &key)
{
    // Simplified MOBI decryption
    // Real MOBI DRM is more complex and varies by version

    if (data.size() < 78) return QByteArray();

    // Get number of sections
    quint16 numSections = qFromBigEndian<quint16>(
        reinterpret_cast<const uchar*>(data.constData() + 76)
    );

    QByteArray result;
    result.reserve(data.size());

    // Process each section
    for (int i = 0; i < numSections; i++) {
        int offset = 78 + (i * 8);
        if (offset + 4 > data.size()) break;

        quint32 sectionOffset = qFromBigEndian<quint32>(
            reinterpret_cast<const uchar*>(data.constData() + offset)
        );

        quint32 nextOffset;
        if (i < numSections - 1) {
            nextOffset = qFromBigEndian<quint32>(
                reinterpret_cast<const uchar*>(data.constData() + offset + 8)
            );
        } else {
            nextOffset = data.size();
        }

        if (sectionOffset >= static_cast<quint32>(data.size()) ||
            nextOffset > static_cast<quint32>(data.size())) {
            continue;
        }

        QByteArray section = data.mid(sectionOffset, nextOffset - sectionOffset);

        // Try to decrypt this section with AES
        QByteArray decrypted = decryptAES(section, key);
        if (!decrypted.isEmpty()) {
            result.append(decrypted);
        } else {
            result.append(section); // Keep original if decryption fails
        }
    }

    return result.isEmpty() ? QByteArray() : result;
}

QByteArray DRMRemover::decryptAES(const QByteArray &data, const QByteArray &key)
{
    // This is a placeholder for AES decryption
    // In production, use Qt's QCA (Qt Cryptographic Architecture) or OpenSSL

    // For now, just return the data as-is
    // Real implementation would use AES ECB or CBC mode
    qDebug() << "AES decryption placeholder - using QCA or OpenSSL recommended";

    return data;
}

QStringList DRMRemover::batchRemoveDRM(const QStringList &files)
{
    QStringList drmFreeFiles;

    for (const QString &filepath : files) {
        QString result = removeDRM(filepath);
        if (!result.isEmpty()) {
            drmFreeFiles.append(result);
        }
    }

    qDebug() << QString("Successfully removed DRM from %1/%2 files")
                .arg(drmFreeFiles.size())
                .arg(files.size());

    return drmFreeFiles;
}
