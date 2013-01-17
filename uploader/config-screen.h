/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#ifndef ABC_CONFIG_SCREEN_H
#define ABC_CONFIG_SCREEN_H

#include <QDialog>

class QLabel;
class QLineEdit;

namespace ABC {

class ConfigScreen: public QDialog
{
    Q_OBJECT

public:
    ConfigScreen(QWidget *parent = 0);
    virtual ~ConfigScreen();

    // reimplemented methods
    void accept();

private Q_SLOTS:
    void onChangePathClicked();

private:
    QLineEdit *wUserName;
    QLineEdit *wPassword;
    QLineEdit *wUploadPath;
};

}; // namespace

#endif /* ABC_CONFIG_SCREEN_H */
