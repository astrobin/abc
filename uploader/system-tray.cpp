/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#include "about-screen.h"
#include "debug.h"
#include "status-screen.h"
#include "system-tray.h"

#include <QAction>
#include <QApplication>
#include <QMenu>

using namespace ABC;

SystemTray::SystemTray(QObject *parent):
    QSystemTrayIcon(QIcon(":astrobin-systray"), parent)
{
    QObject::connect(this,
                     SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                     this,
                     SLOT(onActivated(QSystemTrayIcon::ActivationReason)));

    QAction *statusAction = new QAction(tr("Show status"), this);
    QObject::connect(statusAction, SIGNAL(triggered()),
                     this, SLOT(showStatus()));

    QAction *aboutAction = new QAction(tr("About..."), this);
    QObject::connect(aboutAction, SIGNAL(triggered()),
                     this, SLOT(showAbout()));

    QAction *quitAction = new QAction(tr("Quit"), this);
    QObject::connect(quitAction, SIGNAL(triggered()),
                     qApp, SLOT(quit()));

    menu = new QMenu;
    menu->addAction(statusAction);
    menu->addAction(aboutAction);
    menu->addAction(quitAction);
    setContextMenu(menu);
}

SystemTray::~SystemTray()
{
    delete menu;
}

void SystemTray::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    DEBUG() << "activated" << reason;

    if (reason == QSystemTrayIcon::DoubleClick ||
        reason == QSystemTrayIcon::Trigger) {
        showStatus();
    }
}

void SystemTray::showStatus()
{
    StatusScreen *statusScreen = StatusScreen::instance();
    statusScreen->show();
}

void SystemTray::showAbout()
{
    AboutScreen *aboutScreen = new AboutScreen;
    aboutScreen->exec();
    delete aboutScreen;
}
