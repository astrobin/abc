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

#ifndef ABC_SITE_H
#define ABC_SITE_H

#include <QByteArray>
#include <QList>
#include <QObject>
#include <QString>
#include <QVariantMap>

class QHttpPart;
class QNetworkAccessManager;
class QNetworkReply;

namespace ABC {

class SitePrivate;
class Site: public QObject
{
    Q_OBJECT

public:
    enum ErrorCode {
        NoError = 0,
        UnknownError,
        NetworkError,
        SslError,
        AuthenticationError,
        QuotaExceededError,
        WrongFileType,
    };

    Site(QObject *parent = 0);
    virtual ~Site();

    void setNetworkAccessManager(QNetworkAccessManager *nam);
    QNetworkAccessManager *networkAccessManager() const;

    void setLoginData(const QString &userName,
                      const QString &password);
    void setAccessToken(const QByteArray &token);

    bool isAuthenticated() const;

    ErrorCode lastError() const;
    QString lastErrorMessage() const;

    QNetworkReply *uploadFile(const QString &filePath,
                              const QList<QHttpPart> &extraParts);

    static QVariantMap parseJson(const QByteArray &data);

public Q_SLOTS:
    void authenticate();

Q_SIGNALS:
    void authenticationFinished();
    void error(Site::ErrorCode);

private:
    SitePrivate *d_ptr;
    Q_DECLARE_PRIVATE(Site)
};

}; // namespace

#endif /* ABC_SITE_H */
