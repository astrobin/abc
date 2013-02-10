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

#include <QFile>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslError>
#include <QUrl>
#include <QVariantMap>

#include <qjson/parser.h>

#define API_BASE_URL "https://www.astrobin.com/api/v2"

static const QLatin1String API_URL(API_BASE_URL);
static const QLatin1String AUTH_URL(API_BASE_URL "/api-auth-token/");
static const QLatin1String UPLOAD_URL(API_BASE_URL "/rawdata/rawimages/");

using namespace ABC;

namespace ABC {

class SitePrivate: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(Site)

    SitePrivate(Site *q);

    inline void ensureHasNetworkAccessManager();
    void authenticate();
    QVariantMap parseJson(const QByteArray &data);
    void setError(Site::ErrorCode code,
                  const QString &message = QString());
    bool handleNetworkError(QNetworkReply *reply);
    QByteArray accessTokenFromReply(QNetworkReply *reply);

    QNetworkReply *uploadFile(const QString &filePath,
                              const QList<QHttpPart> &extraParts);

private Q_SLOTS:
    void onAuthenticateReply();
    void onNetworkError(QNetworkReply::NetworkError error);
    void onSslErrors(QList<QSslError> errors);

private:
    QString userName;
    QString password;
    QByteArray accessToken;
    bool isAuthenticating;
    QNetworkAccessManager *nam;

    Site::ErrorCode lastError;
    QString lastErrorMessage;

    mutable Site *q_ptr;
};

} // namespace

SitePrivate::SitePrivate(Site *q):
    QObject(q),
    isAuthenticating(false),
    nam(0),
    q_ptr(q)
{
}

void SitePrivate::ensureHasNetworkAccessManager()
{
    if (nam != 0) return;

    nam = new QNetworkAccessManager(this);
}

void SitePrivate::authenticate()
{
    if (isAuthenticating) return;
    isAuthenticating = true;

    ensureHasNetworkAccessManager();

    // Clear any existing access token
    accessToken.clear();

    QUrl authUrl(AUTH_URL);
    QNetworkRequest request(authUrl);
    request.setRawHeader("Content-Type",
                         "application/x-www-form-urlencoded");
    QUrl data;
    data.addQueryItem("username", userName);
    data.addQueryItem("password", password);

    QNetworkReply *reply = nam->post(request, data.encodedQuery());
    reply->ignoreSslErrors();
    connect(reply, SIGNAL(finished()),
            this, SLOT(onAuthenticateReply()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onNetworkError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(onSslErrors(QList<QSslError>)));
}

QVariantMap SitePrivate::parseJson(const QByteArray &data)
{
    bool ok = false;
    QJson::Parser parser;
    QVariant tree = parser.parse(data, &ok);
    if (ok) {
        return tree.toMap();
    }
    return QVariantMap();
}

void SitePrivate::setError(Site::ErrorCode code, const QString &message)
{
    Q_Q(Site);
    lastError = code;
    lastErrorMessage = message;
    Q_EMIT q->error(code);
}

QByteArray SitePrivate::accessTokenFromReply(QNetworkReply *reply)
{
    QByteArray replyContent = reply->readAll();
    DEBUG() << replyContent;

    uint statusCode =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();
    if (statusCode != 200) {
        DEBUG() << "Status code:" << statusCode;
        setError(Site::NetworkError,
                 QString("Got HTTP code %1").arg(statusCode));
        return QByteArray();
    }

    QVariantMap response = parseJson(replyContent);
    if (response.contains("token")) {
        return response["token"].toByteArray();
    }

    return QByteArray();
}

void SitePrivate::onAuthenticateReply()
{
    Q_Q(Site);

    isAuthenticating = false;
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply != 0);

    QByteArray token = accessTokenFromReply(reply);
    reply->deleteLater();

    q->setAccessToken(token);
}

bool SitePrivate::handleNetworkError(QNetworkReply *reply)
{
    QNetworkReply::NetworkError error = reply->error();
    DEBUG() << error;

    /* Has been handled by onSslErrors already */
    if (error == QNetworkReply::SslHandshakeFailedError) {
        return true;
    }

    setError(Site::NetworkError, reply->errorString());
    return false;
}

void SitePrivate::onNetworkError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error);
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply != 0);
    handleNetworkError(reply);
}

void SitePrivate::onSslErrors(QList<QSslError> errors)
{
    DEBUG() << errors;
    // TODO
}

QNetworkReply *SitePrivate::uploadFile(const QString &filePath,
                                       const QList<QHttpPart> &extraParts)
{
    ensureHasNetworkAccessManager();

    QHttpMultiPart *multiPart =
        new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QFile *file = new QFile(filePath, multiPart);
    file->open(QIODevice::ReadOnly);

    QHttpPart upload;
    QByteArray contentDisposition =
        "form-data; name=\"file\"; filename =\"";
    QFileInfo info(filePath);
    contentDisposition += info.fileName().replace('"', '_').toUtf8();
    contentDisposition += '"';
    upload.setHeader(QNetworkRequest::ContentDispositionHeader,
                     contentDisposition);
    upload.setBodyDevice(file);

    multiPart->append(upload);

    foreach (const QHttpPart &part, extraParts) {
        multiPart->append(part);
    }

    QUrl uploadUrl(UPLOAD_URL);
    QNetworkRequest request(uploadUrl);
    request.setRawHeader("Authorization", "Token " + accessToken);
    QNetworkReply *reply = nam->post(request, multiPart);
    multiPart->setParent(reply);

    return reply;
}

Site::Site(QObject *parent):
    QObject(parent),
    d_ptr(new SitePrivate(this))
{
}

Site::~Site()
{
    delete d_ptr;
    d_ptr = 0;
}

void Site::setNetworkAccessManager(QNetworkAccessManager *nam)
{
    Q_D(Site);
    d->nam = nam;
}

QNetworkAccessManager *Site::networkAccessManager() const
{
    Q_D(const Site);
    return d->nam;
}

void Site::setLoginData(const QString &userName, const QString &password)
{
    Q_D(Site);
    d->userName = userName;
    d->password = password;
}

void Site::setAccessToken(const QByteArray &token)
{
    Q_D(Site);
    d->accessToken = token;
    Q_EMIT authenticationFinished();
}

bool Site::isAuthenticated() const
{
    Q_D(const Site);
    return !d->accessToken.isEmpty();
}

QNetworkReply *Site::uploadFile(const QString &filePath,
                                const QList<QHttpPart> &extraParts)
{
    Q_D(Site);
    return d->uploadFile(filePath, extraParts);
}

void Site::authenticate()
{
    Q_D(Site);
    d->authenticate();
}

Site::ErrorCode Site::lastError() const
{
    Q_D(const Site);
    return d->lastError;
}

QString Site::lastErrorMessage() const
{
    Q_D(const Site);
    return d->lastErrorMessage;
}

#include "site.moc"
