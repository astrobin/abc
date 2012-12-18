/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#include "debug.h"
#include "status-screen.h"

using namespace ABC;

static StatusScreen *sharedInstance = 0;

StatusScreen::StatusScreen(QWidget *parent):
    QDialog(parent)
{
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
