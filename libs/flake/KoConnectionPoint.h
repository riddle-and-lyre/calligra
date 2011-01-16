/* This file is part of the KDE project
 * 
 * Copyright (C) 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KOCONNECTIONPOINT_H
#define KOCONNECTIONPOINT_H

#include "KoFlake.h"
#include "flake_export.h"
#include <QtCore/QPointF>

/// Data of a single connection point
struct FLAKE_EXPORT KoConnectionPoint
{
    /// Default connection point ids
    enum PointId {
        TopConnectionPoint = 0,         ///< default connection point on the middle of the top edge
        RightConnectionPoint = 1,       ///< default connection point on the middle of the right edge
        BottomConnectionPoint = 2,      ///< default connection point on the middle of the bottom edge
        LeftConnectionPoint = 3,        ///< default connection point on the middle of the left edge
        FirstCustomConnectionPoint = 4  ///< first custom connection point id
    };

    /// Escape directions for connections connected to connection points
    enum EscapeDirection {
        AllDirections,        ///< connection can escape in all directions
        HorizontalDirections, ///< connection can escape left and right
        VerticalDirections,   ///< connection can escape top and down
        LeftDirection,        ///< connection can escape left
        RightDirection,       ///< connection can escape right
        UpDirection,          ///< connection can escape up
        DownDirection         ///< connection can escape down
    };

    /// Default constructor
    KoConnectionPoint();

    /// Creates connection point with specified position
    KoConnectionPoint(const QPointF &position);

    /// Creates connection point with specified position and escape direction
    KoConnectionPoint(const QPointF &position, EscapeDirection escapeDirection);

    /// Returns default connection point with specified id
    static KoConnectionPoint defaultConnectionPoint(KoConnectionPoint::PointId connectionPointId);

    QPointF position; ///< the position of the connection point in shape coordinates
    EscapeDirection escapeDirection; ///< the escape direction for connection attached to that connection point
};

/// Connection point container storing connection point data along their id
typedef QMap<int, KoConnectionPoint> KoConnectionPoints;

#endif // KOCONNECTIONPOINT_H
