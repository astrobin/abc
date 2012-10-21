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

#include "configuration.h"
#include "debug.h"

#include <QDesktopServices>
#include <QFileInfo>

using namespace ABC;

static const QLatin1String keyCalibrationFilesDir("CalibrationFilesDir");
static const QLatin1String keyCalibrationMaxTemperatureDiff("CalibrationMax"
                                                            "TemperatureDiff");

static Configuration *configurationInstance = 0;

namespace ABC {
class ConfigurationPrivate
{
    Q_DECLARE_PUBLIC(Configuration)

    ConfigurationPrivate(Configuration *configuration):
        q_ptr(configuration)
    {
    }

    ~ConfigurationPrivate()
    {
    }

private:
    QSettings settings;
    mutable Configuration *q_ptr;
};
};

Configuration::Configuration():
    d_ptr(new ConfigurationPrivate(this))
{
}

Configuration::~Configuration()
{
    delete d_ptr;
}

Configuration *Configuration::instance()
{
    if (configurationInstance == 0) {
        configurationInstance = new Configuration();
    }

    return configurationInstance;
}

QString Configuration::calibrationFilesDir() const
{
    Q_D(const Configuration);
    if (d->settings.contains(keyCalibrationFilesDir))
        return d->settings.value(keyCalibrationFilesDir).toString();

    return QDesktopServices::storageLocation(QDesktopServices::DataLocation) +
        QLatin1String("/CalibrationFiles/");
}

float Configuration::calibrationMaxTemperatureDiff() const
{
    Q_D(const Configuration);
    return d->settings.value(keyCalibrationMaxTemperatureDiff, 1.0).toFloat();
}
