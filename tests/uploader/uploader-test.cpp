/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#include "uploader-test.h"

#include "file-monitor.h"

#include <QDebug>
#include <QDir>
#include <QSignalSpy>

#define UTF8(s) QString::fromUtf8(s)

using namespace ABC;

void UploaderTest::initTestCase()
{
}

void UploaderTest::cleanupTestCase()
{
}

void UploaderTest::fileMonitor()
{
    FileMonitor monitor;
    QSignalSpy changedSignal(&monitor, SIGNAL(changed()));
    QDateTime startTestTime = QDateTime::currentDateTime();

    QDir tmpDir = QDir::temp();
    QString dirName;
    bool created = false;
    int attempt = 0;
    do {
        dirName = QString("abc-fm-test%1").arg(attempt++);
        created = tmpDir.mkdir(dirName);
    } while (!created);
    QVERIFY(created);

    QVERIFY(tmpDir.cd(dirName));
    QString basePath = tmpDir.canonicalPath();
    monitor.setBasePath(basePath);

    QCOMPARE(changedSignal.count(), 0);
    QCOMPARE(monitor.filesChangedSince(startTestTime), QStringList());

    QTest::qWait(2000);

    /* Create some files and directories */
    QString dummyFile = tmpDir.filePath("dummy");
    QFile dummy(dummyFile);
    QVERIFY(dummy.open(QIODevice::WriteOnly));
    dummy.write("Some content");
    dummy.close();

    QFile::copy(dummyFile, tmpDir.filePath("another file.txt"));
    QFile::copy(dummyFile, tmpDir.filePath("image.jpg"));
    QDir subDir = tmpDir;
    subDir.mkdir("subdir");
    subDir.cd("subdir");
    QFile::copy(dummyFile, subDir.filePath("image2.jpg"));

    QTest::qWait(2000);
    QDateTime secondRoundTime = QDateTime::currentDateTime();

    QCOMPARE(changedSignal.count(), 1);

    QStringList expectedNewFiles;
    expectedNewFiles << dummyFile;
    expectedNewFiles << tmpDir.filePath("another file.txt");
    expectedNewFiles << tmpDir.filePath("image.jpg");
    expectedNewFiles << subDir.filePath("image2.jpg");
    expectedNewFiles.sort();

    QStringList newFiles = monitor.filesChangedSince(startTestTime);
    newFiles.sort();

    QCOMPARE(newFiles, expectedNewFiles);

    /* Create some new files, modify some existing ones */
    QTest::qWait(2000);
    QFile image(tmpDir.filePath("image.jpg"));
    QVERIFY(image.open(QIODevice::Append));
    image.write("extra content");
    image.close();

    QFile::copy(dummyFile, subDir.filePath("image3.jpg"));

    QTest::qWait(3000);
    QCOMPARE(changedSignal.count(), 2);

    expectedNewFiles.clear();
    expectedNewFiles << tmpDir.filePath("image.jpg");
    expectedNewFiles << subDir.filePath("image3.jpg");
    expectedNewFiles.sort();

    newFiles = monitor.filesChangedSince(secondRoundTime);
    newFiles.sort();

    QCOMPARE(newFiles, expectedNewFiles);
}

QTEST_MAIN(UploaderTest)
