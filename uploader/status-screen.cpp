/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#include "application.h"
#include "config-screen.h"
#include "debug.h"
#include "status-screen.h"
#include "upload-queue.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

using namespace ABC;

static StatusScreen *sharedInstance = 0;

StatusScreen::StatusScreen(QWidget *parent):
    QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;

    progressLabel = new QLabel;
    layout->addWidget(progressLabel);

    QPushButton *configButton = new QPushButton(tr("Configure..."));
    QObject::connect(configButton, SIGNAL(clicked()),
                     this, SLOT(onConfigButtonClicked()));
    layout->addWidget(configButton);

    setLayout(layout);

    UploadQueue *uploadQueue = Application::instance()->uploadQueue();
    QObject::connect(uploadQueue,
                     SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),
                     this, SLOT(updateProgress()));
    QObject::connect(uploadQueue,
                     SIGNAL(rowsInserted(const QModelIndex &, int, int)),
                     this, SLOT(updateProgress()));
    QObject::connect(uploadQueue,
                     SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
                     this, SLOT(updateProgress()));
    updateProgress();
}

StatusScreen::~StatusScreen()
{
    sharedInstance = 0;
}

StatusScreen *StatusScreen::instance()
{
    if (sharedInstance == 0) {
        sharedInstance = new StatusScreen();
    }

    return sharedInstance;
}

void StatusScreen::updateProgress()
{
    UploadQueue *uploadQueue = Application::instance()->uploadQueue();
    int total = uploadQueue->rowCount();
    int completed = uploadQueue->completedUploads();

    if (completed >= total) {
        progressLabel->setText(tr("Up to date"));
    } else {
        progressLabel->setText(tr("Uploaded %1 out of %2 files").
                               arg(completed).arg(total));
    }
}

void StatusScreen::onConfigButtonClicked()
{
    ConfigScreen *configScreen = new ConfigScreen(this);
    configScreen->exec();
    delete configScreen;
}
