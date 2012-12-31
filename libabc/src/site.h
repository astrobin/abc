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

#include <QObject>
#include <QString>

class QNetworkAccessManager;

namespace ABC {

class SitePrivate;
class Site: public QObject
{
    Q_OBJECT

public:
    enum ErrorCode {
        UnknownError = 0,
        NetworkError,
        SslError,
        AuthenticationError,
        QuotaExceededError,
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
