/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2013 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#ifndef ABC_SITE_H
#define ABC_SITE_H

#include <QList>
#include <QObject>
#include <QString>
#include <QTimer>

class QHttpPart;
class QNetworkAccessManager;
class QNetworkReply;

namespace ABC {

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

    Site(QObject *parent = 0):
        QObject(parent),
        m_lastError(UnknownError),
        m_isAuthenticated(false),
        m_isAuthenticating(false)
    {
        m_authTimer.setSingleShot(true);
        m_authTimer.setInterval(10);
        connect(&m_authTimer, SIGNAL(timeout()),
                this, SLOT(finishAuthentication()));

        instance = this;
    }

    virtual ~Site() { instance = 0; }

    static Site *instance;

    void setNetworkAccessManager(QNetworkAccessManager *nam) { Q_UNUSED(nam); }
    QNetworkAccessManager *networkAccessManager() const { return 0; }

    void setLoginData(const QString &userName,
                      const QString &password) {
        Q_UNUSED(userName);
        Q_UNUSED(password);
    }
    void setAccessToken(const QByteArray &token) {
        Q_UNUSED(token);
    }

    bool isAuthenticated() const { return m_isAuthenticated; }

    ErrorCode lastError() const { return m_lastError; }
    QString lastErrorMessage() const { return m_lastErrorMessage; }

    QNetworkReply *uploadFile(const QString &filePath,
                              const QList<QHttpPart> &extraParts) {
        Q_UNUSED(filePath);
        Q_UNUSED(extraParts);
        return 0;
    }

private Q_SLOTS:
    void finishAuthentication() {
        m_isAuthenticated = true;
        m_isAuthenticating = false;
        Q_EMIT authenticationFinished();
    }

public Q_SLOTS:
    void authenticate() {
        if (m_isAuthenticating || m_isAuthenticated) return;
        m_isAuthenticating = true;
        m_authTimer.start();
    }

Q_SIGNALS:
    void authenticationFinished();
    void error(Site::ErrorCode);

private:
    QTimer m_authTimer;
    ErrorCode m_lastError;
    QString m_lastErrorMessage;
    bool m_isAuthenticated;
    bool m_isAuthenticating;
};

}; // namespace

#endif /* ABC_SITE_H */
