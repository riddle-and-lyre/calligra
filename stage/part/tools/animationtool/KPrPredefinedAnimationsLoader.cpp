/* This file is part of the KDE project
   Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrPredefinedAnimationsLoader.h"

//Stage Headers
#include "KPrCollectionItemModel.h"
#include "animations/KPrAnimationBase.h"
#include "animations/KPrShapeAnimation.h"
#include "animations/KPrAnimationFactory.h"
#include "KPrFactory.h"

//Qt Headers
#include <QListWidget>
#include <QListWidgetItem>
#include <QFont>

//KDE Headers
#include <KLocale>
#include <KIconLoader>
#include <KIcon>
#include <KStandardDirs>
#include <KConfigGroup>
#include <KDebug>

//Calligra Headers
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoXmlNS.h>
#include <KoOdfStylesReader.h>
#include <KoStore.h>

KPrPredefinedAnimationsLoader::KPrPredefinedAnimationsLoader(QObject *parent)
    : QObject(parent)
    , isInitialized(false)
{
    readDefaultAnimations();
}

bool KPrPredefinedAnimationsLoader::populateMainView(QListWidget *view)
{
    loadDefaultAnimations();
    if (m_mainItemsCollection.isEmpty()) {
        return false;
    }
    foreach(QListWidgetItem *item, m_mainItemsCollection) {
        QListWidgetItem *collectionChooserItem = new QListWidgetItem(item->icon(), item->text());
        collectionChooserItem->setData(Qt::UserRole, item->data(Qt::UserRole).toString());
        view->addItem(collectionChooserItem);
    }
    return true;
}

KPrCollectionItemModel *KPrPredefinedAnimationsLoader::modelById(const QString &id)
{
    if(m_modelMap.contains(id)) {
       return m_modelMap[id];
    }
    else {
        kWarning(31000) << "Didn't find a model with id ==" << id;
    }
    return 0;
}

KPrCollectionItemModel *KPrPredefinedAnimationsLoader::subModelById(const QString &id)
{
    if (m_subModelMap.contains(id)) {
        return m_subModelMap[id];
    }
    return 0;
}

void KPrPredefinedAnimationsLoader::loadDefaultAnimations()
{
    if (m_animations.isEmpty() || isInitialized) {
        return;
    }
    isInitialized = true;
    QList<KPrCollectionItem> entranceList;
    QList<KPrCollectionItem> emphasisList;
    QList<KPrCollectionItem> exitList;
    QList<KPrCollectionItem> customList;
    QList<KPrCollectionItem> motion_PathList;
    QList<KPrCollectionItem> ole_ActionList;
    QList<KPrCollectionItem> media_CallList;

    QMap<QString, QList<KPrCollectionItem> > subModelList;
    int row = -1;
    foreach(KPrShapeAnimation *animation, m_animations) {
        row++;
        bool isSubItem = true;
        QString animId = animation->id();
        if (!animation->presetSubType().isEmpty()) {
            if (!subModelList.contains(animId)) {
                QList<KPrCollectionItem> tempList = QList<KPrCollectionItem>();
                subModelList.insert(animId, tempList);
                isSubItem = false;
            }
            KPrCollectionItem subItem;
            QString newId = animId;
            newId.append("-");
            newId.append(animation->presetSubType());
            subItem.id = newId;
            subItem.name = animationName(newId);
            subItem.toolTip = animationName(newId);
            subItem.icon = loadSubTypeIcon(animId, animation->presetSubType());
            subItem.animationContext = m_animationContext.value(row);

            subModelList[animation->id()].append(subItem);
            if (isSubItem) {
                continue;
            }
        }

        KPrCollectionItem temp;
        temp.id = animId;
        temp.name = animationName(animId);
        temp.toolTip = temp.name;
        temp.icon = loadAnimationIcon(temp.name);
        temp.animationContext = m_animationContext.value(row);

        if (animation->presetClass() == KPrShapeAnimation::Entrance) {
            entranceList.append(temp);
        }
        else if (animation->presetClass() == KPrShapeAnimation::Exit) {
            exitList.append(temp);
        }
        else if (animation->presetClass() == KPrShapeAnimation::Emphasis) {
            emphasisList.append(temp);
        }
        else if (animation->presetClass() == KPrShapeAnimation::Custom) {
            customList.append(temp);
        }
        else if (animation->presetClass() == KPrShapeAnimation::Motion_Path) {
            motion_PathList.append(temp);
        }
        else if (animation->presetClass() == KPrShapeAnimation::Ole_Action) {
            ole_ActionList.append(temp);
        }
        else if (animation->presetClass() == KPrShapeAnimation::Media_Call) {
            media_CallList.append(temp);
        }
    }

    KPrCollectionItemModel* model = new KPrCollectionItemModel(this);
    model->setAnimationClassList(entranceList);
    addCollection("entrance", i18n("Entrance"), model);

    if (!exitList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(exitList);
        addCollection("exit", i18n("Exit"), model);
    }
    if (!emphasisList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(emphasisList);
        addCollection("emphasis", i18n("Emphasis"), model);
    }
    if (!customList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(customList);
        addCollection("custom", i18n("Custom"), model);
    }
    if (!motion_PathList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(motion_PathList);
        addCollection("motion_path", i18n("Motion path"), model);
    }
    if (!ole_ActionList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(ole_ActionList);
        addCollection("ole_action", i18n("Ole Action"), model);
    }
    if (!media_CallList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(media_CallList);
        addCollection("media_call", i18n("Media Call"), model);
    }
    if (!subModelList.isEmpty()) {
        QMap<QString, QList<KPrCollectionItem> >::const_iterator i;
        for (i = subModelList.constBegin(); i != subModelList.constEnd(); ++i) {
            model = new KPrCollectionItemModel(this);
            model->setAnimationClassList(i.value());
            addSubCollection(i.key(), model);
        }
    }
}

void KPrPredefinedAnimationsLoader::readDefaultAnimations()
{
    // use the same mechanism for loading the markers that are available
    // per default as when loading the normal markers.
    KoOdfStylesReader stylesReader;
    KoOdfLoadingContext context(stylesReader, 0);
    KoShapeLoadingContext shapeContext(context, 0);
    KoXmlDocument doc;

    const KStandardDirs* dirs = KGlobal::activeComponent().dirs();
    const QString filePath = dirs->findResource("data", "stage/animations/animations.xml");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        QString errorMessage;
        int row = 0;
        if (KoOdfReadStore::loadAndParse(&file, doc, errorMessage, filePath)) {
            const KoXmlElement docElement  = doc.documentElement();
            KoXmlElement animationElement;
            forEachElement(animationElement, docElement) {
                KoXmlElement parAnimation;
                forEachElement(parAnimation, animationElement) {
                    KoXmlElement animation;
                    forEachElement(animation, parAnimation) {
                        KPrShapeAnimation *shapeAnimation = 0;
                        shapeAnimation = loadOdfShapeAnimation(animation, shapeContext);
                        if (shapeAnimation) {
                            m_animations.append(shapeAnimation);
                            m_animationContext.append(animation);
                            row++;
                        }
                    }
                }
            }
        }
        else {
            kWarning(30006) << "reading of" << filePath << "failed:" << errorMessage;
        }
    }
    else {
        kDebug(30006) << "animations.xml not found";
    }
}

QString KPrPredefinedAnimationsLoader::animationName(const QString id) const
{
    QStringList descriptionList = id.split("-");
    if (descriptionList.count() > 2) {
        descriptionList.removeFirst();
        descriptionList.removeFirst();
        return descriptionList.join(QString(" "));
    }
    return QString();
}

QIcon KPrPredefinedAnimationsLoader::loadAnimationIcon(const QString id)
{
    // Animation icon names examples: zoom_animation, spiral_in_animation
    QString name = animationName(id);
    if (!name.isEmpty()) {
        name = name.append("_animation");
        name.replace(" ", "_");
        QString path = KIconLoader::global()->iconPath(name, KIconLoader::Toolbar, true);
        if (!path.isNull()) {
            return KIcon(name);
        }
    }
    return KIcon("unrecognized_animation");
}

QIcon KPrPredefinedAnimationsLoader::loadSubTypeIcon(const QString mainId, const QString subTypeId)
{
    // icon name ex: entrance_zoom_in_animations
    QString subId = subTypeId;
    subId.replace("-", "_");

    QString name = animationName(mainId);
    name.append("_");
    name.append(subId);
    name.append("_animation");

    QIcon icon;
    QString path = KIconLoader::global()->iconPath(name, KIconLoader::Toolbar, true);
    if (!path.isNull()) {
        icon = KIcon(name);
    }
    else {
        // icon name ex: in_animations
        name = subId;
        name.append("_animations");
        path = KIconLoader::global()->iconPath(name, KIconLoader::Toolbar, true);
        if (!path.isNull()) {
            icon = KIcon(name);
        }
        else {
            icon = KIcon("unrecognized_animation");
        }
    }
    return icon;
}

KPrShapeAnimation *KPrPredefinedAnimationsLoader::loadOdfShapeAnimation(const KoXmlElement &element, KoShapeLoadingContext &context, KoShape *animShape)
{
    // load preset and id
    //TODO: motion-path, ole-action, media-call are not supported
    QString presetClass = element.attributeNS(KoXmlNS::presentation, "preset-class");
    QString animationId = element.attributeNS(KoXmlNS::presentation, "preset-id");
    QString presetSubType = element.attributeNS(KoXmlNS::presentation, "preset-sub-type");

    KPrShapeAnimation::Node_Type l_nodeType = KPrShapeAnimation::On_Click;

    KPrShapeAnimation *shapeAnimation = 0;
    // The shape info and create a KPrShapeAnimation. If there is
    KoXmlElement e;
    forEachElement(e, element) {
        if (shapeAnimation == 0) {
            KoShape *shape = animShape;
            KoTextBlockData *textBlockData = 0;
            shapeAnimation = new KPrShapeAnimation(shape, textBlockData);
        }
        KPrAnimationBase *animation(KPrAnimationFactory::createAnimationFromOdf(e, context, shapeAnimation));
        if (shapeAnimation && animation) {
            shapeAnimation->addAnimation(animation);
        }
    }

    if (shapeAnimation) {
        shapeAnimation->setNodeType(l_nodeType);
        if (presetClass == "custom") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Custom);
        }
        if (presetClass == "entrance") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Entrance);
        }
        else if (presetClass == "exit") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Exit);
        }
        else if (presetClass == "emphasis") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Emphasis);
        }
        else if (presetClass == "motion-path") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Motion_Path);
        }
        else if (presetClass == "ole-action") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Ole_Action);
        }
        else if (presetClass == "media-call") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Media_Call);
        }
        else{
            shapeAnimation->setPresetClass(KPrShapeAnimation::None);
        }
        if (!animationId.isEmpty()) {
            shapeAnimation->setId(animationId);
        }
        if (!presetSubType.isEmpty()) {
            shapeAnimation->setPresetSubType(presetSubType);
        }
    }
    return shapeAnimation;
}

bool KPrPredefinedAnimationsLoader::addCollection(const QString &id, const QString &title, KPrCollectionItemModel *model)
{
    if(m_modelMap.contains(id))
        return false;
    m_modelMap.insert(id, model);
    QString iconName = id;
    iconName.append("_animations");
    QIcon icon;
    QString path = KIconLoader::global()->iconPath(iconName, KIconLoader::Toolbar, true);
    if (!path.isNull()) {
        icon = KIcon(iconName);
    }
    else {
        icon = KIcon("unrecognized_animation");
    }

    QListWidgetItem *collectionChooserItem = new QListWidgetItem(icon, title);
    collectionChooserItem->setData(Qt::UserRole, id);
    m_mainItemsCollection.append(collectionChooserItem);
    return true;
}

bool KPrPredefinedAnimationsLoader::addSubCollection(const QString &id, KPrCollectionItemModel *model)
{
    if(m_subModelMap.contains(id))
        return false;
    m_subModelMap.insert(id, model);
    return true;
}
