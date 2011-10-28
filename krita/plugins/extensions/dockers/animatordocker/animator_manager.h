/*
 *  
 *  Copyright (C) 2011 Torio Mlshi <mlshi@lavabit.com>
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
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef ANIMATOR_MANAGER_H
#define ANIMATOR_MANAGER_H

#include <QObject>

#include <KoCanvasObserverBase.h>

#include <kis_image.h>
#include <kis_node_manager.h>

class AnimatorSwitcher;
class AnimatorUpdater;
class AnimatorLoader;
class AnimatorPlayer;
class AnimatorExporter;

#include "simple_frame_layer.h"
#include "framed_animated_layer.h"
#include "animator_meta_info.h"

class AnimatorManager : public QObject, KoCanvasObserverBase {
    Q_OBJECT
    
public:
    AnimatorManager(KisImage* image);
    virtual ~AnimatorManager();
    
public:
    virtual void setCanvas(KoCanvasBase* canvas);
    virtual void unsetCanvas();
    
    virtual bool ready();
    
public:
    virtual AnimatorSwitcher* getSwitcher();
    virtual AnimatorUpdater* getUpdater();
    virtual AnimatorLoader* getLoader();
    virtual AnimatorPlayer* getPlayer();
    virtual AnimatorExporter* getExporter();
    
    virtual KisImage* image();
    
public:
    virtual void layerFramesNumberChange(AnimatedLayer* layer, int number);
    
signals:
    void layerFramesNumberChanged(AnimatedLayer* layer, int number);
    void framesNumberChanged(int number);
    
protected slots:
    virtual void framesNumberCheck(AnimatedLayer* layer, int number);
    
public:
    virtual void setKraMetaInfo(AnimatorMetaInfo* info);
    virtual void setKraMetaInfo();
    virtual AnimatorMetaInfo* kraMetaInfo();
    virtual AnimatorMetaInfo* metaInfo();
    
public:
    virtual void initLayers();
    
public:
    virtual void createNormalLayer();
    virtual void removeLayer();
    virtual void renameLayer(KisNode* layer, const QString& name);
    virtual void renameLayer(const QString& name);
    
    virtual AnimatedLayer* getAnimatedLayerByChild(KisNode* child);
    virtual KisNode* activeLayer();
    
    virtual void createFrame(AnimatedLayer* layer, const QString& ftype);
    virtual void createFrame(AnimatedLayer* layer, const QString& ftype, bool iskey);
    virtual void createFrame(const QString& ftype);
    virtual void createFrame(const QString& ftype, bool iskey);
    virtual void interpolate();
    virtual void removeFrame();
    
    virtual void moveFrame(int relPos);
    
public:
    virtual void setFrameContent(SimpleFrameLayer* frame, KisNode* content);
    virtual void putNodeAt(KisNodeSP node, KisNodeSP parent, int index);
    virtual void removeFrame(KisNode* frame);
    
    virtual void insertLayer(AnimatedLayer* layer, KisNodeSP parent, int index);
    virtual void removeLayer(KisNode* layer);
    
public:
    virtual int framesNumber() const;
    
public:
    virtual void activate(int frameNumber, KisNode* node);
    virtual void activateKeyFrame(AnimatedLayer* alayer, int frameNumber);
    
public:
    virtual QList<AnimatedLayer*> layers();

protected:
    virtual void calculateFramesNumber();
    
protected:
    virtual void layerAdded(AnimatedLayer* layer);
    virtual void layerRemoved(AnimatedLayer* layer);
    
private:
    KisImage* m_image;
    KisNodeManager* m_nodeManager;
    
    AnimatorSwitcher* m_switcher;
    AnimatorUpdater* m_updater;
    AnimatorLoader* m_loader;
    AnimatorPlayer* m_player;
    AnimatorExporter* m_exporter;
    AnimatorMetaInfo* m_info;
    
    
    QList<AnimatedLayer*> m_layers;
    AnimatedLayer* m_maxFrameLayer;
    
    int m_framesNumber;
};

#endif

