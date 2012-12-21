/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#ifndef ABC_FILE_MONITOR_H
#define ABC_FILE_MONITOR_H

#include <QObject>
#include <QStringList>

class QDateTime;

namespace ABC {

class FileMonitorPrivate;
class FileMonitor: public QObject
{
    Q_OBJECT

public:
    FileMonitor(QObject *parent = 0);
    virtual ~FileMonitor();

    void setBasePath(const QString &path);
    QStringList filesChangedSince(const QDateTime &since) const;

Q_SIGNALS:
    void changed();

private:
    FileMonitorPrivate *d_ptr;
    Q_DECLARE_PRIVATE(FileMonitor)
};

}; // namespace

#endif /* ABC_FILE_MONITOR_H */
