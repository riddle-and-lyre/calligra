/* This file is part of the KDE project
 * Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#ifndef KPRANIMATIONSDATAMODEL_H
#define KPRANIMATIONSDATAMODEL_H

#include "animations/KPrShapeAnimation.h"

#include <QAbstractTableModel>
#include <QPixmap>
#include <QIcon>

#include "stage_export.h"

/*Data Model for Animations Time Line View
It requires an activePage to be set*/

class KPrPage;
class QImage;
class KoShape;
class KPrAnimationStep;
class KoPADocument;
class KPrView;

class STAGE_TEST_EXPORT KPrAnimationsDataModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    struct AnimationsData {
        QString name;                   //Shape Name
        int order;                      //Animation Order
        QPixmap thumbnail;              //Shape Thumbnail
        QPixmap animationIcon;          //Animation Icon
        QString animationName;          //Animation name
        KPrShapeAnimation::Preset_Class type;
        KPrShapeAnimation::Node_Type triggerEvent;
        qreal startTime;                //Animation start time
        qreal duration;
        KoShape* shape;                 //pointer to target element (shape)

    };

    explicit KPrAnimationsDataModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void setActivePage(KPrPage *activePage);
    void setDocumentView(KPrView *view);
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex indexByShape(KoShape* shape);

signals:
    void dataReinitialized();

public slots:
    /// Triggers an update of the complete model
    void update();

private:
    QImage createThumbnail(KoShape* shape, const QSize &thumbSize) const;
    void setNameAndAnimationIcon(AnimationsData &data, QString id);

    QList<AnimationsData> m_data;
    QList<KPrAnimationStep*> m_steps;
    KPrPage *m_activePage;
    KPrView *m_view;
};

#endif // KPRANIMATIONSDATAMODEL_H
