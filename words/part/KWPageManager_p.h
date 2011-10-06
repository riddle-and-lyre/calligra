/* This file is part of the Calligra project
 * Copyright (C) 2008, 2010 Thomas Zander <zander@kde.org>
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
#ifndef KWPageManagerPrivate_H
#define KWPageManagerPrivate_H

#include "KWPageStyle.h"
#include "KWPage.h"

#include <QHash>
#include <QMap>

class KWPageManagerPrivate
{
public:
    struct Page
    {
        Page()
            : pageSide(KWPage::Right),
            orientation(KoPageFormat::Portrait),
            textDirection(KoText::InheritDirection),
            pageNumber(1),
            autoGenerated(0)
        {
        }
        KWPageStyle style;
        KWPage::PageSide pageSide;
        KoPageFormat::Orientation orientation;
        KoText::Direction textDirection;
        uint pageNumber : 20; // set by the append-page and overwritten by the text-layout
        uint autoGenerated : 1; // bool to signify words having generated it
        uint padding : 11;
    };

    KWPageManagerPrivate();

    void setPageOffset(int pageNum, qreal offset);
    qreal pageOffset(int pageNum) const;

    void setVisiblePageNumber(int pageId, int newPageNumber);

    void insertPage(const Page &page);

    // use a sorted map to find page the identifier for page objects based on the page number.
    QMap<int, int> pageNumbers; // page number to pageId

    // use a fast access hash to store the page objects, sorted by their identifier
    QHash<int, Page> pages; // pageId to page struct

    QMap<int, int> visiblePageNumbers;

    int lastId; // pageIds are distributed serially,

    bool preferPageSpread;

    QHash<QString, KWPageStyle> pageStyles;
    QHash<QString, QString> pageStyleNames; // map display-name to name
    KoInsets padding;
    KWPageStyle defaultPageStyle;
    QHash<int, qreal> pageOffsets;
    QHash<int, qreal> pageHeights;
};

#endif
