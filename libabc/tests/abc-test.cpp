/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * Copyright (C) 2012 Alberto Mardegan <info@mardy.it>
 *
 * This file is part of libabc.
 *
 * libabc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libabc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libabc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "abc-test.h"

#include "configuration.h"
#include "image-set.h"
#include "image.h"

#include <QDebug>
#include <QRect>

#define UTF8(s) QString::fromUtf8(s)

using namespace ABC;

void AbcTest::initTestCase()
{
}

void AbcTest::cleanupTestCase()
{
}

void AbcTest::loadFits()
{
    Image image;
    QVERIFY(!image.isValid());
    QCOMPARE(image.type(), UnknownType);

    bool ok = image.load("UIT.fits");
    QVERIFY(ok);
    QVERIFY(image.isValid());
    QCOMPARE(image.size(), QSize(512, 512));
}

void AbcTest::imageSetAverage()
{
    Image source;

    bool ok = source.load("UIT.fits");
    QVERIFY(ok);

    ImageSet images;
    QVERIFY(images.addImage(source));
    QVERIFY(images.addImage(source));
    QVERIFY(images.addImage(source));

    Image average = images.average();
    QCOMPARE(average.size(), source.size());

    long numPixels = average.size().width() * average.size().height();
    const PixelValue *sourcePixels = source.pixels();
    const PixelValue *averagePixels = average.pixels();
    for (long i = 0; i < numPixels; i++) {
        QVERIFY(qFuzzyCompare(averagePixels[i], sourcePixels[i]));
    }
}

void AbcTest::imageSetBounds()
{
    Image source;

    bool ok = source.load("UIT.fits");
    QVERIFY(ok);
    QCOMPARE(source.exposure(), 83.2f);

    ImageSet images;
    QVERIFY(images.isEmpty());
    QVERIFY(images.addImage(source, QTransform()));
    QVERIFY(!images.isEmpty());

    QRect imageRect = QRect(QPoint(0, 0), source.size());
    QRect expectedBounds = imageRect;
    QCOMPARE(images.boundingRect(), expectedBounds);

    /* We cannot add an image without an associated transformation, after
     * adding a transformed one. */
    QVERIFY(!images.addImage(source));

    // Add a translated image
    QVERIFY(images.addImage(source, QTransform().translate(10, 0)));
    expectedBounds.adjust(0, 0, 10, 0);
    QCOMPARE(images.boundingRect(), expectedBounds);

    // Add a rotated image
    QVERIFY(images.addImage(source, QTransform().rotate(45)));
    expectedBounds =
        expectedBounds.united(QTransform().rotate(45).mapRect(imageRect));
    QCOMPARE(images.boundingRect(), expectedBounds);
}

void AbcTest::imageOperations()
{
    Image a = Image::fromFile("32i/0.fit");
    Image b = Image::fromFile("1_32i.fit");
    QCOMPARE(b.type(), Light);
    QCOMPARE(b.temperature(), -20.0016f);
    QCOMPARE(b.exposure(), 900.0f);
    QCOMPARE(b.cameraModel(), QLatin1String("G2-1600, Id: 2115"));
    Image c = Image::fromFile("2_64f.fit");

    QCOMPARE(a - a, b - b);
    QCOMPARE(a - a, c - c);

    Image ab = a + b;
    Image bc = b + c;
    Image ca = a + c;
    Image sum = a + b + c;
    QVERIFY(sum != a);
    QVERIFY(sum != b);
    QVERIFY(sum != c);
    QCOMPARE(sum + sum, ab + bc + ca);
    QCOMPARE(ab + bc, sum + b);
    QCOMPARE(ab + bc - b, sum);
}

void AbcTest::configuration()
{
    Configuration *conf = Configuration::instance();
    QVERIFY(conf != 0);

    QString calibrationFilesDir = conf->calibrationFilesDir();
    QVERIFY(!calibrationFilesDir.isEmpty());
    qDebug() << "Calibration files dir:" << calibrationFilesDir;

    QCOMPARE(conf->calibrationMaxTemperatureDiff(), 1.0f);
}

QTEST_MAIN(AbcTest)
