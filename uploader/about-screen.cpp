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

#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>

using namespace ABC;

AboutScreen::AboutScreen(QWidget *parent):
    QDialog(parent)
{
    setWindowTitle(tr("Astrobin Uploader"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(64, 11, 64, 11);

    QLabel *logo = new QLabel;
    logo->setPixmap(QPixmap(":logo"));
    logo->setAlignment(Qt::AlignHCenter);
    layout->addWidget(logo);
    layout->addStretch();

    QLabel *name = new QLabel;
    name->setText(tr("Astrobin Uploader"));
    name->setAlignment(Qt::AlignHCenter);
    name->setStyleSheet("QLabel { font: bold 16px; }");
    layout->addWidget(name);

    QLabel *version = new QLabel;
    version->setText(tr("Version %1").arg(PROJECT_VERSION));
    version->setAlignment(Qt::AlignHCenter);
    version->setStyleSheet("QLabel { font: 12px; }");
    layout->addWidget(version);
    layout->addStretch();

    setLayout(layout);
}

AboutScreen::~AboutScreen()
{
}
