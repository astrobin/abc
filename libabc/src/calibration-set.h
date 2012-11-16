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

#ifndef ABC_CALIBRATION_SET_H
#define ABC_CALIBRATION_SET_H

#include "image-set.h"

namespace ABC {

namespace Calibration {
    enum FileType {
        Offset,
        Dark,
        DarkFlat,
        Flat,
        NumFileTypes
    };
};

class CalibrationSetPrivate;
class CalibrationSet
{
public:
    CalibrationSet();
    virtual ~CalibrationSet();

    void load(const Image &image);

    ImageSet set(Calibration::FileType fileType) const;
    ImageSet offsets() const { return set(Calibration::Offset); }
    ImageSet darks() const { return set(Calibration::Dark); }
    ImageSet darkFlats() const { return set(Calibration::DarkFlat); }
    ImageSet flats() const { return set(Calibration::Flat); }

private:
    CalibrationSetPrivate *d_ptr;
    Q_DECLARE_PRIVATE(CalibrationSet)
};

}; // namespace

#endif /* ABC_CALIBRATION_SET_H */
