/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2013 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#ifndef ABC_UPDATER_H
#define ABC_UPDATER_H

#include <QObject>

class QNetworkAccessManager;
class QUrl;

namespace ABC {

class UpdaterPrivate;
class Updater: public QObject
{
    Q_OBJECT

public:
    Updater(QNetworkAccessManager *nam, QObject *parent = 0);
    virtual ~Updater();

    void checkForUpdates();

    bool updateAvailable() const;
    QString latestVersion() const;
    QUrl fileUrl() const;

Q_SIGNALS:
    void checkFinished();

private:
    UpdaterPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Updater)
};

}; // namespace

#endif /* ABC_UPDATER_H */
