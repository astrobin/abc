/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2013 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#include "debug.h"
#include "updater.h"

#include <QByteArray>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStringList>
#include <QUrl>
#include <QVariantMap>

#include <qjson/parser.h>

#define UPDATES_URL "http://cdn.astrobin.com/uploader/versions.json"

using namespace ABC;

namespace ABC {

class UpdaterPrivate: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(Updater)

    UpdaterPrivate(QNetworkAccessManager *nam, Updater *q);

    QString setupOsName();
    void checkForUpdates();
    bool parseReply();

public Q_SLOTS:
    void onCheckReply();

private:
    QString osName;
    QString currentVersion;
    QString latestVersion;
    QUrl fileUrl;
    QNetworkAccessManager *nam;
    QNetworkReply *checkReply;
    mutable Updater *q_ptr;
};

} // namespace

UpdaterPrivate::UpdaterPrivate(QNetworkAccessManager *nam, Updater *q):
    QObject(q),
    currentVersion(PROJECT_VERSION),
    nam(nam),
    checkReply(0),
    q_ptr(q)
{
    osName = setupOsName();
    DEBUG() << osName;
}

QString UpdaterPrivate::setupOsName()
{
#ifdef Q_OS_MAC
    return "osx";
#elif defined Q_OS_WIN32
    return "win32";
#elif defined Q_OS_LINUX
    QString name = "linux";
    QFile lsb("/etc/lsb-release");
    if (lsb.open(QIODevice::ReadOnly)) {
        QString all = QString::fromUtf8(lsb.readAll());
        foreach (const QString &line, all.split('\n')) {
            QStringList parts = line.split("=");
            if (parts[0].trimmed() == "DISTRIB_ID") {
                name = parts[1].trimmed().toLower();
                break;
            }
        }
    } else {
        qWarning() << "Cannot read distribution name";
    }
    return name;
#else
#warning "Unsupported platform"
    return "";
#endif
}

void UpdaterPrivate::checkForUpdates()
{
    if (checkReply != 0) return;

    QUrl checkUrl(UPDATES_URL);
    QNetworkRequest request(checkUrl);
    checkReply = nam->get(request);
    connect(checkReply, SIGNAL(finished()),
            this, SLOT(onCheckReply()));
}

bool UpdaterPrivate::parseReply()
{
    if (checkReply->error() != QNetworkReply::NoError) {
        qWarning() << "Network error:" << checkReply->error();
        return false;
    }

    uint statusCode =
        checkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();
    if (statusCode != 200) {
        qWarning() << "Status code:" << statusCode;
        return false;
    }
    QByteArray replyContent = checkReply->readAll();
    QJson::Parser parser;
    bool ok;
    QVariant tree = parser.parse(replyContent, &ok);
    if (!ok) return false;

    QVariantMap osData = tree.toMap().value(osName).toMap();
    if (!osData.contains("version")) return false;

    latestVersion = osData.value("version").toString();
    fileUrl = QUrl(osData.value("file").toString());

    return true;
}

void UpdaterPrivate::onCheckReply()
{
    Q_Q(Updater);

    parseReply();

    checkReply->deleteLater();
    checkReply = 0;

    Q_EMIT q->checkFinished();
}

Updater::Updater(QNetworkAccessManager *nam, QObject *parent):
    QObject(parent),
    d_ptr(new UpdaterPrivate(nam, this))
{
}

Updater::~Updater()
{
    delete d_ptr;
    d_ptr = 0;
}

void Updater::checkForUpdates()
{
    Q_D(Updater);
    d->checkForUpdates();
}

bool Updater::updateAvailable() const
{
    Q_D(const Updater);
    return d->latestVersion > d->currentVersion;
}

QString Updater::latestVersion() const
{
    Q_D(const Updater);
    return d->latestVersion;
}

QUrl Updater::fileUrl() const
{
    Q_D(const Updater);
    return d->fileUrl;
}

#include "updater.moc"
