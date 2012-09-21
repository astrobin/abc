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

#include "image.h"

#include <QDebug>
#include <fitsio.h>

using namespace ABC;

#define PIXEL_VALUE_FITS_TYPE \
    (sizeof(PixelValue) == sizeof(float) ? TFLOAT : TDOUBLE)

ImageData::ImageData():
    pixels(0)
{
}

ImageData::ImageData(const ImageData &other):
    QSharedData(other),
    pixels(0)
{
    long numPixels = resize(other.size);
    memcpy(pixels, other.pixels, numPixels * sizeof(PixelValue));
}

ImageData::~ImageData()
{
    delete pixels;
    pixels = 0;
}

bool ImageData::loadFits(const QString &fileName)
{
    fitsfile *ff;
    int status = 0;

    fits_open_image(&ff, fileName.toUtf8().constData(),
                    READWRITE, &status);
    if (status != 0) return false;

    int numAxes = 0;
    fits_get_img_dim(ff, &numAxes, &status);
    if (numAxes != 2) {
        qWarning() << "Only 2D FITS images are supported";
        fits_close_file(ff, &status);
        return false;
    }

    long axes[2];
    fits_get_img_size(ff, 2, axes, &status);
    if (status != 0) {
        fits_close_file(ff, &status);
        return false;
    }

    long numPixels = resize(QSize(axes[0], axes[1]));
    long firstPixels[2];
    firstPixels[0] = firstPixels[1] = 1;
    fits_read_pix(ff, PIXEL_VALUE_FITS_TYPE, firstPixels, numPixels,
                  NULL, pixels, NULL, &status);
    if (status != 0) {
        delete pixels;
        pixels = 0;
        fits_close_file(ff, &status);
        return false;
    }

    fits_close_file(ff, &status);
    return true;
}

long ImageData::resize(const QSize &newSize)
{
    long numPixels = newSize.width() * newSize.height();
    if (size == newSize) return numPixels;

    size = newSize;
    delete pixels;
    pixels = new PixelValue[numPixels];
    return numPixels;
}

Image Image::fromFile(const QString &fileName)
{
    Image image;
    image.load(fileName);
    return image;
}

bool Image::load(const QString &fileName)
{
    // TODO: if loading a FITS fail, fallback to a raw file
    return d->loadFits(fileName);
}

void Image::setAverageOf(const QList<Image> &images)
{
    // This method assumes that all images have the same size
    if (images.isEmpty()) {
        d = new ImageData;
        return;
    }

    long numPixels = d->resize(images[0].size());
    int numImages = images.count();
    for (long i = 0; i < numPixels; i++) {
        PixelValue sum = 0;
        foreach (const Image &image, images) {
            sum += image.d->pixels[i];
        }
        d->pixels[i] = sum / numImages;
    }
}
