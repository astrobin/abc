/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012-2013 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#ifndef ABC_UPLOADER_TEST_H
#define ABC_UPLOADER_TEST_H

#include <QString>
#include <QTest>

namespace ABC {

class UploaderTest: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void uploadQueue();
    void uploadQueueRetry();
    void fileMonitor();
    void fileLog();

private:
    QString createTmpDir();
};

}; // namespace

#endif /* ABC_UPLOADER_TEST_H */
