/*
 *  Copyright (c) 2006 Cyrille Berger <cberger@cberger.net>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _KIS_DYNAMIC_SENSOR_DISTANCE_H_
#define _KIS_DYNAMIC_SENSOR_DISTANCE_H_

#include "kis_vec.h"

#include "kis_dynamic_sensor.h"
//
class KisDynamicSensorDistance : public QObject, public KisDynamicSensor
{
    Q_OBJECT
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    using KisSerializableConfiguration::fromXML;
    using KisSerializableConfiguration::toXML;

    KisDynamicSensorDistance();
    virtual ~KisDynamicSensorDistance() { }
    virtual qreal value(const KisPaintInformation &);
    virtual void reset();
    virtual QWidget *createConfigurationWidget(QWidget *parent, QWidget *);
public Q_SLOTS:
    virtual void setPeriodic(bool periodic);
    virtual void setLength(int length);

    virtual void toXML(QDomDocument &, QDomElement &) const;
    virtual void fromXML(const QDomElement &);
private:
    qreal m_measuredDistance;
    int m_length;
    bool m_periodic;
};

#endif
