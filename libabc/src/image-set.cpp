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

#include "image-set.h"

#include <QDebug>
#include <QTransform>

using namespace ABC;

namespace ABC {

class ImageSetPrivate
{
    friend class ImageSet;

    ImageSetPrivate() {};

    QList<Image> images;
    QList<QTransform> transformations;
    QRect boundingRect;
};

}; // namespace

ImageSet::ImageSet():
    d_ptr(new ImageSetPrivate)
{
}

ImageSet::~ImageSet()
{
    delete d_ptr;
    d_ptr = 0;
}

bool ImageSet::addImage(const Image &image)
{
    Q_D(ImageSet);

    // Fail if this set contains images with transformations
    if (!d->transformations.isEmpty()) return false;

    // Fail if the image size is different from the images already inserted
    bool isFirstImage = d->images.isEmpty();
    if (!isFirstImage && d->boundingRect.size() != image.size())
        return false;

    d->images.append(image);
    if (isFirstImage) {
        d->boundingRect = QRect(QPoint(0, 0), image.size());
    }
    return true;
}

bool ImageSet::addImage(const Image &image, const QTransform &transform)
{
    Q_D(ImageSet);

    // Fail if this set contains images without transformations
    if (d->transformations.count() != d->images.count()) return false;

    d->images.append(image);
    d->transformations.append(transform);
    // update the bounding rect
    QRect imageRect(QPoint(0, 0), image.size());
    d->boundingRect = d->boundingRect.united(transform.mapRect(imageRect));

    return true;
}

QRect ImageSet::boundingRect() const
{
    Q_D(const ImageSet);
    return d->boundingRect;
}

Image ImageSet::average() const
{
    Q_D(const ImageSet);

    if (!d->transformations.isEmpty()) {
        qWarning() << "Average not implemented for transformed images";
        return Image();
    }

    Image result;
    result.setAverageOf(d->images);
    return result;
}
