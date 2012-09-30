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
#include "image.h"

#include <QFileInfo>
#include <fitsio.h>

using namespace ABC;

#define PIXEL_VALUE_FITS_TYPE \
    (sizeof(PixelValue) == sizeof(float) ? TFLOAT : TDOUBLE)
#define FITS_RECORD_LENGTH 81

ImageData::ImageData():
    type(UnknownType),
    temperature(INVALID_TEMPERATURE),
    exposure(-1),
    pixels(0)
{
}

ImageData::ImageData(const ImageData &other):
    QSharedData(other),
    type(other.type),
    temperature(other.temperature),
    exposure(other.exposure),
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

/* Infer the image type from a string. This can be coming either from a header
 * record or the filename.
 * Some of the used values are listed here:
 * http://www.cyanogen.com/help/maximdl/SettingsFITSHeader.htm
 */
static ImageType typeFromString(const QString typeString)
{
    if (typeString.contains("light", Qt::CaseInsensitive)) {
        return Light;
    } else if (typeString.contains("bias", Qt::CaseInsensitive) ||
               typeString.contains("offset", Qt::CaseInsensitive) ||
               typeString == "zero") {
        return Offset;
    } else if (typeString.contains("flat", Qt::CaseInsensitive)) {
        return Flat;
    } else if (typeString.contains("dark", Qt::CaseInsensitive)) {
        return Dark;
    } else {
        return UnknownType;
    }
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

    /* Read the image type. Some of the used values are listed here:
     * http://www.cyanogen.com/help/maximdl/SettingsFITSHeader.htm
     */
    char headerRecord[FITS_RECORD_LENGTH];
    fits_read_key(ff, TSTRING, "IMAGETYP", headerRecord, NULL, &status);
    if (status == 0) {
        QString typeString = QString::fromLatin1(headerRecord);
        type = typeFromString(typeString);
    } else {
        type = UnknownType;
        status = 0;
    }

    /* Read the temperature. */
    fits_read_key(ff, TFLOAT, "CCD-TEMP", &temperature, NULL, &status);
    if (status != 0) {
        temperature = INVALID_TEMPERATURE;
        status = 0;
    }

    /* Exposure time. */
    fits_read_key(ff, TFLOAT, "EXPTIME", &exposure, NULL, &status);
    if (status != 0) {
        status = 0;
        fits_read_key(ff, TFLOAT, "EXPOSURE", &exposure, NULL, &status);
        if (status != 0) {
            exposure = -1;
            status = 0;
        }
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
    bool ok = d->loadFits(fileName);

    if (ok && d->type == UnknownType) {
        QFileInfo fi(fileName);
        d->type = typeFromString(fi.baseName());
    }

    return ok;
}

QImage Image::toQImage() const
{
    // Generate a greyscale colormap
    QVector<QRgb> greyScale(256);
    for (int i = 0; i < 256; i++) {
        greyScale[i] = qRgb(i, i, i);
    }

    // Convert floating point pixels to indexes
    int width = d->size.width();
    int height = d->size.height();
    int numPixels = width * height;
    QByteArray pixels;
    pixels.resize(numPixels);
    for (int i = 0; i < numPixels; i++) {
        pixels[i] = (unsigned char)(d->pixels[i] * 255);
    }
    QImage result((unsigned char *)pixels.constData(),
                  width, height, QImage::Format_Indexed8);
    result.setColorTable(greyScale);
    return result.copy();
}

void Image::divide(const Image &other)
{
    if (other.d->size != d->size) {
        qWarning() << "Size mismatch";
        return;
    }

    long numPixels = d->size.width() * d->size.height();
    for (long i = 0; i < numPixels; i++) {
        // Ignore black pixels
        if (other.d->pixels[i] > 0) {
            d->pixels[i] /= other.d->pixels[i];
        }
    }
}

bool Image::operator==(const Image &other) const
{
    if (other.d->size != d->size) {
        return false;
    }

    long numPixels = d->size.width() * d->size.height();
    for (long i = 0; i < numPixels; i++) {
        if (!qFuzzyCompare(d->pixels[i], other.d->pixels[i])) {
            return false;
        }
    }
    return true;
}

bool Image::operator!=(const Image &other) const
{
    return !(*this == other);
}

Image Image::operator+(const Image &other) const
{
    if (other.d->size != d->size) {
        qWarning() << "Size mismatch";
        return Image();
    }

    Image result;
    long numPixels = result.d->resize(d->size);
    for (long i = 0; i < numPixels; i++) {
        result.d->pixels[i] = d->pixels[i] + other.d->pixels[i];
    }
    return result;
}

Image Image::operator-(const Image &other) const
{
    if (other.d->size != d->size) {
        qWarning() << "Size mismatch";
        return Image();
    }

    Image result;
    long numPixels = result.d->resize(d->size);
    for (long i = 0; i < numPixels; i++) {
        result.d->pixels[i] = d->pixels[i] - other.d->pixels[i];
    }
    return result;
}
