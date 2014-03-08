/* This file is part of the KDE project
 * Copyright (C) 2014 Denis Kuplyakov <dener.kup@gmail.com>
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

#ifndef KWNAVIGATIONDOCKER_H
#define KWNAVIGATIONDOCKER_H

// Base classes
#include <QDockWidget>
#include <KoCanvasObserverBase.h>

// Qt

// Calligra
#include <KoDockFactoryBase.h>

// Local
#include <dockers/KWNavigationWidget.h>

class KoCanvasBase;


class KWNavigationDocker : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT
    
public:
    explicit KWNavigationDocker();
    ~KWNavigationDocker();
    /// reimplemented from KoCanvasObserver
    virtual void setCanvas(KoCanvasBase *canvas);
    virtual void unsetCanvas();
    
private:
    bool m_canvasReset;
    
    // The navigation widget
    KWNavigationWidget *m_navigationWidget;
    
    
public slots:
    void ondockLocationChanged(Qt::DockWidgetArea newArea);
};

class KWNavigationDockerFactory : public KoDockFactoryBase
{
public:
    KWNavigationDockerFactory();
    virtual QString id() const;
    virtual QDockWidget* createDockWidget();
    DockPosition defaultDockPosition() const {
        return DockMinimized;
    }
};

#endif //KWNAVIGATIONDOCKER_H
