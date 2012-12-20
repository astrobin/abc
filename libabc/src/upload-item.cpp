/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of libabc.
 *
 * libabc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libabc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libabc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "debug.h"
#include "upload-item.h"

using namespace ABC;

namespace ABC {

class UploadItemPrivate
{
    Q_DECLARE_PUBLIC(UploadItem)

    UploadItemPrivate(const QUrl &fileName, UploadItem *q);

private:
    QUrl fileName;
    int progress;
    mutable UploadItem *q_ptr;
};

} // namespace

UploadItemPrivate::UploadItemPrivate(const QUrl &fileName, UploadItem *q):
    fileName(fileName),
    progress(0),
    q_ptr(q)
{
}

UploadItem::UploadItem(const QUrl &fileName, QObject *parent):
    QObject(parent),
    d_ptr(new UploadItemPrivate(fileName, this))
{
}

UploadItem::~UploadItem()
{
    delete d_ptr;
    d_ptr = 0;
}

QUrl UploadItem::fileName() const
{
    Q_D(const UploadItem);
    return d->fileName;
}

int UploadItem::progress() const
{
    Q_D(const UploadItem);
    return d->progress;
}

void UploadItem::startUpload()
{
    // TODO
    Q_EMIT progressChanged(100);
}
