/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
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
 *
 */

#ifndef CTCANVASCONTROLLER_H
#define CTCANVASCONTROLLER_H

#include <libs/flake/KoCanvasController.h>
#include <QSize>
#include <QPoint>

class KoCanvasBase;
class CTCanvasController : public KoCanvasController
{
public:
    explicit CTCanvasController(KActionCollection* actionCollection);
    ~CTCanvasController();
    virtual void setVastScrolling(qreal factor);
    virtual void setZoomWithWheel(bool zoom);
    virtual void updateDocumentSize(const QSize& sz, bool recalculateCenter);
    virtual void setScrollBarValue(const QPoint& value);
    virtual QPoint scrollBarValue() const;
    virtual void pan(const QPoint& distance);
    virtual QPointF preferredCenter() const;
    virtual void setPreferredCenter(const QPointF& viewPoint);
    virtual void recenterPreferred();
    virtual void zoomTo(const QRect& rect);
    virtual void zoomBy(const QPoint& center, qreal zoom);
    virtual void zoomOut(const QPoint& center);
    virtual void zoomIn(const QPoint& center);
    virtual void ensureVisible(KoShape* shape);
    virtual void ensureVisible(const QRectF& rect, bool smooth);
    virtual int canvasOffsetY() const;
    virtual int canvasOffsetX() const;
    virtual int visibleWidth() const;
    virtual int visibleHeight() const;
    virtual KoCanvasBase* canvas() const;
    virtual void setCanvas(KoCanvasBase* canvas);
    virtual void setDrawShadow(bool drawShadow);
    virtual QSize viewportSize() const;
    virtual void scrollContentsBy(int dx, int dy);

private:
    KoCanvasBase *m_canvas;
};

#endif // CTCANVASCONTROLLER_H


