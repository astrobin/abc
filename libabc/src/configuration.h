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

#ifndef ABC_CONFIGURATION_H
#define ABC_CONFIGURATION_H

#include <QSettings>
#include <QString>

namespace ABC {

class ConfigurationPrivate;
class Configuration
{
public:
    static Configuration *instance();

    QString calibrationFilesDir() const;

private:
    Configuration();
    virtual ~Configuration();

private:
    ConfigurationPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Configuration)
};

}; // namespace

#endif /* ABC_CONFIGURATION_H */
