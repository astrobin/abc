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
#include "controller.h"
#include "debug.h"
#include "file-monitor.h"
#include "upload-queue.h"

#include <QDateTime>
#include <QUrl>

using namespace ABC;

namespace ABC {

class ControllerPrivate: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(Controller)

    ControllerPrivate(Controller *q);

private Q_SLOTS:
    void updateWatcher();
    void onDirectoryChanged();

private:
    QDateTime lastUpdateTime;
    FileMonitor watcher;
    mutable Controller *q_ptr;
};

} // namespace

ControllerPrivate::ControllerPrivate(Controller *q):
    QObject(q),
    q_ptr(q)
{
    Configuration *configuration =
       Application::instance()->configuration();
    lastUpdateTime = configuration->lastUploadTime();
    QObject::connect(configuration, SIGNAL(uploadPathChanged()),
                     this, SLOT(updateWatcher()));

    QObject::connect(&watcher, SIGNAL(changed()),
                     this, SLOT(onDirectoryChanged()));
    watcher.setBasePath(configuration->uploadPath());

    /* Force a refresh */
    onDirectoryChanged();
}

void ControllerPrivate::updateWatcher()
{
    Configuration *configuration =
       Application::instance()->configuration();
    watcher.setBasePath(configuration->uploadPath());

    /* The last upload time needs to be reset, since this is a new
     * directory and we'd better check every file in it. */
    lastUpdateTime = QDateTime();

    /* Force a refresh */
    onDirectoryChanged();
}

void ControllerPrivate::onDirectoryChanged()
{
    QDateTime oldLastUpdateTime = lastUpdateTime;
    lastUpdateTime = QDateTime::currentDateTime();

    UploadQueue *uploadQueue = Application::instance()->uploadQueue();

    DEBUG() << "Scanning upload directory";
    QStringList allFiles = watcher.filesChangedSince(oldLastUpdateTime);
    foreach (const QString &fileName, allFiles) {
        DEBUG() << "File:" << fileName;
        uploadQueue->requestUpload(fileName);
    }
}

Controller::Controller(QObject *parent):
    QObject(parent),
    d_ptr(new ControllerPrivate(this))
{
}

Controller::~Controller()
{
    delete d_ptr;
    d_ptr = 0;
}

#include "controller.moc"
