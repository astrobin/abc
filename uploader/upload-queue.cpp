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

#include <ABC/UploadItem>

using namespace ABC;

namespace ABC {

class UploadQueuePrivate: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(UploadQueue)

    UploadQueuePrivate(UploadQueue *q);

public Q_SLOTS:
    void onProgressChanged(int progress);

private:
    QList<UploadItem *> items;
    mutable UploadQueue *q_ptr;
};

} // namespace

UploadQueuePrivate::UploadQueuePrivate(UploadQueue *q):
    QObject(q),
    q_ptr(q)
{
}

void UploadQueuePrivate::onProgressChanged(int progress)
{
    Q_Q(UploadQueue);

    if (progress < 0 || progress >= 100) {
        Q_EMIT q->uploadCompleted();
    }
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

void UploadQueue::requestUpload(const QUrl &fileName)
{
    Q_D(UploadQueue);
    UploadItem *item = new UploadItem(fileName, this);
    QObject::connect(item, SIGNAL(progressChanged(int)),
                     d, SLOT(onProgressChanged(int)));

    QModelIndex root;
    int index = rowCount(root);
    beginInsertRows(root, index, index);
    d->items.append(item);
    endInsertRows();

    // TODO: start download
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
