/* This file is part of the KDE project
  Copyright (C) 2012 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version..

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#ifndef PLANWORK_DEBUGAREA
#define PLANWORK_DEBUGAREA

#include "kplatowork_export.h"

#include <QDebug>
#include <QLoggingCategory>

extern const KPLATOWORK_EXPORT QLoggingCategory &PLANWORK_LOG();

#define debugPlanWork qCDebug(PLANWORK_LOG)<<QString("%1:").arg(__func__)
#define warnPlanWork qCWarning(PLANWORK_LOG)
#define errorPlanWork qCCritical(PLANWORK_LOG)

#endif
