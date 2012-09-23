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

    bool ok = image.load("UIT.fits");
    QVERIFY(ok);
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
    for (long i = 0; i < numPixels; i++) {
        QVERIFY(qFuzzyCompare(average.d->pixels[i], source.d->pixels[i]));
    }
}

void AbcTest::imageSetBounds()
{
    Image source;

    bool ok = source.load("UIT.fits");
    QVERIFY(ok);

    ImageSet images;
    QVERIFY(images.addImage(source, QTransform()));

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

QTEST_MAIN(AbcTest)
