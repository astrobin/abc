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

#include "calibration-set.h"
#include "configuration.h"
#include "debug.h"
#include "image.h"

#include <QDir>

using namespace ABC;

namespace ABC {

class CalibrationSetPrivate
{
    friend class CalibrationSet;

    inline CalibrationSetPrivate();

    void load();
    void loadImageSets(const QDir &baseDir);
    void loadImageSet(const QDir &baseDir,
                      Calibration::FileType fileType);
    inline void findClosestTemperatures(int goalTemperature,
                                        const QList<int> &temperatures,
                                        int &lower, int &upper);

private:
    ImageSet sets[Calibration::NumFileTypes];
    QString camera;
    float temperature;
    float exposure;
};

}; // namespace

static QString stringToFileName(const QString &text)
{
    QString result;
    int length = text.size();
    result.reserve(length);
    for (int i; i < length; i++) {
        QChar ch = text.at(i);
        if (ch.isLetterOrNumber() || ch == ' ' || ch == '_' || ch == '-') {
            result.append(ch);
        }
    }
    return result;
}

CalibrationSetPrivate::CalibrationSetPrivate():
    temperature(INVALID_TEMPERATURE),
    exposure(0)
{
}

void CalibrationSetPrivate::load()
{
    QDir baseDir(Configuration::instance()->calibrationFilesDir());

    // TODO: add error codes

    if (!baseDir.exists()) {
        DEBUG() << "Directory not found:" << baseDir.dirName();
        return;
    }

    if (!baseDir.cd(stringToFileName(camera))) {
        DEBUG() << "Camera not found:" << camera;
        return;
    }

    /* Find the folder corresponding to the desired temperature.
     * TODO: implement interpolation. */
    QStringList subDirs = baseDir.entryList(QStringList("T????"),
                                            QDir::Dirs | QDir::NoDotAndDotDot,
                                            QDir::Name);
    if (subDirs.isEmpty()) {
        DEBUG() << "No temperatures";
        return;
    }

    /* convert directory names to temperatures; directory names are formatted
     * like this:
     * T####, where #### is the temperature, in K, multiplied by 10.
     */
    QList<int> availableTemperatures;
    foreach (const QString &subDir, subDirs) {
        availableTemperatures.append(subDir.mid(1).toInt());
    }

    int lower = 0, upper = 0;
    int goalTemperature = temperature * 10;
    findClosestTemperatures(goalTemperature, availableTemperatures,
                            lower, upper);
    int diffLower = qAbs(goalTemperature - lower);
    int diffUpper = qAbs(goalTemperature - upper);
    int chosenTemperature = diffUpper <= diffLower ? upper : lower;
    float diff = (goalTemperature - chosenTemperature) / 10.0;
    if (diff > Configuration::instance()->calibrationMaxTemperatureDiff()) {
        DEBUG() << "No suitable calibration files. Difference is" << diff;
        return;
    }

    /* Enter the temperature directory and load all files */
    if (!baseDir.cd(QString::fromLatin1("T%1").arg(chosenTemperature, 4, 10,
                                                   QLatin1Char('0')))) {
        DEBUG() << "Couldn't cd to temperature" << chosenTemperature;
        return;
    }

    loadImageSets(baseDir);
}

void CalibrationSetPrivate::loadImageSets(const QDir &baseDir)
{
    loadImageSet(baseDir, Calibration::Offset);
    loadImageSet(baseDir, Calibration::Dark);
    loadImageSet(baseDir, Calibration::DarkFlat);
    loadImageSet(baseDir, Calibration::Flat);
}

static QString directoryFromType(Calibration::FileType fileType)
{
    switch (fileType) {
    case Calibration::Offset:
        return "Offsets";
    case Calibration::Dark:
        return "Darks";
    case Calibration::DarkFlat:
        return "DarkFlats";
    case Calibration::Flat:
        return "Flats";
    default:
        qCritical() << "Calibration file type unknown" << fileType;
        return "";
    }
}

void CalibrationSetPrivate::loadImageSet(const QDir &baseDir,
                                         Calibration::FileType fileType)
{
    QDir dir(baseDir);
    QString subDir = directoryFromType(fileType);
    if (!dir.cd(subDir)) {
        DEBUG() << "Calibration directory not found:" << subDir;
        return;
    }

    foreach (const QString fileName,
             dir.entryList(QDir::Files | QDir::Readable, QDir::Name)) {
        Image image = Image::fromFile(fileName);

        if (image.isValid()) {
            sets[fileType].addImage(image);
        }
    }
}

void
CalibrationSetPrivate::findClosestTemperatures(int goalTemperature,
                                               const QList<int> &temperatures,
                                               int &lower, int &upper)
{
    foreach (int t, temperatures) {
        if (t <= goalTemperature) {
            if (t > lower) lower = t;
        } else {
            if (t < upper) upper = t;
        }
    }
}

CalibrationSet::CalibrationSet():
    d_ptr(new CalibrationSetPrivate)
{
}

CalibrationSet::~CalibrationSet()
{
    delete d_ptr;
    d_ptr = 0;
}

void CalibrationSet::load(const Image &image)
{
    Q_D(CalibrationSet);

    d->camera = image.cameraModel();
    d->temperature = image.temperature();
    d->exposure = image.exposure();
    d->load();
}
