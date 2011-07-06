/*
 *  Copyright (c) 2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#ifndef KIS_MIMEDATA_H
#define KIS_MIMEDATA_H

#include <QMimeData>

#include <kis_types.h>
/**
 * KisMimeData implements delayed retrieval of node for d&d and copy/paste
 */
class KisMimeData : public QMimeData
{
    Q_OBJECT
public:
    KisMimeData();

    /**
     * Set the node this KisMimeData instance contains. There can be only one node.
     */
    void setNode(KisNodeSP node);

    /// return the node set on this mimedata object -- for internal use
    KisNodeSP node() const;

    /**
     * KisMimeData provides the following formats if a node has been set:
     * <ul>
     * <li>application/x-krita-node: requests a whole serialized node. For d&d between instances of Krita.
     * <li>application/x-qt-image: fallback for other applications, returns a QImage of the
     * current node's paintdevice
     * <li>application/x-krita-node-pointer: for internal d&d
     * <li>application/zip: allows drop targets that can handle zip files to open the data
     * <li>image/openraster: for interchange with other apps that support openraster
     * </ul>
     */
    QStringList formats () const;

protected:

    QVariant retrieveData(const QString &mimetype, QVariant::Type preferredType) const;

private:

    KisNodeSP m_node;

};

#endif // KIS_MIMEDATA_H
