#ifndef DRMREMOVER_H
#define DRMREMOVER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>

class DRMRemover : public QObject
{
    Q_OBJECT

public:
    explicit DRMRemover(const QString &kindleSerial,
                       const QString &kindlePID,
                       const QString &outputDir,
                       QObject *parent = nullptr);
    ~DRMRemover();

    QString removeDRM(const QString &filepath);
    QStringList batchRemoveDRM(const QStringList &files);

signals:
    void progressUpdate(const QString &message);
    void drmRemoved(const QString &outputPath);
    void drmRemovalFailed(const QString &filepath, const QString &error);

private:
    QString m_kindleSerial;
    QString m_kindlePID;
    QString m_outputDir;

    static const QStringList KINDLE_FORMATS;

    bool hasDRM(const QByteArray &data);
    QString removeMobiDRM(const QString &filepath);
    QString removeAZW3DRM(const QString &filepath);
    QString removeKFXDRM(const QString &filepath);
    QList<QByteArray> generateKindleKeys();
    QByteArray generateKeyFromSerial(const QString &serial);
    QByteArray generateKeyFromPID(const QString &pid);
    QByteArray decryptMobi(const QByteArray &data, const QByteArray &key);
    QByteArray decryptAES(const QByteArray &data, const QByteArray &key);
};

#endif // DRMREMOVER_H
