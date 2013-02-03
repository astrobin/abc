/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#ifndef ABC_SYSTEM_TRAY_H
#define ABC_SYSTEM_TRAY_H

#include <QSystemTrayIcon>

#include "upload-queue.h"

class QMenu;

namespace ABC {

class SystemTray: public QSystemTrayIcon
{
    Q_OBJECT

public:
    SystemTray(QObject *parent = 0);
    virtual ~SystemTray();

private Q_SLOTS:
    void onUploadQueueStatusChanged(const UploadQueue::Status status);
    void onActivated(QSystemTrayIcon::ActivationReason reason);
    void showStatus();
    void showAbout();
    void onUpdateCheckFinished();

private:
    QMenu *menu;
    int previousStatus;
};

}; // namespace

#endif /* ABC_SYSTEM_TRAY_H */
