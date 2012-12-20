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

using namespace ABC;

static const QLatin1String keyUploadPath("UploadPath");
static const QLatin1String keyUserName("UserName");
static const QLatin1String keyPassword("Password");

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
    setValue(keyUserName, userName);
}

QString Configuration::userName() const
{
    return value(keyUserName).toString();
}

void Configuration::setPassword(const QString &password)
{
    setValue(keyPassword, password);
}

QString Configuration::password() const
{
    return value(keyPassword).toString();
}

void Configuration::setUploadPath(const QString &uploadPath)
{
    setValue(keyUploadPath, uploadPath);
}

QString Configuration::uploadPath() const
{
    return value(keyUploadPath).toString();
}
