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

namespace ABC {

class ImageData: public QSharedData
{
public:
    ImageData();
    ImageData(const ImageData &other);
    ~ImageData();

    inline bool ensureFitsOpen() const;
    bool loadFits();
    long resize(const QSize &newSize);

    long totalPixels() const { return size.width() * size.height(); }

    inline void loadPixels();
    inline PixelValue *pixelData();
    inline const PixelValue *constPixelData() const;
    inline PixelValue *line(int l);
    inline const PixelValue *constLine(int l) const;

    bool operator==(const Image &other) const;
    bool operator!=(const Image &other) const;
    Image operator+(const Image &other) const;
    Image operator-(const Image &other) const;

    QString fileName;
    mutable fitsfile *ff;
    ImageType type;
    float temperature;
    float exposure;
    QString cameraModel;
    QSize size;
    mutable PixelValue *lineBuffer;
    PixelValue *pixels;
};

} // namespace

ImageData::ImageData():
    ff(0),
    type(UnknownType),
    temperature(INVALID_TEMPERATURE),
    exposure(-1),
    lineBuffer(0),
    pixels(0)
{
}

ImageData::ImageData(const ImageData &other):
    QSharedData(other),
    fileName(other.fileName),
    ff(0),
    type(other.type),
    temperature(other.temperature),
    exposure(other.exposure),
    cameraModel(other.cameraModel),
    lineBuffer(0),
    pixels(0)
{
    long numPixels = resize(other.size);
    memcpy(pixels, other.pixels, numPixels * sizeof(PixelValue));
}

ImageData::~ImageData()
{
    if (ff != 0) {
        int status = 0;
        fits_close_file(ff, &status);
    }
    delete pixels;
    pixels = 0;
    delete lineBuffer;
    lineBuffer = 0;
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

bool ImageData::ensureFitsOpen() const
{
    if (ff == 0) {
        int status = 0;
        fits_open_image(&ff, fileName.toUtf8().constData(),
                        READWRITE, &status);
        if (status != 0) return false;
    }

    return true;
}

bool ImageData::loadFits()
{
    int status = 0;

    if (ff != 0) {
        fits_close_file(ff, &status);
        status = 0;
    }

    if (!ensureFitsOpen()) return false;

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

    /* Camera model. */
    fits_read_key(ff, TSTRING, "INSTRUME", headerRecord, NULL, &status);
    if (status == 0) {
        cameraModel = QString::fromLatin1(headerRecord);
    } else {
        cameraModel = QString();
        status = 0;
    }

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

void ImageData::loadPixels()
{
    if (!ensureFitsOpen()) return;

    long numPixels = totalPixels();
    Q_ASSERT(pixels == 0);
    pixels = new PixelValue[numPixels];

    long firstPixels[2];
    firstPixels[0] = firstPixels[1] = 1;
    int status = 0;
    fits_read_pix(ff, PIXEL_VALUE_FITS_TYPE, firstPixels, numPixels,
                  NULL, pixels, NULL, &status);
    if (status != 0) {
        delete pixels;
        pixels = 0;
        status = 0;
    }

    /* We won't need to access the fits file anymore */
    fits_close_file(ff, &status);
    ff = 0;

    /* or the line buffer either */
    delete lineBuffer;
    lineBuffer = 0;
}

PixelValue *ImageData::pixelData()
{
    if (pixels == 0) loadPixels();
    return pixels;
}

const PixelValue *ImageData::constPixelData() const
{
    return pixels;
}

PixelValue *ImageData::line(int l)
{
    return pixelData() + l * size.width();
}

const PixelValue *ImageData::constLine(int l) const
{
    int width = size.width();

    /* If the data is already loaded, just return it */
    if (pixels != 0) return pixels + l * width;

    /* TODO: support raw files */
    if (!ensureFitsOpen()) return 0;

    long numPixels = size.width();
    if (lineBuffer == 0) {
        lineBuffer = new PixelValue[numPixels];
    }

    int status = 0;
    long firstPixels[2];
    firstPixels[0] = 1;
    firstPixels[1] = l;
    fits_read_pix(ff, PIXEL_VALUE_FITS_TYPE, firstPixels, numPixels,
                  NULL, lineBuffer, NULL, &status);
    if (status != 0) {
        delete lineBuffer;
        lineBuffer = 0;
        status = 0;
    }

    return lineBuffer;
}

Image::Image():
    d(new ImageData)
{
}

Image::Image(const Image &other):
    d(other.d)
{
}

Image::~Image()
{
}

Image Image::fromFile(const QString &fileName, const QString &label)
{
    Image image;
    image.load(fileName, label);
    return image;
}

bool Image::load(const QString &fileName, const QString &label)
{
    d->fileName = fileName;

    // TODO: if loading a FITS fail, fallback to a raw file
    bool ok = d->loadFits();

    if (ok && d->type == UnknownType) {
        if (!label.isEmpty()) {
            d->type = typeFromString(label);
        } else {
            QFileInfo fi(fileName);
            d->type = typeFromString(fi.baseName());
        }
    }

    return ok;
}

ImageType Image::type() const
{
    return d->type;
}

bool Image::isValid() const
{
    return d->size.isValid();
}

QSize Image::size() const
{
    return d->size;
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
    const PixelValue *thisPixels = constPixels();
    QByteArray pixels;
    pixels.resize(numPixels);
    for (int i = 0; i < numPixels; i++) {
        pixels[i] = (unsigned char)(thisPixels[i] * 255);
    }
    QImage result((unsigned char *)pixels.constData(),
                  width, height, QImage::Format_Indexed8);
    result.setColorTable(greyScale);
    return result.copy();
}

PixelValue *Image::pixels()
{
    return d->pixelData();
}

const PixelValue *Image::pixels() const
{
    return d->constPixelData();
}

const PixelValue *Image::constPixels() const
{
    return d->constPixelData();
}

PixelValue *Image::line(int l)
{
    return d->line(l);
}

const PixelValue *Image::line(int l) const
{
    return d->constLine(l);
}

const PixelValue *Image::constLine(int l) const
{
    return d->constLine(l);
}

float Image::temperature() const
{
    return d->temperature;
}

bool Image::hasTemperature() const
{
    return d->temperature != INVALID_TEMPERATURE;
}

float Image::exposure() const
{
    return d->exposure;
}

QString Image::cameraModel() const
{
    return d->cameraModel;
}

void Image::divide(const Image &other)
{
    if (other.d->size != d->size) {
        qWarning() << "Size mismatch";
        return;
    }

    long numPixels = d->size.width() * d->size.height();
    PixelValue *thisPixels = pixels();
    const PixelValue *otherPixels = other.constPixels();
    for (long i = 0; i < numPixels; i++) {
        // Ignore black pixels
        if (otherPixels[i] > 0) {
            thisPixels[i] /= otherPixels[i];
        }
    }
}

Image &Image::operator=(const Image &other)
{
    if (this != &other) {
        d = other.d;
    }
    return *this;
}

bool Image::operator==(const Image &other) const
{
    if (other.d->size != d->size) {
        return false;
    }

    long numPixels = d->size.width() * d->size.height();
    const PixelValue *thisPixels = constPixels();
    const PixelValue *otherPixels = other.constPixels();
    for (long i = 0; i < numPixels; i++) {
        if (!qFuzzyCompare(thisPixels[i], otherPixels[i])) {
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
    PixelValue *resultPixels = result.pixels();
    const PixelValue *thisPixels = constPixels();
    const PixelValue *otherPixels = other.constPixels();
    for (long i = 0; i < numPixels; i++) {
        resultPixels[i] = thisPixels[i] + otherPixels[i];
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
    PixelValue *resultPixels = result.pixels();
    const PixelValue *thisPixels = constPixels();
    const PixelValue *otherPixels = other.constPixels();
    for (long i = 0; i < numPixels; i++) {
        resultPixels[i] = thisPixels[i] - otherPixels[i];
    }
    return result;
}

Image &Image::operator-=(const Image &other)
{
    if (other.d->size != d->size) {
        qWarning() << "Size mismatch";
        return *this;
    }

    long numPixels = d->size.width() * d->size.height();
    PixelValue *thisPixels = pixels();
    const PixelValue *otherPixels = other.constPixels();
    for (long i = 0; i < numPixels; i++) {
        thisPixels[i] -= otherPixels[i];
        if (thisPixels[i] < 0) thisPixels[i] = 0;
    }
    return *this;
}

long Image::resize(const QSize &size)
{
    return d->resize(size);
}
