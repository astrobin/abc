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
#include <QSharedDataPointer>
#include <QString>

#define INVALID_TEMPERATURE (-300)

namespace ABC {

// This can be changed to double if more precision is needed
typedef float PixelValue;

enum ImageType {
    UnknownType = 0,
    Light,
    Offset,
    Dark,
    Flat,
};

class AbcTest;
class ImageSetPrivate;

class ImageData;
class Image
{
public:
    Image();
    Image(const Image &other);
    virtual ~Image();

    static Image fromFile(const QString &fileName);

    bool load(const QString &fileName);

    ImageType type() const;
    bool isValid() const;
    QSize size() const;
    QImage toQImage() const;

    PixelValue *pixels();
    const PixelValue *pixels() const;
    const PixelValue *constPixels() const;
    PixelValue *line(int l);
    const PixelValue *line(int l) const;
    const PixelValue *constLine(int l) const;

    float temperature() const;
    bool hasTemperature() const;

    float exposure() const;

    QString cameraModel() const;

    void divide(const Image &other);

    Image &operator=(const Image &other);
    bool operator==(const Image &other) const;
    bool operator!=(const Image &other) const;
    Image operator+(const Image &other) const;
    Image operator-(const Image &other) const;

private:
    long resize(const QSize &size);

private:
    friend class AbcTest;
    friend class ImageSetPrivate;
    QSharedDataPointer<ImageData> d;
};

}; // namespace

#endif /* ABC_IMAGE_H */
