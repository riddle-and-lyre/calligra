/*
 *  Copyright (c) 2006-2007 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef _KO_COLORSPACE_TRAITS_H_
#define _KO_COLORSPACE_TRAITS_H_

#include <QVector>

#include "KoColorSpaceConstants.h"
#include "KoColorSpaceMaths.h"
#include "DebugPigment.h"

/**
 * This class is the base class to define the main characteristics of a colorspace
 * which inherits KoColorSpaceAbstract.
 *
 * - _channels_type_ is the type of the value use for each channel, for example quint8 for 8bits per channel
 *                   color spaces, or quint16 for 16bits integer per channel, float for 32bits per channel
 *                   floating point color spaces
 * - _channels_nb_ is the total number of channels in an image (for example RGB is 3 but RGBA is four)
 * - _alpha_pos_ is the position of the alpha channel among the channels, if there is no alpha channel,
 *               then _alpha_pos_ is set to -1
 *
 * For instance a colorspace of three color channels and alpha channel in 16bits,
 * will be defined as KoColorSpaceTrait\<quint16, 4, 3\>. The same without the alpha
 * channel is KoColorSpaceTrait\<quint16,3,-1\>
 *
 */
template<typename _channels_type_, int _channels_nb_, int _alpha_pos_>
struct KoColorSpaceTrait {

    /// the type of the value of the channels of this color space
    typedef _channels_type_ channels_type;

    /// the number of channels in this color space
    static const quint32 channels_nb = _channels_nb_;

    /// the position of the alpha channel in the channels of the pixel (or -1 if no alpha
    /// channel.
    static const qint32 alpha_pos = _alpha_pos_;

    /// the number of bit for each channel
    static const int depth = KoColorSpaceMathsTraits<_channels_type_>::bits;

    /**
     * @return the size in byte of one pixel
     */
    static const quint32 pixelSize = channels_nb * sizeof(channels_type);

    /**
     * @return the value of the alpha channel for this pixel in the 0..255 range
     */
    inline static quint8 opacityU8(const quint8 *U8_pixel)
    {
        if (alpha_pos < 0) {
            return OPACITY_OPAQUE_U8;
        }
        channels_type c = nativeArray(U8_pixel)[alpha_pos];
        return  KoColorSpaceMaths<channels_type, quint8>::scaleToA(c);
    }

    inline static qreal opacityF(const quint8 *U8_pixel)
    {
        if (alpha_pos < 0) {
            return OPACITY_OPAQUE_F;
        }
        channels_type c = nativeArray(U8_pixel)[alpha_pos];
        return  KoColorSpaceMaths<channels_type, qreal>::scaleToA(c);
    }

    /**
     * Set the alpha channel for this pixel from a value in the 0..255 range
     */
    inline static void setOpacity(quint8 *pixels, quint8 alpha, qint32 nPixels)
    {
        if (alpha_pos < 0) {
            return;
        }
        qint32 psize = pixelSize;
        channels_type valpha =  KoColorSpaceMaths<quint8, channels_type>::scaleToA(alpha);
        for (; nPixels > 0; --nPixels, pixels += psize) {
            nativeArray(pixels)[alpha_pos] = valpha;
        }
    }

    inline static void setOpacity(quint8 *pixels, qreal alpha, qint32 nPixels)
    {
        if (alpha_pos < 0) {
            return;
        }
        qint32 psize = pixelSize;
        channels_type valpha =  KoColorSpaceMaths<qreal, channels_type>::scaleToA(alpha);
        for (; nPixels > 0; --nPixels, pixels += psize) {
            nativeArray(pixels)[alpha_pos] = valpha;
        }
    }

    /**
     * Convenient function for transforming a quint8* array in a pointer of the native channels type
     */
    inline static const channels_type *nativeArray(const quint8 *a)
    {
        return reinterpret_cast<const channels_type *>(a);
    }

    /**
     * Convenient function for transforming a quint8* array in a pointer of the native channels type
     */
    inline static channels_type *nativeArray(quint8 *a)
    {
        return reinterpret_cast< channels_type *>(a);
    }

    /**
     * Allocate nPixels pixels for this colorspace.
     */
    inline static quint8 *allocate(quint32 nPixels)
    {
        return new quint8[ nPixels * pixelSize ];
    }

    inline static void singleChannelPixel(quint8 *dstPixel, const quint8 *srcPixel, quint32 channelIndex)
    {
        const channels_type *src = nativeArray(srcPixel);
        channels_type *dst = nativeArray(dstPixel);
        for (uint i = 0; i < channels_nb; i++) {
            if (i != channelIndex) {
                dst[i] = 0;
            } else {
                dst[i] = src[i];
            }
        }
    }

    inline static QString channelValueText(const quint8 *pixel, quint32 channelIndex)
    {
        if (channelIndex > channels_nb) {
            return QString("Error");
        }
        channels_type c = nativeArray(pixel)[channelIndex];
        return QString().setNum(c);
    }

    inline static QString normalisedChannelValueText(const quint8 *pixel, quint32 channelIndex)
    {
        if (channelIndex > channels_nb) {
            return QString("Error");
        }
        channels_type c = nativeArray(pixel)[channelIndex];
        return QString().setNum(100. *((qreal)c) / KoColorSpaceMathsTraits< channels_type>::unitValue);
    }

    inline static void normalisedChannelsValue(const quint8 *pixel, QVector<float> &channels)
    {
        Q_ASSERT((int)channels.count() == (int)channels_nb);
        channels_type c;
        for (uint i = 0; i < channels_nb; i++) {
            c = nativeArray(pixel)[i];
            channels[i] = ((qreal)c) / KoColorSpaceMathsTraits<channels_type>::unitValue;
        }
    }

    inline static void fromNormalisedChannelsValue(quint8 *pixel, const QVector<float> &values)
    {
        Q_ASSERT((int)values.count() == (int)channels_nb);
        channels_type c;
        for (uint i = 0; i < channels_nb; i++) {
            c = (channels_type)
                ((float)KoColorSpaceMathsTraits<channels_type>::unitValue * values[i]);
            nativeArray(pixel)[i] = c;
        }
    }
    inline static void multiplyAlpha(quint8 *pixels, quint8 alpha, qint32 nPixels)
    {
        if (alpha_pos < 0) {
            return;
        }

        channels_type valpha =  KoColorSpaceMaths<quint8, channels_type>::scaleToA(alpha);

        for (; nPixels > 0; --nPixels, pixels += pixelSize) {
            channels_type *alphapixel = nativeArray(pixels) + alpha_pos;
            *alphapixel = KoColorSpaceMaths<channels_type>::multiply(*alphapixel, valpha);
        }
    }

    inline static void applyAlphaU8Mask(quint8 *pixels, const quint8 *alpha, qint32 nPixels)
    {
        if (alpha_pos < 0) {
            return;
        }

        for (; nPixels > 0; --nPixels, pixels += pixelSize, ++alpha) {
            channels_type valpha =  KoColorSpaceMaths<quint8, channels_type>::scaleToA(*alpha);
            channels_type *alphapixel = nativeArray(pixels) + alpha_pos;
            *alphapixel = KoColorSpaceMaths<channels_type>::multiply(*alphapixel, valpha);
        }
    }

    inline static void applyInverseAlphaU8Mask(quint8 *pixels, const quint8 *alpha, qint32 nPixels)
    {
        if (alpha_pos < 0) {
            return;
        }

        for (; nPixels > 0; --nPixels, pixels += pixelSize, ++alpha) {
            channels_type valpha =  KoColorSpaceMaths<quint8, channels_type>::scaleToA(OPACITY_OPAQUE_U8 - *alpha);
            channels_type *alphapixel = nativeArray(pixels) + alpha_pos;
            *alphapixel = KoColorSpaceMaths<channels_type>::multiply(*alphapixel, valpha);
        }
    }

    inline static void applyAlphaNormedFloatMask(quint8 *pixels, const float *alpha, qint32 nPixels)
    {
        if (alpha_pos < 0) {
            return;
        }

        for (; nPixels > 0; --nPixels, pixels += pixelSize, ++alpha) {
            channels_type valpha =  channels_type(KoColorSpaceMathsTraits<channels_type>::unitValue * (*alpha));
            channels_type *alphapixel = nativeArray(pixels) + alpha_pos;
            *alphapixel = KoColorSpaceMaths<channels_type>::multiply(*alphapixel, valpha);
        }
    }

    inline static void applyInverseAlphaNormedFloatMask(quint8 *pixels, const float *alpha, qint32 nPixels)
    {
        if (alpha_pos < 0) {
            return;
        }

        for (; nPixels > 0; --nPixels, pixels += pixelSize, ++alpha) {
            channels_type valpha =  channels_type(KoColorSpaceMathsTraits<channels_type>::unitValue * (1.0f - *alpha));
            channels_type *alphapixel = nativeArray(pixels) + alpha_pos;
            *alphapixel = KoColorSpaceMaths<channels_type>::multiply(*alphapixel, valpha);
        }
    }
};

#include "KoRgbColorSpaceTraits.h"
#include "KoBgrColorSpaceTraits.h"
#include "KoGrayColorSpaceTraits.h"
#include "KoLabColorSpaceTraits.h"
#include "KoXyzColorSpaceTraits.h"
#include "KoYcbcrColorSpaceTraits.h"
#include "KoCmykColorSpaceTraits.h"

#endif
