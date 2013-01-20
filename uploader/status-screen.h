/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#ifndef ABC_STATUS_SCREEN_H
#define ABC_STATUS_SCREEN_H

#include <QDialog>

class QLabel;

namespace ABC {

class StatusScreen: public QDialog
{
    Q_OBJECT

public:
    static StatusScreen *instance();
    virtual ~StatusScreen();

private:
    StatusScreen(QWidget *parent = 0);

private Q_SLOTS:
    void updateProgress();
    void onConfigButtonClicked();

private:
    QLabel *progressLabel;
};

}; // namespace

#endif /* ABC_STATUS_SCREEN_H */