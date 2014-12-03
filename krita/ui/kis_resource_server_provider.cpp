/*
 *  kis_resourceserver.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <elter@kde.org>
 *  Copyright (c) 2003 Patrick Julien <freak@codepimps.org>
 *  Copyright (c) 2005 Sven Langkamp <sven.langkamp@gmail.com>
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

#include "kis_resource_server_provider.h"

#include <QDir>
#include <QApplication>
#include <QDebug>

#include <kglobal.h>
#include <kstandarddirs.h>
#include <kcomponentdata.h>

#include <KoResource.h>
#include <KoResourceServer.h>
#include <KoResourceServerProvider.h>
#include <KoResourceServerAdapter.h>

#include <kis_debug.h>
#include <KoPattern.h>
#include <kis_paintop_preset.h>
#include <kis_workspace_resource.h>
#include <kis_psd_layer_style_resource.h>

#include <kis_brush_server.h>

typedef KoResourceServer<KisPaintOpPreset, SharedPointerStroragePolicy<KisPaintOpPresetSP> > KisPaintOpPresetResourceServer;
typedef KoResourceServerAdapter<KisPaintOpPreset, SharedPointerStroragePolicy<KisPaintOpPresetSP> > KisPaintOpPresetResourceServerAdapter;


KisResourceServerProvider::KisResourceServerProvider()
{

    KisBrushServer *brushServer = KisBrushServer::instance();

    KGlobal::mainComponent().dirs()->addResourceType("kis_paintoppresets", "data", "krita/paintoppresets/");
    KGlobal::mainComponent().dirs()->addResourceDir("kis_paintoppresets", QDir::homePath() + QString("/.create/paintoppresets/krita"));

    KGlobal::mainComponent().dirs()->addResourceType("kis_workspaces", "data", "krita/workspaces/");

    KGlobal::mainComponent().dirs()->addResourceType("psd_layer_styles", "data", "krita/asl");
    
    m_paintOpPresetServer = new KisPaintOpPresetResourceServer("kis_paintoppresets", "*.kpp");
    if (!QFileInfo(m_paintOpPresetServer->saveLocation()).exists()) {
        QDir().mkpath(m_paintOpPresetServer->saveLocation());
    }
    m_paintOpPresetThread = new KoResourceLoaderThread(m_paintOpPresetServer);
    m_paintOpPresetThread->start();
    if (!qApp->applicationName().toLower().contains("krita")) {
        m_paintOpPresetThread->barrier();
    }

    m_workspaceServer = new KoResourceServer<KisWorkspaceResource>("kis_workspaces", "*.kws");
    if (!QFileInfo(m_workspaceServer->saveLocation()).exists()) {
        QDir().mkpath(m_workspaceServer->saveLocation());
    }
    m_workspaceThread = new KoResourceLoaderThread(m_workspaceServer);
    m_workspaceThread->start();
    if (!qApp->applicationName().toLower().contains("krita")) {
        m_workspaceThread->barrier();
    }

    m_layerStyleServer = new KoResourceServer<KisPSDLayerStyleResource>("psd_layer_styles", "*.asl");
    if (!QFileInfo(m_layerStyleServer->saveLocation()).exists()) {
        QDir().mkpath(m_layerStyleServer->saveLocation());
    }
    m_layerStyleThread = new KoResourceLoaderThread(m_layerStyleServer);
    m_layerStyleThread->start();
    if (!qApp->applicationName().toLower().contains("krita")) {
        m_layerStyleThread->barrier();
    }

    connect(this, SIGNAL(notifyBrushBlacklistCleanup()),
            brushServer, SLOT(slotRemoveBlacklistedResources()));

}

KisResourceServerProvider::~KisResourceServerProvider()
{
    delete m_paintOpPresetThread;
    delete m_workspaceThread;
    delete m_layerStyleThread;

    delete m_paintOpPresetServer;
    delete m_workspaceServer;
    delete m_layerStyleServer;
}

KisResourceServerProvider* KisResourceServerProvider::instance()
{
    K_GLOBAL_STATIC(KisResourceServerProvider, s_instance);
    return s_instance;
}


KisPaintOpPresetResourceServer* KisResourceServerProvider::paintOpPresetServer()
{
    m_paintOpPresetThread->barrier();
    return m_paintOpPresetServer;
}

KoResourceServer< KisWorkspaceResource >* KisResourceServerProvider::workspaceServer()
{
    m_workspaceThread->barrier();
    return m_workspaceServer;
}

KoResourceServer<KisPSDLayerStyleResource> *KisResourceServerProvider::layerStyleServer()
{
    m_layerStyleThread->barrier();
    return m_layerStyleServer;
}

void KisResourceServerProvider::brushBlacklistCleanup()
{
    emit notifyBrushBlacklistCleanup();
}


#include "kis_resource_server_provider.moc"


