/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#include "debug.h"
#include "file-monitor.h"

#include <QDateTime>
#include <QDir>
#include <QFileSystemWatcher>
#include <QTimer>

#define MIN_SIGNAL_INTERVAL 5 // seconds

using namespace ABC;

namespace ABC {

class FileMonitorPrivate: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(FileMonitor)

    FileMonitorPrivate(FileMonitor *q);

    void setBasePath(const QString &path);
    QStringList filesChangedSince(const QDateTime &since,
                                  const QString &path) const;

private Q_SLOTS:
    void onDirectoryChanged(const QString &path);
    void emitChanged();

private:
    void scheduleNotification();
    void recurseAddPath(const QString &path);

private:
    QFileSystemWatcher *watcher;
    QDateTime lastSignalTime;
    QTimer signalTimer;
    QString basePath;
    mutable FileMonitor *q_ptr;
};

} // namespace

FileMonitorPrivate::FileMonitorPrivate(FileMonitor *q):
    QObject(q),
    watcher(0),
    q_ptr(q)
{
    signalTimer.setSingleShot(true);
    signalTimer.setInterval(1000 * MIN_SIGNAL_INTERVAL);
    QObject::connect(&signalTimer, SIGNAL(timeout()),
                     this, SLOT(emitChanged()));
}

void FileMonitorPrivate::setBasePath(const QString &path)
{
    delete watcher;
    watcher = new QFileSystemWatcher(this);
    QObject::connect(watcher, SIGNAL(directoryChanged(const QString &)),
                     this, SLOT(onDirectoryChanged(const QString &)));
    basePath = path;

    /* Recursively find directories to be monitored */
    recurseAddPath(path);
}

void FileMonitorPrivate::onDirectoryChanged(const QString &path)
{
    /* Recursively find new directories to be monitored. */
    recurseAddPath(path);

    scheduleNotification();
}

void FileMonitorPrivate::emitChanged()
{
    Q_Q(FileMonitor);

    lastSignalTime = QDateTime::currentDateTimeUtc();
    Q_EMIT q->changed();
}

void FileMonitorPrivate::scheduleNotification()
{
    if (signalTimer.isActive()) return;

    /* Emit the signal, if it hasn't been emitted in the last few seconds */
    QDateTime now = QDateTime::currentDateTimeUtc();
    if (lastSignalTime.msecsTo(now) > signalTimer.interval()) {
        emitChanged();
    } else {
        /* Schedule the signal for later */
        signalTimer.start();
    }
}

void FileMonitorPrivate::recurseAddPath(const QString &path)
{
    if (path.isEmpty()) return;

    if (!watcher->directories().contains(path)) {
        DEBUG() << "watching" << path;
        watcher->addPath(path);
    }

    QDir dir(path);
    QStringList allDirs = dir.entryList(QDir::Dirs |
                                        QDir::NoDotAndDotDot);
    foreach (const QString &dirName, allDirs) {
        recurseAddPath(path + QDir::separator() + dirName);
    }
}

QStringList
FileMonitorPrivate::filesChangedSince(const QDateTime &since,
                                      const QString &path) const
{
    QStringList list;

    QDir dir(path);
    QFileInfoList allFiles = dir.entryInfoList(QDir::Files | QDir::Dirs |
                                               QDir::NoDotAndDotDot |
                                               QDir::Readable);
    foreach (const QFileInfo &info, allFiles) {
        if (info.isDir()) {
            list += filesChangedSince(since, info.absoluteFilePath());
        } else if (info.lastModified() > since) {
            list.append(info.absoluteFilePath());
        }
    }
    return list;
}

FileMonitor::FileMonitor(QObject *parent):
    QObject(parent),
    d_ptr(new FileMonitorPrivate(this))
{
}

FileMonitor::~FileMonitor()
{
    delete d_ptr;
    d_ptr = 0;
}

void FileMonitor::setBasePath(const QString &path)
{
    Q_D(FileMonitor);
    return d->setBasePath(path);
}

QStringList FileMonitor::filesChangedSince(const QDateTime &since) const
{
    Q_D(const FileMonitor);
    return d->filesChangedSince(since, d->basePath);
}

#include "file-monitor.moc"
