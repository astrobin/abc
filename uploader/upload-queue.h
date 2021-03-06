/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#ifndef ABC_UPLOAD_QUEUE_H
#define ABC_UPLOAD_QUEUE_H

#include <ABC/Site>
#include <QAbstractListModel>

namespace ABC {

class UploadQueuePrivate;
class UploadQueue: public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        ProgressRole = Qt::UserRole,
        UploadItemRole,
    };

    enum Status {
        Idle = 0,
        Uploading,
        Warning,
    };

    UploadQueue(QObject *parent = 0);
    virtual ~UploadQueue();

    Site *site() const;

    void requestUpload(const QString &filePath, const QString &fileName);

    Status status() const;
    Site::ErrorCode lastUploadError() const;
    void itemsStatus(int *succeeded, int *inProgress = 0,
                     int *failed = 0, int *retryLater = 0) const;

    // reimplemented virtual methods:
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

Q_SIGNALS:
    void statusChanged(UploadQueue::Status status);

private:
    UploadQueuePrivate *d_ptr;
    Q_DECLARE_PRIVATE(UploadQueue)
};

}; // namespace

#endif /* ABC_UPLOAD_QUEUE_H */
