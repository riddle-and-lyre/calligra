/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
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


#include "KoShapeEndLinesDocker.h"

#include <QGridLayout>
#include <QList>

#include <klocale.h>


#include "../shapecollection/KoCollectionItemModel.h"
#include <KoShapeFactory.h>
#include <KoShapeRegistry.h>
#include <kicon.h>

#include "KoEndLineShapeModel.h"

KoShapeEndLinesDocker::KoShapeEndLinesDocker(QWidget* parent): QDockWidget(parent){
	setWindowTitle(i18n("End lines"));

	    QWidget* mainWidget = new QWidget(this);
	    QGridLayout* mainLayout = new QGridLayout(mainWidget);
	    mainLayout->setMargin(0);
	    setWidget(mainWidget);

	    m_quickView = new QListView (mainWidget);
	    mainLayout->addWidget(m_quickView, 0, 0);
	    m_quickView->setViewMode(QListView::IconMode);
	    m_quickView->setDragDropMode(QListView::DragOnly);
	    m_quickView->setSelectionMode(QListView::SingleSelection);
	    m_quickView->setResizeMode(QListView::Adjust);
	    m_quickView->setFlow(QListView::LeftToRight);
	    m_quickView->setGridSize(QSize(40, 44));
	    m_quickView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	    m_quickView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	    m_quickView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	    m_quickView->setTextElideMode(Qt::ElideNone);
	    m_quickView->setWordWrap(true);

	   QList<KoCollectionItem> quicklist;
	    KoCollectionItem temp;
            temp.id = "triangle";
            temp.name = "triangle";
           // temp.toolTip = shapeTemplate.toolTip;
            temp.icon = KIcon("triangle");
            //temp.properties = shapeTemplate.properties;
	    quicklist.append(temp);


	 /* QList<KoEndLineShape> quicklist;

	    KoEndLineShape kels;
	    kels.id = "triangle";
	    kels.name = "triangle";
	    kels.icon = QIcon(QString("triangle.svg"));
	    quicklist.append(kels);*/

   KoCollectionItemModel* quickModel = new KoCollectionItemModel(this);
    quickModel->setShapeTemplateList(quicklist);
    m_quickView->setModel(quickModel);
 /* KoEndLineShapeModel * quickModel = new KoEndLineShapeModel(this);
    quickModel->setShapeTemplateList(quicklist);
    m_quickView->setModel(quickModel);*/
}

KoShapeEndLinesDocker::~KoShapeEndLinesDocker() {

}
