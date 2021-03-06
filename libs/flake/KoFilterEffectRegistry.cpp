/* This file is part of the KDE project
 * Copyright (c) 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KoFilterEffectRegistry.h"

#include <QGlobalStatic>

#include "KoFilterEffect.h"
#include <KoPluginLoader.h>

#include <FlakeDebug.h>
#include <KoXmlReader.h>

Q_GLOBAL_STATIC(KoFilterEffectRegistry, s_instance)

KoFilterEffectRegistry::KoFilterEffectRegistry()
  : d(0)
{
}

void KoFilterEffectRegistry::init()
{
    KoPluginLoader::PluginsConfig config;
    config.whiteList = "FilterEffectPlugins";
    config.blacklist = "FilterEffectPluginsDisabled";
    KoPluginLoader::load(QStringLiteral("calligra/shapefiltereffects"), config);
}


KoFilterEffectRegistry::~KoFilterEffectRegistry()
{
    qDeleteAll(doubleEntries());
    qDeleteAll(values());
}

KoFilterEffectRegistry* KoFilterEffectRegistry::instance()
{
    if (!s_instance.exists()) {
        s_instance->init();
    }
    return s_instance;
}

KoFilterEffect * KoFilterEffectRegistry::createFilterEffectFromXml(const KoXmlElement & element, const KoFilterEffectLoadingContext &context)
{
    KoFilterEffectFactoryBase * factory = get(element.tagName());
    if (!factory)
        return 0;

    KoFilterEffect * filterEffect = factory->createFilterEffect();
    if (filterEffect->load(element, context))
        return filterEffect;

    delete filterEffect;
    return 0;
}
