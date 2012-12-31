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
#include "file-log.h"
#include "file-monitor.h"
#include "upload-item.h"
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
    void onUploadPathChanged();
    void onDirectoryChanged();
    void onDataChanged(const QModelIndex &first, const QModelIndex &last);

private:
    QDateTime lastUpdateTime;
    FileMonitor watcher;
    FileLog fileLog;
    mutable Controller *q_ptr;
};

} // namespace

ControllerPrivate::ControllerPrivate(Controller *q):
    QObject(q),
    q_ptr(q)
{
    Configuration *configuration =
       Application::instance()->configuration();
    QObject::connect(configuration, SIGNAL(uploadPathChanged()),
                     this, SLOT(onUploadPathChanged()));

    QObject::connect(&watcher, SIGNAL(changed()),
                     this, SLOT(onDirectoryChanged()));
    watcher.setBasePath(configuration->uploadPath());

    fileLog.setBasePath(configuration->uploadPath());

    /* Monitor completed uploads (and update the FileLog) */
    UploadQueue *uploadQueue = Application::instance()->uploadQueue();
    connect(uploadQueue,
            SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
            this,
            SLOT(onDataChanged(const QModelIndex &, const QModelIndex &)));

    /* Force a refresh */
    onDirectoryChanged();
}

void ControllerPrivate::onUploadPathChanged()
{
    Configuration *configuration =
       Application::instance()->configuration();
    watcher.setBasePath(configuration->uploadPath());
    fileLog.setBasePath(configuration->uploadPath());

    /* The last upload time needs to be reset, since this is a new
     * directory and we'd better check every file in it. */
    lastUpdateTime = QDateTime();

    /* Force a refresh */
    onDirectoryChanged();
}

void ControllerPrivate::onDirectoryChanged()
{
    QDateTime newLastUpdateTime = QDateTime::currentDateTime();

    QStringList allFiles = watcher.filesChangedSince(lastUpdateTime);
    /* Check that the files actually need an upload; this is especially
     * needed when the object is first instantiated: since lastUpdateTime
     * is invalid we get the list of all monitored files, so we check
     * them against our upload log. */
    QStringList changedFiles = fileLog.filterOutLogged(allFiles);
    if (changedFiles.isEmpty()) return;

    UploadQueue *uploadQueue = Application::instance()->uploadQueue();

    foreach (const QString &fileName, changedFiles) {
        DEBUG() << "File:" << fileName;
        uploadQueue->requestUpload(fileName);
    }

    lastUpdateTime = newLastUpdateTime;
}

void ControllerPrivate::onDataChanged(const QModelIndex &first,
                                      const QModelIndex &last)
{
    UploadQueue *uploadQueue = Application::instance()->uploadQueue();

    for (QModelIndex i = first;
         i < last;
         i = i.sibling(i.row() + 1, i.column())) {
        QVariant data = uploadQueue->data(i, UploadQueue::UploadItemRole);
        UploadItem *item = data.value<UploadItem *>();
        if (Q_UNLIKELY(!item)) continue;

        if (item->progress() < 100) continue;

        DEBUG() << "Upload completed:" << item->fileName();
        fileLog.addFile(item->fileName());
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
