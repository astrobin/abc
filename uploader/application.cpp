/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#include "application.h"
#include "configuration.h"
#include "debug.h"
#include "updater.h"
#include "upload-queue.h"

#include <QNetworkAccessManager>

using namespace ABC;

namespace ABC {

class ApplicationPrivate
{
    Q_DECLARE_PUBLIC(Application)

    ApplicationPrivate(Application *q);

private:
    mutable QNetworkAccessManager nam;
    mutable Configuration *configuration;
    mutable UploadQueue *uploadQueue;
    mutable Updater *updater;
    mutable Application *q_ptr;
};

} // namespace

ApplicationPrivate::ApplicationPrivate(Application *q):
    configuration(0),
    uploadQueue(0),
    updater(0),
    q_ptr(q)
{
}

Application::Application(int &argc, char **argv):
    QApplication(argc, argv),
    d_ptr(new ApplicationPrivate(this))
{
    setQuitOnLastWindowClosed(false);
    setOrganizationName("astrobin.com");
    setApplicationName("Astrobin Uploader");
}

Application::~Application()
{
    delete d_ptr;
    d_ptr = 0;
}

Configuration *Application::configuration() const
{
    Q_D(const Application);
    if (d->configuration == 0) {
        d->configuration =
            new Configuration(const_cast<Application *>(this));
    }
    return d->configuration;
}

UploadQueue *Application::uploadQueue() const
{
    Q_D(const Application);
    if (d->uploadQueue == 0) {
        d->uploadQueue = new UploadQueue(const_cast<Application *>(this));
    }
    return d->uploadQueue;
}

Updater *Application::updater() const
{
    Q_D(const Application);
    if (d->updater == 0) {
        d->updater = new Updater(&d->nam, const_cast<Application *>(this));
    }
    return d->updater;
}

QNetworkAccessManager *Application::nam() const
{
    Q_D(const Application);
    return &d->nam;
}
