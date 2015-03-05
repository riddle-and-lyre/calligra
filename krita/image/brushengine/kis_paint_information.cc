/*
 *  Copyright (c) 2007,2010 Cyrille Berger <cberger@cberger.net>
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

#include "kis_paint_information.h"

#include <QDomElement>
#include <QScopedPointer>

#include "kis_paintop.h"
#include "kis_distance_information.h"
#include "kis_algebra_2d.h"

struct KisPaintInformation::Private {
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    Private() : currentDistanceInfo(0) {}
    ~Private()
    {
        KIS_ASSERT_RECOVER_NOOP(!currentDistanceInfo);
    }
    Private(const Private &rhs)
    {
        copy(rhs);
    }
    Private &operator=(const Private &rhs)
    {
        copy(rhs);
        return *this;
    }

    void copy(const Private &rhs)
    {
        pos = rhs.pos;
        pressure = rhs.pressure;
        xTilt = rhs.xTilt;
        yTilt = rhs.yTilt;
        rotation = rhs.rotation;
        tangentialPressure = rhs.tangentialPressure;
        perspective = rhs.perspective;
        time = rhs.time;
        isHoveringMode = rhs.isHoveringMode;
        currentDistanceInfo = rhs.currentDistanceInfo;

        if (rhs.drawingAngleOverride) {
            drawingAngleOverride.reset(new qreal(*rhs.drawingAngleOverride));
        }
    }

    QPointF pos;
    qreal pressure;
    qreal xTilt;
    qreal yTilt;
    qreal rotation;
    qreal tangentialPressure;
    qreal perspective;
    qreal time;
    bool isHoveringMode;

    QScopedPointer<qreal> drawingAngleOverride;
    KisDistanceInformation *currentDistanceInfo;

    void registerDistanceInfo(KisDistanceInformation *di)
    {
        currentDistanceInfo = di;
    }

    void unregisterDistanceInfo()
    {
        currentDistanceInfo = 0;
    }
};

KisPaintInformation::DistanceInformationRegistrar::
DistanceInformationRegistrar(KisPaintInformation *_p, KisDistanceInformation *distanceInfo)
    : p(_p)
{
    p->d->registerDistanceInfo(distanceInfo);
}

KisPaintInformation::DistanceInformationRegistrar::
~DistanceInformationRegistrar()
{
    p->d->unregisterDistanceInfo();
}

KisPaintInformation::KisPaintInformation(const QPointF &pos_,
        qreal pressure_,
        qreal xTilt_, qreal yTilt_,
        qreal rotation_,
        qreal tangentialPressure_,
        qreal perspective_,
        qreal time)
    : d(new Private)
{
    d->pos = pos_;
    d->pressure = pressure_;
    d->xTilt = xTilt_;
    d->yTilt = yTilt_;
    d->rotation = rotation_;
    d->tangentialPressure = tangentialPressure_;
    d->perspective = perspective_;
    d->time = time;
    d->isHoveringMode = false;
}

KisPaintInformation::KisPaintInformation(const KisPaintInformation &rhs) : d(new Private(*rhs.d))
{
}

void KisPaintInformation::operator=(const KisPaintInformation &rhs)
{
    *d = *rhs.d;
}

KisPaintInformation::~KisPaintInformation()
{
    delete d;
}

bool KisPaintInformation::isHoveringMode() const
{
    return d->isHoveringMode;
}

KisPaintInformation
KisPaintInformation::createHoveringModeInfo(const QPointF &pos,
        qreal pressure,
        qreal xTilt, qreal yTilt,
        qreal rotation,
        qreal tangentialPressure,
        qreal perspective)
{
    KisPaintInformation info(pos,
                             pressure,
                             xTilt, yTilt,
                             rotation,
                             tangentialPressure,
                             perspective, 0);
    info.d->isHoveringMode = true;
    return info;
}

void KisPaintInformation::toXML(QDomDocument &, QDomElement &e) const
{
    // hovering mode infos are not supposed to be saved
    KIS_ASSERT_RECOVER_NOOP(!d->isHoveringMode);

    e.setAttribute("pointX", QString::number(pos().x(), 'g', 15));
    e.setAttribute("pointY", QString::number(pos().y(), 'g', 15));
    e.setAttribute("pressure", QString::number(pressure(), 'g', 15));
    e.setAttribute("xTilt", QString::number(xTilt(), 'g', 15));
    e.setAttribute("yTilt", QString::number(yTilt(), 'g', 15));
    e.setAttribute("rotation", QString::number(rotation(), 'g', 15));
    e.setAttribute("tangentialPressure", QString::number(tangentialPressure(), 'g', 15));
    e.setAttribute("perspective", QString::number(perspective(), 'g', 15));
    e.setAttribute("time", d->time);
}

KisPaintInformation KisPaintInformation::fromXML(const QDomElement &e)
{
    qreal pointX = qreal(e.attribute("pointX", "0.0").toDouble());
    qreal pointY = qreal(e.attribute("pointY", "0.0").toDouble());
    qreal pressure = qreal(e.attribute("pressure", "0.0").toDouble());
    qreal rotation = qreal(e.attribute("rotation", "0.0").toDouble());
    qreal tangentialPressure = qreal(e.attribute("tangentialPressure", "0.0").toDouble());
    qreal perspective = qreal(e.attribute("perspective", "0.0").toDouble());
    qreal xTilt = qreal(e.attribute("xTilt", "0.0").toDouble());
    qreal yTilt = qreal(e.attribute("yTilt", "0.0").toDouble());
    qreal time = e.attribute("time", "0").toDouble();

    return KisPaintInformation(QPointF(pointX, pointY), pressure, xTilt, yTilt,
                               rotation, tangentialPressure, perspective, time);
}

const QPointF &KisPaintInformation::pos() const
{
    return d->pos;
}

void KisPaintInformation::setPos(const QPointF &p)
{
    d->pos = p;
}

qreal KisPaintInformation::pressure() const
{
    return d->pressure;
}

void KisPaintInformation::setPressure(qreal p)
{
    d->pressure = p;
}

qreal KisPaintInformation::xTilt() const
{
    return d->xTilt;
}

qreal KisPaintInformation::yTilt() const
{
    return d->yTilt;
}

void KisPaintInformation::overrideDrawingAngle(qreal angle)
{
    d->drawingAngleOverride.reset(new qreal(angle));
}

qreal KisPaintInformation::drawingAngleSafe(const KisDistanceInformation &distance) const
{
    if (d->drawingAngleOverride) {
        return *d->drawingAngleOverride;
    }

    QVector2D diff(pos() - distance.lastPosition());
    return atan2(diff.y(), diff.x());
}

KisPaintInformation::DistanceInformationRegistrar
KisPaintInformation::registerDistanceInformation(KisDistanceInformation *distance)
{
    return DistanceInformationRegistrar(this, distance);
}

qreal KisPaintInformation::drawingAngle() const
{
    if (d->drawingAngleOverride) {
        return *d->drawingAngleOverride;
    }

    if (!d->currentDistanceInfo || !d->currentDistanceInfo->hasLastDabInformation()) {
        qWarning() << "KisPaintInformation::drawingAngle()" << "Cannot access Distance Info last dab data";
        return 0.0;
    }

    QVector2D diff(pos() - d->currentDistanceInfo->lastPosition());
    return atan2(diff.y(), diff.x());
}

QPointF KisPaintInformation::drawingDirectionVector() const
{
    if (d->drawingAngleOverride) {
        qreal angle = *d->drawingAngleOverride;
        return QPointF(cos(angle), sin(angle));
    }

    if (!d->currentDistanceInfo || !d->currentDistanceInfo->hasLastDabInformation()) {
        qWarning() << "KisPaintInformation::drawingDirectionVector()" << "Cannot access Distance Info last dab data";
        return QPointF(1.0, 0.0);
    }

    QPointF diff(pos() - d->currentDistanceInfo->lastPosition());
    return KisAlgebra2D::normalize(diff);
}

qreal KisPaintInformation::drawingDistance() const
{
    if (!d->currentDistanceInfo || !d->currentDistanceInfo->hasLastDabInformation()) {
        qWarning() << "KisPaintInformation::drawingDistance()" << "Cannot access Distance Info last dab data";
        return 1.0;
    }

    QVector2D diff(pos() - d->currentDistanceInfo->lastPosition());
    return diff.length();
}

qreal KisPaintInformation::drawingSpeed() const
{
    if (!d->currentDistanceInfo || !d->currentDistanceInfo->hasLastDabInformation()) {
        qWarning() << "KisPaintInformation::drawingSpeed()" << "Cannot access Distance Info last dab data";
        return 0.5;
    }

    qreal timeDiff = currentTime() - d->currentDistanceInfo->lastTime();

    if (timeDiff <= 0) {
        return 0.5;
    }

    QVector2D diff(pos() - d->currentDistanceInfo->lastPosition());
    return diff.length() / timeDiff;
}

qreal KisPaintInformation::rotation() const
{
    return d->rotation;
}

qreal KisPaintInformation::tangentialPressure() const
{
    return d->tangentialPressure;
}

qreal KisPaintInformation::perspective() const
{
    return d->perspective;
}

qreal KisPaintInformation::currentTime() const
{
    return d->time;
}

QDebug operator<<(QDebug dbg, const KisPaintInformation &info)
{
#ifdef NDEBUG
    Q_UNUSED(info);
#else
    dbg.nospace() << "Position: " << info.pos();
    dbg.nospace() << ", Pressure: " << info.pressure();
    dbg.nospace() << ", X Tilt: " << info.xTilt();
    dbg.nospace() << ", Y Tilt: " << info.yTilt();
    dbg.nospace() << ", Rotation: " << info.rotation();
    dbg.nospace() << ", Tangential Pressure: " << info.tangentialPressure();
    dbg.nospace() << ", Perspective: " << info.perspective();
    dbg.nospace() << ", Drawing Angle: " << info.drawingAngle();
    dbg.nospace() << ", Drawing Speed: " << info.drawingSpeed();
    dbg.nospace() << ", Drawing Distance: " << info.drawingDistance();
    dbg.nospace() << ", Time: " << info.currentTime();
#endif
    return dbg.space();
}

KisPaintInformation KisPaintInformation::mixOnlyPosition(qreal t, const KisPaintInformation &mixedPi, const KisPaintInformation &basePi)
{
    QPointF pt = (1 - t) * mixedPi.pos() + t * basePi.pos();
    KisPaintInformation result(pt,
                               basePi.pressure(),
                               basePi.xTilt(),
                               basePi.yTilt(),
                               basePi.rotation(),
                               basePi.tangentialPressure(),
                               basePi.perspective(),
                               basePi.currentTime());

    return result;
}

KisPaintInformation KisPaintInformation::mix(qreal t, const KisPaintInformation &pi1, const KisPaintInformation &pi2)
{
    QPointF pt = (1 - t) * pi1.pos() + t * pi2.pos();
    return mix(pt, t, pi1, pi2);
}

KisPaintInformation KisPaintInformation::mix(const QPointF &p, qreal t, const KisPaintInformation &pi1, const KisPaintInformation &pi2)
{
    qreal pressure = (1 - t) * pi1.pressure() + t * pi2.pressure();
    qreal xTilt = (1 - t) * pi1.xTilt() + t * pi2.xTilt();
    qreal yTilt = (1 - t) * pi1.yTilt() + t * pi2.yTilt();

    qreal rotation = pi1.rotation();

    if (pi1.rotation() != pi2.rotation()) {
        qreal a1 = kisDegreesToRadians(pi1.rotation());
        qreal a2 = kisDegreesToRadians(pi2.rotation());
        qreal distance = shortestAngularDistance(a2, a1);

        rotation = kisRadiansToDegrees(incrementInDirection(a1, t * distance, a2));
    }

    qreal tangentialPressure = (1 - t) * pi1.tangentialPressure() + t * pi2.tangentialPressure();
    qreal perspective = (1 - t) * pi1.perspective() + t * pi2.perspective();
    qreal time = (1 - t) * pi1.currentTime() + t * pi2.currentTime();

    KisPaintInformation result(p, pressure, xTilt, yTilt, rotation, tangentialPressure, perspective, time);
    KIS_ASSERT_RECOVER_NOOP(pi1.isHoveringMode() == pi2.isHoveringMode());
    result.d->isHoveringMode = pi1.isHoveringMode();

    return result;
}

qreal KisPaintInformation::tiltDirection(const KisPaintInformation &info, bool normalize)
{
    qreal xTilt = info.xTilt();
    qreal yTilt = info.yTilt();
    // radians -PI, PI
    qreal tiltDirection = atan2(-xTilt, yTilt);
    // if normalize is true map to 0.0..1.0
    return normalize ? (tiltDirection / (2 * M_PI) + 0.5) : tiltDirection;
}

qreal KisPaintInformation::tiltElevation(const KisPaintInformation &info, qreal maxTiltX, qreal maxTiltY, bool normalize)
{
    qreal xTilt = qBound(qreal(-1.0), info.xTilt() / maxTiltX, qreal(1.0));
    qreal yTilt = qBound(qreal(-1.0), info.yTilt() / maxTiltY, qreal(1.0));

    qreal e;
    if (fabs(xTilt) > fabs(yTilt)) {
        e = sqrt(qreal(1.0) + yTilt * yTilt);
    } else {
        e = sqrt(qreal(1.0) + xTilt * xTilt);
    }

    qreal cosAlpha    = sqrt(xTilt * xTilt + yTilt * yTilt) / e;
    qreal tiltElevation = acos(cosAlpha); // in radians in [0, 0.5 * PI]

    // mapping to 0.0..1.0 if normalize is true
    return normalize ? (tiltElevation / (M_PI * qreal(0.5))) : tiltElevation;
}

