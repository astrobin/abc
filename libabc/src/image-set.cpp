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

#include "debug.h"
#include "image-set.h"

#include <QTransform>
#include <math.h>

using namespace ABC;

namespace ABC {

class ImageSetPrivate: public QSharedData
{
public:
    ImageSetPrivate() {};
    inline ImageSetPrivate(const ImageSetPrivate &other);

    Image uniformAverage() const;
    Image uniformSigmaClip(float sigmaFactor) const;

    QList<Image> images;
    QList<QTransform> transformations;
    Image subtrahend;
    QRect boundingRect;
};

}; // namespace

ImageSetPrivate::ImageSetPrivate(const ImageSetPrivate &other):
    QSharedData(other),
    images(other.images),
    transformations(other.transformations),
    subtrahend(other.subtrahend),
    boundingRect(other.boundingRect)
{
}

Image ImageSetPrivate::uniformAverage() const
{
    Image result;
    QSize size = images[0].size();
    result.resize(size);
    int numImages = images.count();

    int width = size.width();
    PixelValue sumBuffer[width];
    for (int line = 0; line < size.height(); line++) {
        memset(sumBuffer, 0, sizeof(sumBuffer));

        foreach (const Image &image, images) {
            const PixelValue *imagePixels = image.constLine(line);
            for (int i = 0; i < width; i++) {
                sumBuffer[i] += imagePixels[i];
            }
        }

        PixelValue *resultPixels = result.line(line);
        for (int i = 0; i < width; i++) {
            resultPixels[i] = sumBuffer[i] / numImages;
        }
    }

    if (subtrahend.isValid()) {
        result -= subtrahend;
    }

    return result;
}

Image ImageSetPrivate::uniformSigmaClip(float sigmaFactor) const
{
    Image result;
    QSize size = images[0].size();
    result.resize(size);
    int numImages = images.count();

    int width = size.width();
    QVector<const PixelValue*> imagesData(numImages);
    for (int line = 0; line < size.height(); line++) {
        /* Cache the line buffers into a QVector for faster access */
        for (int i = 0; i < numImages; i++) {
            imagesData[i] = images[i].constLine(line);
        }

        PixelValue *resultPixels = result.line(line);

        for (int x = 0; x < width; x++) {
            PixelValue sum = 0;
            for (int i = 0; i < numImages; i++) {
                sum += imagesData[i][x];
            }

            PixelValue average = sum / numImages;

            // Calculate standard deviation
            sum = 0;
            for (int i = 0; i < numImages; i++) {
                sum += powf(imagesData[i][x] - average, 2);
            }
            PixelValue standardDeviation = sqrtf(sum);

            /* Consider only those pixels which don't differ too much from the
             * average. */
            PixelValue min = average - sigmaFactor * standardDeviation;
            PixelValue max = average + sigmaFactor * standardDeviation;
            sum = 0;
            int validImages = 0;
            for (int i = 0; i < numImages; i++) {
                PixelValue pixel = imagesData[i][x];
                if (pixel >= min && pixel <= max) {
                    validImages++;
                    sum += pixel;
                }
            }

            /* The resulting pixel is the average of those pixels with a value
             * within the accepted range */
            if (validImages > 0) {
                resultPixels[x] = sum / validImages;
            } else {
                /* This can happen for too low values of sigmaFactor, or when pixel
                 * values are all too far from the average */
                resultPixels[x] = average;
            }
        }
    }

    if (subtrahend.isValid()) {
        result -= subtrahend;
    }

    return result;
}

ImageSet::ImageSet():
    d(new ImageSetPrivate)
{
}

ImageSet::ImageSet(const ImageSet &other):
    d(other.d)
{
}

ImageSet::~ImageSet()
{
}

bool ImageSet::addImage(const Image &image)
{
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
    return d->boundingRect;
}

Image ImageSet::average() const
{
    if (!d->transformations.isEmpty()) {
        qWarning() << "Average not implemented for transformed images";
        return Image();
    }

    return d->uniformAverage();
}

Image ImageSet::sigmaClip(float sigmaFactor) const
{
    if (!d->transformations.isEmpty()) {
        qWarning() << "Sigma clip not implemented for transformed images";
        return Image();
    }

    return d->uniformSigmaClip(sigmaFactor);
}

void ImageSet::setSubtractCorrection(const Image &subtrahend)
{
    d->subtrahend = subtrahend;
}

void ImageSet::clearCorrections()
{
    d->subtrahend = Image();
}

void ImageSet::divide(const Image &divisor)
{
    for (QList<Image>::iterator i = d->images.begin();
         i != d->images.end();
         i++) {
        i->divide(divisor);
    }
}
