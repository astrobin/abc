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
#include "upload-queue.h"

#include <ABC/Site>
#include <ABC/UploadItem>
#include <QDateTime>
#include <QFileInfo>
#include <QHash>
#include <QQueue>
#include <QSet>
#include <QTimer>

#define MAX_UPLOADS 2
#define SAFE_UPLOAD_DELAY   10 // seconds
#define INITIAL_RETRY_TIME  2 // seconds
#define MAX_RETRY_TIME      300 // seconds

using namespace ABC;

namespace ABC {

class UploadQueuePrivate: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(UploadQueue)

    UploadQueuePrivate(UploadQueue *q);

public Q_SLOTS:
    void authenticate();
    void onAuthenticationFinished();
    void runQueue();
    void retryFailed();
    void onProgressChanged(int progress);

private:
    /* "items" and "fileMap" must always be kept in sync */
    QList<UploadItem *> items;
    QHash<QString, UploadItem *> fileMap;
    QQueue<UploadItem *> queue;
    QSet<UploadItem *> activeUploads;
    QSet<UploadItem *> retryItems;
    QTimer runTimer;
    QTimer retryTimer;
    Site *site;
    mutable UploadQueue *q_ptr;
};

} // namespace

UploadQueuePrivate::UploadQueuePrivate(UploadQueue *q):
    QObject(q),
    site(new Site(this)),
    q_ptr(q)
{
    runTimer.setSingleShot(true);
    runTimer.setInterval(SAFE_UPLOAD_DELAY * 1000);
    QObject::connect(&runTimer, SIGNAL(timeout()),
                     this, SLOT(runQueue()));

    retryTimer.setSingleShot(true);
    retryTimer.setInterval(INITIAL_RETRY_TIME * 1000);
    QObject::connect(&retryTimer, SIGNAL(timeout()),
                     this, SLOT(retryFailed()));

    QObject::connect(site, SIGNAL(authenticationFinished()),
                     this, SLOT(onAuthenticationFinished()));
    authenticate();
}

void UploadQueuePrivate::authenticate()
{
    if (site->isAuthenticated()) {
        onAuthenticationFinished();
        return;
    }

    Configuration *configuration =
       Application::instance()->configuration();

    site->setLoginData(configuration->userName(),
                       configuration->password());
    site->authenticate();
}

void UploadQueuePrivate::onAuthenticationFinished()
{
    if (Q_UNLIKELY(!site->isAuthenticated())) {
        qWarning() << "Authentication failed";
        // TODO: expose the error to the UI
        return;
    }

    DEBUG() << "Authenticated; running queue";
    runQueue();
}

void UploadQueuePrivate::runQueue()
{
    Q_Q(UploadQueue);

    if (queue.isEmpty() || activeUploads.count() >= MAX_UPLOADS) return;

    QDateTime safeUploadTime =
        QDateTime::currentDateTimeUtc().addSecs(-SAFE_UPLOAD_DELAY);
    int rescheduled = 0;
    int numItems = queue.count();
    do {
        Q_EMIT q->statusChanged(UploadQueue::Uploading);
        UploadItem *item = queue.dequeue();
        /* Check that the file hasn't been modified in the last few seconds
         * (because otherwise it might not be complete) */
        QFileInfo info(item->filePath());
        if (info.lastModified() > safeUploadTime) {
            // push the item at the end of the queue
            queue.enqueue(item);
            rescheduled++;
        } else {
            activeUploads.insert(item);
            item->startUpload(site);
        }
    } while (activeUploads.count() < MAX_UPLOADS &&
             rescheduled < numItems &&
             !queue.isEmpty());

    Q_EMIT q->statusChanged(UploadQueue::Idle);
    /* If all items were rescheduled, it means that no files can be
     * safely uploaded at the moment; in that case, let's try again after
     * some time. */
    if (rescheduled >= numItems && !runTimer.isActive()) {
        runTimer.start();
    }
}

void UploadQueuePrivate::retryFailed()
{
    /* Put all failed items back into the queue, if the error is
     * recoverable */
    foreach (UploadItem *item, retryItems) {
        queue.enqueue(item);
    }

    /* Increase the retry interval; note that this doesn't start the
     * timer, it just sets it up for the next time. */
    int interval = retryTimer.interval() * 2;
    retryTimer.setInterval(interval < MAX_RETRY_TIME * 1000 ?
                           interval : MAX_RETRY_TIME * 1000);

    runQueue();
}

void UploadQueuePrivate::onProgressChanged(int progress)
{
    Q_Q(UploadQueue);

    if (progress > 0 && progress < 100) {
        /* At the moment, we are not interested in the download progress
         * of a single file. */
        return;
    }

    UploadItem *item = qobject_cast<UploadItem *>(sender());
    if (item == 0) return;

    int index = items.indexOf(item);
    if (index < 0) return;

    activeUploads.remove(item);

    if (item->progress() < 0) {
        Q_EMIT q->statusChanged(UploadQueue::Warning);
        if (item->errorIsRecoverable()) {
            retryItems.insert(item);
            if (!retryTimer.isActive())
                retryTimer.start();
        }
    } else {
        retryItems.remove(item);
        if (retryItems.isEmpty()) {
            retryTimer.setInterval(INITIAL_RETRY_TIME * 1000);
        }
    }

    QModelIndex modelIndex = q->index(index, 0);
    Q_EMIT q->dataChanged(modelIndex, modelIndex);

    runQueue();
}

UploadQueue::UploadQueue(QObject *parent):
    QAbstractListModel(parent),
    d_ptr(new UploadQueuePrivate(this))
{
}

UploadQueue::~UploadQueue()
{
    delete d_ptr;
    d_ptr = 0;
}

void UploadQueue::requestUpload(const QString &filePath,
                                const QString &fileName)
{
    Q_D(UploadQueue);

    if (d->fileMap.contains(filePath)) {
        /* TODO: if the upload is in progress, check the file's last
         * modification time and, if needed, re-start the download. */
        return;
    }

    UploadItem *item = new UploadItem(filePath, fileName, this);
    QObject::connect(item, SIGNAL(progressChanged(int)),
                     d, SLOT(onProgressChanged(int)));

    QModelIndex root;
    int index = rowCount(root);
    beginInsertRows(root, index, index);
    d->items.append(item);
    d->fileMap.insert(filePath, item);
    d->queue.enqueue(item);
    endInsertRows();

    d->authenticate();
}

int UploadQueue::completedUploads() const
{
    Q_D(const UploadQueue);

    int completed = 0;
    foreach (const UploadItem *item, d->items) {
        if (item->progress() >= 100) completed++;
    }

    return completed;
}

QVariant UploadQueue::data(const QModelIndex &index, int role) const
{
    Q_D(const UploadQueue);

    if (!index.isValid()) return QVariant();

    UploadItem *item = d->items[index.row()];

    switch (role) {
    case Qt::DisplayRole:
        // TODO: return the filename
        break;
    case ProgressRole:
        return item->progress();
    case UploadItemRole:
        return QVariant::fromValue(item);
    default:
        break;
    }

    return QVariant();
}

int UploadQueue::rowCount(const QModelIndex &parent) const
{
    Q_D(const UploadQueue);
    Q_UNUSED(parent);
    return d->items.count();
}

#include "upload-queue.moc"
