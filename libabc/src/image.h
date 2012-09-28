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

#ifndef ABC_IMAGE_H
#define ABC_IMAGE_H

#include <QImage>
#include <QSize>
#include <QSharedData>
#include <QString>

namespace ABC {

// This can be changed to double if more precision is needed
typedef float PixelValue;

class ImageData: public QSharedData
{
public:
    ImageData();
    ImageData(const ImageData &other);
    ~ImageData();

    bool loadFits(const QString &fileName);
    long resize(const QSize &newSize);

    QSize size;
    PixelValue *pixels;
};

class AbcTest;
class ImageSetPrivate;

class Image
{
public:
    Image() { d = new ImageData; }
    Image(const Image &other): d(other.d) {}
    virtual ~Image() {};

    static Image fromFile(const QString &fileName);

    bool load(const QString &fileName);

    bool isValid() const { return d->size.isValid(); }
    QSize size() const { return d->size; }
    QImage toQImage() const;

    bool operator==(const Image &other) const;
    bool operator!=(const Image &other) const;
    Image operator+(const Image &other) const;
    Image operator-(const Image &other) const;

private:
    friend class AbcTest;
    friend class ImageSetPrivate;
    QSharedDataPointer<ImageData> d;
};

}; // namespace

#endif /* ABC_IMAGE_H */
