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

#include "image.h"
#include <QDebug>

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

QTEST_MAIN(AbcTest)
