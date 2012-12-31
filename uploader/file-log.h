/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#ifndef ABC_FILE_LOG_H
#define ABC_FILE_LOG_H

#include <QObject>
#include <QStringList>

class QDateTime;

namespace ABC {

class FileLogPrivate;
class FileLog: public QObject
{
    Q_OBJECT

public:
    FileLog(QObject *parent = 0);
    virtual ~FileLog();

    void clear();
    void setBasePath(const QString &path);

    bool isLogged(const QString &filePath) const;
    QStringList filterOutLogged(const QStringList &allFiles) const;

public Q_SLOTS:
    void addFile(const QString &filePath);

private:
    FileLogPrivate *d_ptr;
    Q_DECLARE_PRIVATE(FileLog)
};

}; // namespace

#endif /* ABC_FILE_LOG_H */
