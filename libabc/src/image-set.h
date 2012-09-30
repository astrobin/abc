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

#ifndef ABC_IMAGE_SET_H
#define ABC_IMAGE_SET_H

#include "image.h"

#include <QRect>

class QTransform;

namespace ABC {

class ImageSetPrivate;
class ImageSet
{
public:
    ImageSet();
    ImageSet(const ImageSet &other);
    virtual ~ImageSet();

    bool addImage(const Image &image);
    bool addImage(const Image &image, const QTransform &transform);

    QRect boundingRect() const;

    Image average() const;
    Image sigmaClip(float sigmaFactor) const;

    void setSubtractCorrection(const Image &subtrahend);
    void clearCorrections();

private:
    QSharedDataPointer<ImageSetPrivate> d;
};

}; // namespace

#endif /* ABC_IMAGE_SET_H */
