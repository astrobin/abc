/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of libabc.
 *
 * libabc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libabc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libabc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "debug.h"
#include "site.h"
#include "upload-item.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QHttpMultiPart>
#include <QNetworkReply>

using namespace ABC;

namespace ABC {

class UploadItemPrivate: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(UploadItem)

    UploadItemPrivate(const QString &filePath,
                      const QString &fileName,
                      UploadItem *q);
    void startUpload(Site *site);

    bool checkReply(QNetworkReply *reply);
    void computeHash();
    void updateProgress(int value);

private Q_SLOTS:
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void onFinished();

private:
    QString filePath;
    QString fileName;
    QDir baseDir;
    QByteArray fileHash;
    int progress;
    Site::ErrorCode lastError;
    QString lastErrorMessage;
    mutable UploadItem *q_ptr;
};

} // namespace

UploadItemPrivate::UploadItemPrivate(const QString &filePath,
                                     const QString &fileName,
                                     UploadItem *q):
    filePath(filePath),
    fileName(fileName),
    progress(0),
    lastError(Site::NoError),
    q_ptr(q)
{
}

void UploadItemPrivate::computeHash()
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    hash.addData(file.readAll());
    fileHash = hash.result().toHex();
}

void UploadItemPrivate::startUpload(Site *site)
{
    computeHash();

    QList<QHttpPart> parts;

    QHttpPart hashPart;
    hashPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QByteArray("form-data; name=\"file_hash\""));
    hashPart.setBody(fileHash);
    parts.append(hashPart);

    QHttpPart pathPart;
    pathPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QByteArray("form-data; name=\"original_path\""));
    pathPart.setBody(fileName.toUtf8());
    parts.append(pathPart);

    QNetworkReply *reply = site->uploadFile(filePath, parts);
    Q_ASSERT(reply != 0);

    QObject::connect(reply, SIGNAL(uploadProgress(qint64, qint64)),
                     this, SLOT(onUploadProgress(qint64, qint64)));
    QObject::connect(reply, SIGNAL(finished()),
                     this, SLOT(onFinished()));
}

bool UploadItemPrivate::checkReply(QNetworkReply *reply)
{
    QByteArray replyContent = reply->readAll();

    uint statusCode =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();
    if (statusCode != 200) {
        QVariantMap response = Site::parseJson(replyContent);
        if (response.contains("detail")) {
            lastErrorMessage = response["detail"].toString();
        }

        DEBUG() << statusCode << lastErrorMessage;
        switch (statusCode) {
        case 403:
            lastError = Site::QuotaExceededError;
            break;
        case 415:
            lastError = Site::WrongFileType;
            break;
        default:
            lastError = Site::NetworkError;
            break;
        }
        return false;
    }

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Network error while uploading " << fileName <<
            ":" << reply->errorString() << " / " << replyContent;
        // TODO: better error handling
        lastError = Site::NetworkError;
        return false;
    }

    DEBUG() << "Upload reply:" << replyContent;
    return true;
}

void UploadItemPrivate::onUploadProgress(qint64 bytesSent,
                                         qint64 bytesTotal)
{
    DEBUG() << bytesSent << "out of" << bytesTotal;

    int progress = (bytesTotal > 0) ? bytesSent * 100 / bytesTotal : 0;
    /* Let's keep 100% for confirmed uploads only */
    if (progress >= 100) progress = 99;
    updateProgress(progress);
}

void UploadItemPrivate::onFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply != 0);

    updateProgress(checkReply(reply) ? 100 : -1);

    reply->deleteLater();
}

void UploadItemPrivate::updateProgress(int value)
{
    Q_Q(UploadItem);
    if (value != progress) {
        progress = value;
        Q_EMIT q->progressChanged(progress);
    }
}

UploadItem::UploadItem(const QString &filePath,
                       const QString &fileName,
                       QObject *parent):
    QObject(parent),
    d_ptr(new UploadItemPrivate(filePath, fileName, this))
{
}

UploadItem::~UploadItem()
{
    delete d_ptr;
    d_ptr = 0;
}

QString UploadItem::fileName() const
{
    Q_D(const UploadItem);
    return d->fileName;
}

QString UploadItem::filePath() const
{
    Q_D(const UploadItem);
    return d->filePath;
}

QByteArray UploadItem::fileHash() const
{
    Q_D(const UploadItem);
    return d->fileHash;
}

int UploadItem::progress() const
{
    Q_D(const UploadItem);
    return d->progress;
}

Site::ErrorCode UploadItem::lastError() const
{
    Q_D(const UploadItem);
    return d->lastError;
}

QString UploadItem::lastErrorMessage() const
{
    Q_D(const UploadItem);
    return d->lastErrorMessage;
}

bool UploadItem::errorIsRecoverable() const
{
    Q_D(const UploadItem);
    switch (d->lastError) {
    case Site::NetworkError:
        return true;
    default:
        break;
    }
    return false;
}

void UploadItem::startUpload(Site *site)
{
    Q_D(UploadItem);
    d->startUpload(site);
}

#include "upload-item.moc"
