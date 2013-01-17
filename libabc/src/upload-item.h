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

#ifndef ABC_UPLOAD_ITEM_H
#define ABC_UPLOAD_ITEM_H

#include <QByteArray>
#include <QMetaType>
#include <QObject>
#include <QString>

namespace ABC {

class Site;

class UploadItemPrivate;
class UploadItem: public QObject
{
    Q_OBJECT

public:
    UploadItem(const QString &filePath,
               const QString &fileName,
               QObject *parent = 0);
    virtual ~UploadItem();

    void setBasePath(const QString &path);

    QString filePath() const;
    QString fileName() const;
    QByteArray fileHash() const;
    int progress() const;

    QString lastErrorMessage() const;
    bool errorIsRecoverable() const;

public Q_SLOTS:
    void startUpload(Site *site);

Q_SIGNALS:
    void progressChanged(int progress);

private:
    UploadItemPrivate *d_ptr;
    Q_DECLARE_PRIVATE(UploadItem)
};

}; // namespace

Q_DECLARE_METATYPE(ABC::UploadItem*)

#endif /* ABC_UPLOAD_ITEM_H */
