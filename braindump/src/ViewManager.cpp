/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "ViewManager.h"

#include <KoShapeManager.h>

#include "Canvas.h"
#include "Section.h"
#include "View.h"
#include "SectionContainer.h"
#include <KoShapeLayer.h>
#include "RootSection.h"
#include "Utils.h"

ViewManager::ViewManager(RootSection* _rootSection) : m_lastViewInFocus(0), m_rootSection(_rootSection)
{
}

void ViewManager::addShape(KoShape* shape)
{
  if(!shape)
    return;
  Section * page( sectionByShape( shape ) );
  if( not page) {
    return;
  }
  foreach( View *view, m_views )
  {
    if ( page == view->activeSection() ) {
      view->canvas()->shapeManager()->add( shape );
    }
  }
}

void ViewManager::removeShape(KoShape* shape)
{
  if(!shape)
    return;
  Section * page = sectionByShape( shape );
  if( not page) {
    return;
  }
  foreach( View *view, m_views )
  {
    if ( page == view->activeSection() ) {
      view->canvas()->shapeManager()->remove( shape );
    }
  }
}

QMap<QString, KoDataCenter *> ViewManager::dataCenterMap() const
{
  Q_ASSERT(m_lastViewInFocus);
  if(m_lastViewInFocus->activeSection())
  {
    return m_lastViewInFocus->activeSection()->sectionContainer()->dataCenterMap();
  }
  return QMap<QString, KoDataCenter *>();
}

Section* ViewManager::sectionByShape( KoShape * shape ) const
{
  KoShape * parent = shape;
  while( ( parent = parent->parent() ) )
  {
    KoShapeLayer* layer = dynamic_cast<KoShapeLayer*>( parent );
    Section* section = Utils::sectionForLayer(layer, m_rootSection);
    if(section) return section;
  }
  return 0;
}

void ViewManager::addView(View* view)
{
  Q_ASSERT(not m_views.contains(view));
  m_views.append(view);
}
void ViewManager::removeView(View* view)
{
  Q_ASSERT(m_views.contains(view));
  m_views.removeAll(view);
}

void ViewManager::viewHasFocus(View* view)
{
  m_lastViewInFocus = view;
}
