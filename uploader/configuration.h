/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#ifndef ABC_CONFIGURATION_H
#define ABC_CONFIGURATION_H

#include <QSettings>

class QDateTime;

namespace ABC {

class ConfigurationPrivate;
class Configuration: public QSettings
{
    Q_OBJECT

public:
    Configuration(QObject *parent = 0);
    virtual ~Configuration();

    void setUserName(const QString &userName);
    QString userName() const;

    void setPassword(const QString &password);
    QString password() const;

    void setUploadPath(const QString &path);
    QString uploadPath() const;

    void setLastUploadTime(const QDateTime &time);
    QDateTime lastUploadTime() const;

    QString logDbPath() const;

Q_SIGNALS:
    void uploadPathChanged();

private:
    ConfigurationPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Configuration)
};

}; // namespace

#endif /* ABC_CONFIGURATION_H */
