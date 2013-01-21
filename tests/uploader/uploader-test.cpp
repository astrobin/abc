/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012-2013 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of ABC (AstroBin Companion).
 *
 * All rights reserved.
 */

#include "uploader-test.h"

#include "application.h"
#include "configuration.h"
#include "file-log.h"
#include "file-monitor.h"
#include "mock/site.h"
#include "mock/upload-item.h"
#include "upload-queue.h"

#include <QDebug>
#include <QDir>
#include <QSignalSpy>

#define UTF8(s) QString::fromUtf8(s)

using namespace ABC;

/* Handles to mocked objects */
Site *Site::instance = 0;
QList<UploadItem *> UploadItem::allItems;

void UploaderTest::initTestCase()
{
    QApplication::setApplicationName("abc-uploader-test");
}

void UploaderTest::cleanupTestCase()
{
}

QString UploaderTest::createTmpDir()
{
    QDir tmpDir = QDir::temp();
    QString dirName;
    bool created = false;
    int attempt = 0;
    do {
        dirName = QString("abc-fm-test%1").arg(attempt++);
        created = tmpDir.mkdir(dirName);
    } while (!created);

    tmpDir.cd(dirName);
    return tmpDir.canonicalPath();
}

void UploaderTest::fileMonitor()
{
    FileMonitor monitor;
    QSignalSpy changedSignal(&monitor, SIGNAL(changed()));
    QDateTime startTestTime = QDateTime::currentDateTime();

    QString basePath = createTmpDir();
    QDir tmpDir(basePath);
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

void UploaderTest::fileLog()
{
    // Remove existing DB
    QFile::remove(Application::instance()->configuration()->logDbPath());
    FileLog log;

    QString basePath = createTmpDir();
    log.setBasePath(basePath);

    QCOMPARE(log.isLogged("dummy"), false);

    QDir tmpDir(basePath);

    /* Create some files and directories */
    QString dummyFile = tmpDir.filePath("dummy");
    QFile dummy(dummyFile);
    QVERIFY(dummy.open(QIODevice::WriteOnly));
    dummy.write("Some content");
    dummy.close();

    dummy.setFileName(tmpDir.filePath("another file"));
    QVERIFY(dummy.open(QIODevice::WriteOnly));
    dummy.write("Totally different file");
    dummy.close();

    QDir subDir = tmpDir;
    subDir.mkdir("subdir");
    subDir.cd("subdir");
    dummy.setFileName(subDir.filePath("image2.jpg"));
    QVERIFY(dummy.open(QIODevice::WriteOnly));
    dummy.write("Image file");
    dummy.close();

    QCOMPARE(log.isLogged("dummy"), false);
    QCOMPARE(log.isLogged("subdir/image2.jpg"), false);

    /* Add files to the log */
    log.addFile(tmpDir.absoluteFilePath("dummy"));
    log.addFile("subdir/image2.jpg");
    QCOMPARE(log.isLogged("dummy"), true);
    QCOMPARE(log.isLogged("another file"), false);
    QCOMPARE(log.isLogged(subDir.absoluteFilePath("image2.jpg")), true);

    /* Change the date of a file; the file should still be logged */
    QTest::qWait(2000);
    QFile::copy(dummyFile, tmpDir.filePath("dummy2"));
    QFile::remove(dummyFile);
    QFile::rename(tmpDir.filePath("dummy2"), dummyFile);
    QCOMPARE(log.isLogged("dummy"), true);

    /* Change the contents of a file; this should mark the file as not
     * being logged anymore */
    dummy.setFileName(dummyFile);
    QVERIFY(dummy.open(QIODevice::Append));
    dummy.write("Some more text");
    dummy.close();

    QCOMPARE(log.isLogged("dummy"), false);

    log.addFile("dummy");
    QCOMPARE(log.isLogged("dummy"), true);
}

void UploaderTest::uploadQueue()
{
    UploadQueue queue;

    QCOMPARE(queue.rowCount(), 0);
    QVERIFY(UploadItem::allItems.isEmpty());

    /* request an upload */
    queue.requestUpload("a file", "filename");
    QCOMPARE(queue.rowCount(), 1);
    QCOMPARE(UploadItem::allItems.count(), 1);

    /* request another one */
    queue.requestUpload("another file", "filename");
    QCOMPARE(queue.rowCount(), 2);
    QCOMPARE(UploadItem::allItems.count(), 2);

    /* request the first item again; the count should not increase */
    queue.requestUpload("a file", "filename");
    QCOMPARE(queue.rowCount(), 2);
    QCOMPARE(UploadItem::allItems.count(), 2);

    int completed = 0;
    queue.itemsStatus(&completed);
    QCOMPARE(completed, 0);

    /* give some time for the uploads to complete; the default time for
     * the mocked UploadItem and for the site authentication is 10 msecs
     * each */
    QTest::qWait(25);
    queue.itemsStatus(&completed);
    QCOMPARE(completed, 2);
}

void UploaderTest::uploadQueueRetry()
{
    UploadQueue queue;

    /* shorten the authentication time */
    QVERIFY(Site::instance != 0);
    Site::instance->authenticateAfter(0);

    QCOMPARE(queue.rowCount(), 0);
    QVERIFY(UploadItem::allItems.isEmpty());

    /* request a few uploads, some of which will fail */
    queue.requestUpload("file1", "file1");
    queue.requestUpload("file2", "file2");
    UploadItem *file2 = UploadItem::allItems.last();
    file2->failAfter(5, true);
    queue.requestUpload("file3", "file3");
    UploadItem::allItems.last()->failAfter(20, false);
    queue.requestUpload("file4", "file4");
    UploadItem *file4 = UploadItem::allItems.last();
    file4->failAfter(30, true);

    QCOMPARE(queue.rowCount(), 4);
    int completed = 0;
    int inProgress = 0;
    int failed = 0;
    int retryLater = 0;
    queue.itemsStatus(&completed, &inProgress, &failed, &retryLater);
    QCOMPARE(completed, 0);

    /* periodically check the upload status */
    QTest::qWait(15);
    queue.itemsStatus(&completed, &inProgress, &failed, &retryLater);
    QCOMPARE(completed, 1);
    QCOMPARE(inProgress, 2);
    QCOMPARE(failed, 0);
    QCOMPARE(retryLater, 1);

    QTest::qWait(15);
    queue.itemsStatus(&completed, &inProgress, &failed, &retryLater);
    QCOMPARE(completed, 1);
    QCOMPARE(inProgress, 1);
    QCOMPARE(failed, 1);
    QCOMPARE(retryLater, 1);

    QTest::qWait(20);
    queue.itemsStatus(&completed, &inProgress, &failed, &retryLater);
    QCOMPARE(completed, 1);
    QCOMPARE(inProgress, 0);
    QCOMPARE(failed, 1);
    QCOMPARE(retryLater, 2);

    /* Change the recoverable so that next time they'll upload
     * successfully */
    file2->succeedAfter(10);
    file4->succeedAfter(20);

    /* After one second, the situation should still be the same */
    QTest::qWait(1000);
    queue.itemsStatus(&completed, &inProgress, &failed, &retryLater);
    QCOMPARE(completed, 1);
    QCOMPARE(inProgress, 0);
    QCOMPARE(failed, 1);
    QCOMPARE(retryLater, 2);

    /* After two seconds, UploadQueue should retry the recoverable failed
     * items; let's add some time to actually let the uploads complete */
    QTest::qWait(1100);
    queue.itemsStatus(&completed, &inProgress, &failed, &retryLater);
    QCOMPARE(completed, 3);
    QCOMPARE(inProgress, 0);
    QCOMPARE(failed, 1);
    QCOMPARE(retryLater, 0);
}

int main(int argc, char **argv)
{
    Application app(argc, argv);

    UploaderTest test1;
    return QTest::qExec(&test1);
}
