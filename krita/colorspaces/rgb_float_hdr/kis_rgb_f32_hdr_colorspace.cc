/*
 *  Copyright (c) 2006 Cyrille Berger <cberger@cberger.net>
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

#include <limits.h>
#include <stdlib.h>
#include <lcms.h>

#include <QImage>
#include <QColor>

#include <kdebug.h>
#include <klocale.h>

#include <KoIntegerMaths.h>
#include <KoColorSpaceRegistry.h>

#include "kis_rgb_f32_hdr_colorspace.h"

#include "compositeops/KoCompositeOpOver.h"
#include "compositeops/KoCompositeOpErase.h"

KisRgbF32HDRColorSpace::KisRgbF32HDRColorSpace(KoColorSpaceRegistry * parent, KoColorProfile *p)
: KisRgbFloatHDRColorSpace<RgbF32Traits>("RGBAF32", i18n("RGB (32-bit float/channel) for High Dynamic Range imaging"), parent, p)
{
    addChannel(new KoChannelInfo(i18n("Red"), 2 * sizeof(float), KoChannelInfo::COLOR, KoChannelInfo::FLOAT32, sizeof(float), QColor(255,0,0)));
    addChannel(new KoChannelInfo(i18n("Green"), 1 * sizeof(float), KoChannelInfo::COLOR, KoChannelInfo::FLOAT32, sizeof(float), QColor(0,255,0)));
    addChannel(new KoChannelInfo(i18n("Blue"), 0, KoChannelInfo::COLOR, KoChannelInfo::FLOAT32, sizeof(double), QColor(0,0,255)));
    addChannel(new KoChannelInfo(i18n("Alpha"), 3 * sizeof(float), KoChannelInfo::ALPHA, KoChannelInfo::FLOAT32));

    addCompositeOp( new KoCompositeOpOver<RgbF32Traits>( this ) );
    addCompositeOp( new KoCompositeOpErase<RgbF32Traits>( this ) );
}
