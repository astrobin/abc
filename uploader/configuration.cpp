/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#include "configuration.h"
#include "debug.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QDir>

using namespace ABC;

static const QLatin1String keyAutoStart("AutoStart");
static const QLatin1String keyLastUploadTime("LastUploadTime");
static const QLatin1String keyUploadPath("UploadPath");
static const QLatin1String keyUserName("UserName");
static const QLatin1String keyPassword("Password");
static const QLatin1String keyLogDbPath("LogDbPath");

namespace ABC {

class ConfigurationPrivate
{
    Q_DECLARE_PUBLIC(Configuration)

    ConfigurationPrivate(Configuration *q);

private:
    mutable Configuration *q_ptr;
};

} // namespace

ConfigurationPrivate::ConfigurationPrivate(Configuration *q):
    q_ptr(q)
{
}

Configuration::Configuration(QObject *parent):
    QSettings(parent),
    d_ptr(new ConfigurationPrivate(this))
{
}

Configuration::~Configuration()
{
    delete d_ptr;
    d_ptr = 0;
}

void Configuration::setUserName(const QString &userName)
{
    if (userName == this->userName()) return;

    setValue(keyUserName, userName);
    Q_EMIT userNameChanged();
}

QString Configuration::userName() const
{
    return value(keyUserName).toString();
}

void Configuration::setPassword(const QString &password)
{
    if (password == this->password()) return;

    setValue(keyPassword, password);
    Q_EMIT passwordChanged();
}

QString Configuration::password() const
{
    return value(keyPassword).toString();
}

void Configuration::setUploadPath(const QString &uploadPath)
{
    QString oldValue = this->uploadPath();
    if (uploadPath == oldValue) return;

    setValue(keyUploadPath, uploadPath);
    Q_EMIT uploadPathChanged();
}

QString Configuration::uploadPath() const
{
    return value(keyUploadPath).toString();
}

void Configuration::setLastUploadTime(const QDateTime &time)
{
    setValue(keyLastUploadTime, time);
}

QDateTime Configuration::lastUploadTime() const
{
    return value(keyLastUploadTime).toDateTime();
}

void Configuration::setAutoStart(bool autoStart)
{
    if (autoStart == this->autoStart()) return;

    setValue(keyAutoStart, autoStart);
    Q_EMIT autoStartChanged(autoStart);
}

bool Configuration::autoStart() const
{
    return value(keyAutoStart, true).toBool();
}

QString Configuration::logDbPath() const
{
    QString path = value(keyLogDbPath).toString();
    if (path.isEmpty()) {
        QString directory =
            QDesktopServices::storageLocation(QDesktopServices::DataLocation);
        path = directory + QDir::separator() + "abc-uploader.db";
    }

    return path;
}
