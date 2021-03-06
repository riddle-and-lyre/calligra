/*
 * This file is part of KPlato
 *
 * Copyright (c) 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTINGDEBUG_H
#define SCRIPTINGDEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &PLANSCRIPTING_LOG();

#define debugPlanScripting qCDebug(PLANSCRIPTING_LOG)
#define warnPlanScripting qCWarning(PLANSCRIPTING_LOG)
#define errorPlanScripting qCCritical(PLANSCRIPTING_LOG)

#endif
