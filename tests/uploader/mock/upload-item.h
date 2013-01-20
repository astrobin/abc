/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2013 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#ifndef ABC_UPLOAD_ITEM_H
#define ABC_UPLOAD_ITEM_H

#include <QByteArray>
#include <QList>
#include <QMetaType>
#include <QObject>
#include <QString>
#include <QTimer>

namespace ABC {

class Site;

class UploadItem: public QObject
{
    Q_OBJECT

public:
    UploadItem(const QString &filePath,
               const QString &fileName,
               QObject *parent = 0):
        QObject(parent),
        m_filePath(filePath),
        m_fileName(fileName),
        m_progress(0)
    {
        m_replyTimer.setSingleShot(true);
        m_replyTimer.setInterval(10);
        connect(&m_replyTimer, SIGNAL(timeout()),
                this, SLOT(sendReply()));
        allItems.append(this);
    }
    virtual ~UploadItem() { allItems.removeAll(this); }

    void setBasePath(const QString &path) { Q_UNUSED(path); }

    QString filePath() const { return m_filePath; }
    QString fileName() const { return m_fileName; }
    QByteArray fileHash() const { return QByteArray(); }
    int progress() const { return m_progress; }

    QString lastErrorMessage() const { return m_errorMessage; }
    bool errorIsRecoverable() const { return m_errorIsRecoverable; }

    /* Methods useful for mocking */
    void failAfter(int msec, bool recoverable,
                   const QString &message = QString("Error")) {
        m_replyTimer.setInterval(msec);
        m_errorIsRecoverable = recoverable;
        m_errorMessage = message;
    }

    void succeedAfter(int msec) {
        m_replyTimer.setInterval(msec);
        m_errorMessage.clear();
    }

    static QList<UploadItem *> allItems;

private Q_SLOTS:
    void sendReply() {
        m_progress = m_errorMessage.isEmpty() ? 100 : -1;
        Q_EMIT progressChanged(m_progress);
    }

public Q_SLOTS:
    void startUpload(Site *site) {
        Q_UNUSED(site);
        m_replyTimer.start();
    }

Q_SIGNALS:
    void progressChanged(int progress);

private:
    QString m_filePath;
    QString m_fileName;
    QString m_errorMessage;
    bool m_errorIsRecoverable;
    int m_progress;
    QTimer m_replyTimer;
};

}; // namespace

Q_DECLARE_METATYPE(ABC::UploadItem*)

#endif /* ABC_UPLOAD_ITEM_H */
