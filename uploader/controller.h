/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#ifndef ABC_CONTROLLER_H
#define ABC_CONTROLLER_H

#include <QObject>

namespace ABC {

class Configuration;
class UploadQueue;

class ControllerPrivate;
class Controller: public QObject
{
    Q_OBJECT

public:
    Controller(QObject *parent = 0);
    virtual ~Controller();

private:
    ControllerPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Controller)
};

}; // namespace

#endif /* ABC_CONTROLLER_H */
