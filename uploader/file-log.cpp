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
#include "file-log.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

using namespace ABC;

static const int dbVersion = 1;

namespace ABC {

class FileLogPrivate
{
    Q_DECLARE_PUBLIC(FileLog)

    FileLogPrivate(FileLog *q);
    ~FileLogPrivate() {}

    bool isLogged(const QString &filePath) const;
    QStringList filterLogged(const QStringList &allFiles) const;
    void addFile(const QString &filePath);

private:
    bool initDb();
    bool updateDb(int oldVersion);
    QString computeHash(const QString &filePath) const;

private:
    QSqlDatabase db;
    QDir baseDir;
    mutable FileLog *q_ptr;
};

} // namespace

FileLogPrivate::FileLogPrivate(FileLog *q):
    q_ptr(q)
{
    Configuration *conf = Application::instance()->configuration();
    QString dbPath = conf->logDbPath();

    QFileInfo info(dbPath);
    if (!info.exists()) {
        QDir dir(info.dir());
        dir.mkpath(dir.absolutePath());
    }

    db = QSqlDatabase::addDatabase("QSQLITE", "abc-uploader-conn");
    db.setDatabaseName(dbPath);

    if (!initDb()) {
        qWarning() << "Cannot open DB";
    }
}

bool FileLogPrivate::initDb()
{
    if (!db.open()) return false;

    // check the DB version
    QSqlQuery q = db.exec("PRAGMA user_version");
    int version = q.first() ? q.value(0).toInt() : 0;
    return version >= dbVersion ? true : updateDb(version);
}

bool FileLogPrivate::updateDb(int oldVersion)
{
    DEBUG() << "Database version:" << oldVersion;

    if (oldVersion < 1) {
        // create DB
        QString command =
            "CREATE TABLE Uploads ("
            "filePath TEXT UNIQUE NOT NULL,"
            "hash TEXT PRIMARY KEY NOT NULL ON CONFLICT REPLACE,"
            "modified TEXT"
            ")";
        db.exec(command);
    }

    if (db.lastError().isValid()) return false;

    // Update version number
    db.exec(QString("PRAGMA user_version = %1").arg(dbVersion));

    return true;
}

QString FileLogPrivate::computeHash(const QString &filePath) const
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    hash.addData(file.readAll());
    return QString(hash.result().toHex());
}

void FileLogPrivate::addFile(const QString &filePath)
{
    QString absolutePath = baseDir.absoluteFilePath(filePath);
    QString relativePath = baseDir.relativeFilePath(filePath);
    QFileInfo info(absolutePath);
    QSqlQuery q(db);
    q.prepare("INSERT OR REPLACE INTO Uploads (filePath, hash, modified) "
              "VALUES (:filePath, :hash, :modified)");
    q.bindValue(":filePath", relativePath);
    q.bindValue(":hash", computeHash(absolutePath));
    q.bindValue(":modified", info.lastModified().toString(Qt::ISODate));
    if (!q.exec()) {
        qWarning() << "Error executing query:" << q.lastError();
    }
}

bool FileLogPrivate::isLogged(const QString &filePath) const
{
    QString absolutePath = baseDir.absoluteFilePath(filePath);
    QString relativePath = baseDir.relativeFilePath(filePath);

    QSqlQuery q(db);
    q.prepare("SELECT hash, modified FROM Uploads "
              "WHERE filePath = :filePath");
    q.bindValue(":filePath", relativePath);
    if (!q.exec()) {
        qWarning() << "Error executing query:" << q.lastError();
        return false;
    }

    if (!q.next()) return false;

    /* If the last modification time is the same as the logged file, we trust
     * that it's the same file and we don't check the hash */
    QFileInfo info(absolutePath);
    QString lastModified = info.lastModified().toString(Qt::ISODate);
    if (q.value(1).toString() == lastModified) return true;

    /* If the times differ, it might still be the same file; let's check the
     * hash */
    if (q.value(0).toString() == computeHash(absolutePath)) return true;

    return false;
}

QStringList FileLogPrivate::filterLogged(const QStringList &allFiles) const
{
    QStringList loggedFiles;

    foreach (const QString &file, allFiles) {
        if (isLogged(file)) loggedFiles.append(file);
    }
    return loggedFiles;
}

FileLog::FileLog(QObject *parent):
    QObject(parent),
    d_ptr(new FileLogPrivate(this))
{
}

FileLog::~FileLog()
{
    delete d_ptr;
    d_ptr = 0;
}

void FileLog::clear()
{
    Q_D(FileLog);
    QSqlQuery q(d->db);
    if (!q.exec("DELETE FROM Uploads")) {
        qWarning() << "Error executing query:" << q.lastError();
    }
}

void FileLog::setBasePath(const QString &path)
{
    Q_D(FileLog);
    d->baseDir.setPath(path);
}

void FileLog::addFile(const QString &filePath)
{
    Q_D(FileLog);
    return d->addFile(filePath);
}

bool FileLog::isLogged(const QString &filePath) const
{
    Q_D(const FileLog);
    return d->isLogged(filePath);
}

QStringList FileLog::filterLogged(const QStringList &allFiles) const
{
    Q_D(const FileLog);
    return d->filterLogged(allFiles);
}
