/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#ifndef ABC_ABOUT_SCREEN_H
#define ABC_ABOUT_SCREEN_H

#include <QDialog>

namespace ABC {

class AboutScreen: public QDialog
{
    Q_OBJECT

public:
    AboutScreen(QWidget *parent = 0);
    virtual ~AboutScreen();
};

}; // namespace

#endif /* ABC_ABOUT_SCREEN_H */
