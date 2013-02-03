/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#ifndef ABC_APPLICATION_H
#define ABC_APPLICATION_H

#include <QApplication>

class QNetworkAccessManager;

namespace ABC {

class Configuration;
class Updater;
class UploadQueue;

class ApplicationPrivate;
class Application: public QApplication
{
    Q_OBJECT

public:
    Application(int &argc, char **argv);
    virtual ~Application();

    static Application *instance() {
        return static_cast<Application *>(qApp);
    }

    Configuration *configuration() const;
    UploadQueue *uploadQueue() const;
    QNetworkAccessManager *nam() const;
    Updater *updater() const;

private:
    ApplicationPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Application)
};

}; // namespace

#endif /* ABC_APPLICATION_H */
