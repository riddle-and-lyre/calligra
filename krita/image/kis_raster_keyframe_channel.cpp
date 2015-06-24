/*
 *  Copyright (c) 2015 Jouni Pentikäinen <joupent@gmail.com>
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
#include "kis_raster_keyframe_channel.h"
#include "kis_node.h"

#include "kis_paint_device.h"

struct KisRasterKeyframeChannel::Private
{
  Private(KisPaintDeviceWSP paintDevice)
      : paintDevice(paintDevice)
  {}

  KisPaintDeviceWSP paintDevice;
};

KisRasterKeyframeChannel::KisRasterKeyframeChannel(const KoID &id, const KisNodeWSP node, const KisPaintDeviceWSP paintDevice)
    : KisKeyframeChannel(id, node),
      m_d(new Private(paintDevice))
{
    // Raster channels always have at least one frame (representing a static image)
    addKeyframe(0);
}

KisRasterKeyframeChannel::~KisRasterKeyframeChannel()
{
    delete m_d;
}

int KisRasterKeyframeChannel::frameIdAt(int time) const
{
    KisKeyframe *key = activeKeyframeAt(time);
    return key->value();
}

bool KisRasterKeyframeChannel::fetchFrame(KisPaintDeviceSP targetDevice, int time, int offset)
{
    QMap<int, KisKeyframe*>::const_iterator i = activeKeyIterator(time);

    while (offset > 0 && i != constKeys().end()) {
        offset--;
        i++;
    }
    while (offset < 0 && i != constKeys().begin()) {
        offset++;
        i--;
    }

    if (offset != 0 || i == constKeys().end()) return false;

    m_d->paintDevice->fetchFrame(i.value()->value(), targetDevice);

    return true;
}

KisKeyframe *KisRasterKeyframeChannel::createKeyframe(int time, const KisKeyframe *copySrc)
{
    int srcFrame = (copySrc != 0) ? srcFrame = copySrc->value() : 0;

    quint32 frameId = (quint32)m_d->paintDevice->createFrame((copySrc != 0), srcFrame);
    KisKeyframe *keyframe = new KisKeyframe(this, time, frameId);

    return keyframe;
}

bool KisRasterKeyframeChannel::canDeleteKeyframe(KisKeyframe *key)
{
    // Raster content must have at least one keyframe at all times
    return keys().count() > 1;
}

void KisRasterKeyframeChannel::destroyKeyframe(KisKeyframe *key)
{
    m_d->paintDevice->deleteFrame(key->value());
}

void KisRasterKeyframeChannel::saveKeyframe(KisKeyframe *keyframe, QDomElement keyframeElement) const
{
    keyframeElement.setAttribute("frame", keyframe->value());
}

KisKeyframe * KisRasterKeyframeChannel::loadKeyframe(KoXmlNode keyframeNode)
{
    int time = keyframeNode.toElement().attribute("time").toUInt();
    int frameId = keyframeNode.toElement().attribute("frame").toInt();

    m_d->paintDevice->forceCreateFrame(frameId);

    return new KisKeyframe(this, time, frameId);
}

bool KisRasterKeyframeChannel::hasScalarValue() const
{
    return false;
}

qreal KisRasterKeyframeChannel::minScalarValue() const
{
    return 0;
}

qreal KisRasterKeyframeChannel::maxScalarValue() const
{
    return 0;
}

qreal KisRasterKeyframeChannel::scalarValue(const KisKeyframe *keyframe) const
{
    return 0;
}

void KisRasterKeyframeChannel::setScalarValue(KisKeyframe *keyframe, qreal value)
{
}
