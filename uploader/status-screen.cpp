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

#include <ABC/Site>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>

using namespace ABC;

static StatusScreen *sharedInstance = 0;

static QString errorMessageFromCode(Site::ErrorCode code)
{
    switch (code) {
    case Site::NetworkError:
        return QObject::tr("Network error");
    case Site::SslError:
        return QObject::tr("SSL error");
    case Site::AuthenticationError:
        return QObject::tr("Authentication error");
    case Site::WrongFileType:
        return QObject::tr("Unsupported file type");
    case Site::QuotaExceededError:
        return QObject::tr("Not enough storage space in your account");
    case Site::UnknownError:
    default:
        return QObject::tr("Unknown error");
    }
}

StatusScreen::StatusScreen(QWidget *parent):
    QDialog(parent)
{
    setWindowTitle(tr("Astrobin Uploader"));

    QVBoxLayout *layout = new QVBoxLayout;

    progressBar = new QProgressBar;
    progressBar->setVisible(false);
    layout->addWidget(progressBar);

    progressLabel = new QLabel;
    layout->addWidget(progressLabel);

    layout->addStretch();

    updateLabel = new QLabel;
    updateLabel->setTextFormat(Qt::RichText);
    updateLabel->setOpenExternalLinks(true);
    updateLabel->setVisible(false);
    layout->addWidget(updateLabel);
    layout->addStretch();

    QHBoxLayout *buttonBox = new QHBoxLayout;
    buttonBox->addStretch();

    QPushButton *configButton = new QPushButton(tr("Configure..."));
    QObject::connect(configButton, SIGNAL(clicked()),
                     this, SLOT(onConfigButtonClicked()));
    buttonBox->addWidget(configButton);

    QPushButton *hideButton = new QPushButton(tr("Hide"));
    QObject::connect(hideButton, SIGNAL(clicked()),
                     this, SLOT(reject()));
    buttonBox->addWidget(hideButton);
    buttonBox->addStretch();

    layout->addLayout(buttonBox);

    layout->addStretch();
    errorLabel = new QLabel;
    errorLabel->setAlignment(Qt::AlignHCenter);
    errorLabel->setVisible(false);
    errorLabel->setStyleSheet("QLabel { color : red; }");
    layout->addWidget(errorLabel);

    setLayout(layout);

    UploadQueue *uploadQueue = Application::instance()->uploadQueue();
    QObject::connect(uploadQueue,
                     SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),
                     this, SLOT(updateProgress()));
    QObject::connect(uploadQueue,
                     SIGNAL(statusChanged(UploadQueue::Status)),
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

void StatusScreen::setUpdate(const QString &version, const QUrl &fileUrl)
{
    updateLabel->setText(tr("A new version (%1) of Astrobin Uploader "
                            "is available! <a href=\"%2\">Get it here!</a>").
                         arg(version).arg(fileUrl.toString()));
    updateLabel->show();
}

void StatusScreen::updateProgress()
{
    UploadQueue *uploadQueue = Application::instance()->uploadQueue();
    int total = uploadQueue->rowCount();
    int completed;
    int inProgress;
    int failed;
    int retryLater;
    uploadQueue->itemsStatus(&completed, &inProgress, &failed, &retryLater);

    if (completed >= total) {
        progressLabel->setText(tr("Up to date"));
        progressBar->hide();
        errorLabel->hide();
    } else {
        progressLabel->setText(tr("Uploaded %1 out of %2 files").
                               arg(completed).arg(total));
        progressBar->setMaximum(total);
        progressBar->setMinimum(0);
        progressBar->setValue(completed);
        progressBar->show();

        /* For the time being, we don't distinguish between persistent failures
         * and retriable ones. */
        int errors = failed + retryLater;
        if (uploadQueue->status() == UploadQueue::Warning || errors > 0) {
            Site::ErrorCode errorCode = uploadQueue->lastUploadError();
            if (errorCode == Site::NoError) {
                errorCode = uploadQueue->site()->lastError();
            }
            QString errorMessage = errorMessageFromCode(errorCode);

            if (errors > 0) {
                errorLabel->setText(tr("%1 files failed to upload (%2).\n"
                                       "The upload will be attempted again.").
                                    arg(errors).arg(errorMessage));
            } else {
                /* This is probably an authentication error */
                errorLabel->setText(tr("Upload halted: %1").arg(errorMessage));
            }
            errorLabel->show();
        } else {
            errorLabel->hide();
        }
    }
}

void StatusScreen::onConfigButtonClicked()
{
    ConfigScreen *configScreen = new ConfigScreen(this);
    configScreen->exec();
    delete configScreen;
}
