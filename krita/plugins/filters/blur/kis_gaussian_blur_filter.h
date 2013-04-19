/* This file is part of Krita
 *
 * Copyright (c) 2009 Edward Apap <schumifer@hotmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */


#ifndef KIS_GAUSSIAN_BLUR_FILTER_H
#define KIS_GAUSSIAN_BLUR_FILTER_H

#include "filter/kis_filter.h"
#include "ui_wdg_gaussian_blur.h"

#include <Eigen/Core>

using namespace Eigen;

class KisGaussianBlurFilter : public KisFilter
{
public:
    KisGaussianBlurFilter();
public:
    using KisFilter::process;

    void processImpl(KisPaintDeviceSP device,
                     const QRect& rect,
                     const KisFilterConfiguration* config,
                     KoUpdater* progressUpdater
                     ) const;
    static inline KoID id() {
        return KoID("gaussian blur", i18n("Gaussian Blur"));
    }

    virtual KisFilterConfiguration* factoryConfiguration(const KisPaintDeviceSP) const;
public:
    KisConfigWidget * createConfigurationWidget(QWidget* parent, const KisPaintDeviceSP dev) const;
    QRect neededRect(const QRect & rect, const KisFilterConfiguration*) const;
    QRect changedRect(const QRect & rect, const KisFilterConfiguration*) const;
};

#endif
