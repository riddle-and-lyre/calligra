/*
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
 *  Copyright (c) 2005 C. Boemann <cbo@boemann.dk>
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

#include <klocale.h>
#include "kis_node.h"
#include "commands/kis_node_opacity_command.h"

KisNodeOpacityCommand::KisNodeOpacityCommand(KisNodeSP node, quint8 oldOpacity, quint8 newOpacity) :
    KisNodeCommand(kundo2_i18n("Opacity Change"), node)
{
    m_oldOpacity = oldOpacity;
    m_newOpacity = newOpacity;
}

void KisNodeOpacityCommand::redo()
{
    m_node->setOpacity(m_newOpacity);
    m_node->setDirty();
}

void KisNodeOpacityCommand::undo()
{
    m_node->setOpacity(m_oldOpacity);
    m_node->setDirty();
}
