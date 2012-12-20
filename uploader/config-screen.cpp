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
#include "configuration.h"
#include "debug.h"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

using namespace ABC;

ConfigScreen::ConfigScreen(QWidget *parent):
    QDialog(parent)
{
    Configuration *configuration =
        Application::instance()->configuration();

    // Login information
    QFormLayout *loginForm = new QFormLayout;

    wUserName = new QLineEdit;
    loginForm->addRow(tr("User name:"), wUserName);
    wUserName->setText(configuration->userName());

    wPassword = new QLineEdit;
    wPassword->setEchoMode(QLineEdit::Password);
    loginForm->addRow(tr("Password:"), wPassword);
    wUserName->setText(configuration->password());

    QGroupBox *loginGroup = new QGroupBox(tr("Site authentication"));
    loginGroup->setLayout(loginForm);

    // Application configuration
    QFormLayout *uploadForm = new QFormLayout;

    wUploadPath = new QLineEdit;
    wUploadPath->setText(configuration->uploadPath());

    QPushButton *changePath = new QPushButton(tr("Choose..."));
    QObject::connect(changePath, SIGNAL(clicked()),
                     this, SLOT(onChangePathClicked()));

    QHBoxLayout *pathBox = new QHBoxLayout;
    pathBox->addWidget(wUploadPath);
    pathBox->addWidget(changePath);
    uploadForm->addRow(tr("Upload files from:"), pathBox);

    QGroupBox *uploadGroup = new QGroupBox(tr("Upload configuration"));
    uploadGroup->setLayout(uploadForm);

    // default buttons
    QDialogButtonBox *buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok |
                             QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(loginGroup);
    layout->addWidget(uploadGroup);
    layout->addWidget(buttonBox);
    setLayout(layout);
}

ConfigScreen::~ConfigScreen()
{
}

void ConfigScreen::accept()
{
    // Store the changes
    Configuration *configuration =
        Application::instance()->configuration();

    configuration->setUserName(wUserName->text());
    configuration->setPassword(wPassword->text());
    configuration->setUploadPath(wUploadPath->text());

    QDialog::accept();
}

void ConfigScreen::onChangePathClicked()
{
    QString uploadPath =
        QFileDialog::getExistingDirectory(this,
                                          tr("Choose an upload folder"),
                                          wUploadPath->text());
    if (!uploadPath.isEmpty()) {
        wUploadPath->setText(uploadPath);
    }
}
