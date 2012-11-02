/* This file is part of the KDE project
   Copyright (C) 2009 Thorsten Zachmann <zachmann@kde.org>

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

#include "KoPAConfigureDialog.h"

#include "KoPAView.h"
#include <KoConfigDocumentPage.h>
#include <KoConfigGridPage.h>
#include <KoConfigMiscPage.h>
#include <KoConfigAuthorPage.h>
#include <KoPACanvasBase.h>
#include <KoShapeController.h>
#include <KoDocument.h>

#include <KoIcon.h>

#include <klocale.h>

KoPAConfigureDialog::KoPAConfigureDialog(KoPAView* parent)
: KPageDialog(parent)
{
    setFaceType(List);
    setCaption(i18n("Configure"));
    setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel | KDialog::Default);
    setDefaultButton(KDialog::Ok);

    m_miscPage = new KoConfigMiscPage(parent->koDocument()->componentData().config(), parent->kopaCanvas()->shapeController()->resourceManager(), parent->kopaCanvas()->resourceManager());
    KPageWidgetItem *item = addPage( m_miscPage, i18n( "Misc" ) );
    item->setHeader( i18n( "Misc" ) );
    item->setIcon(koIcon("preferences-other"));

    m_gridPage = new KoConfigGridPage(parent->koDocument());
    item = addPage(m_gridPage, i18n("Grid"));
    item->setHeader(i18n("Grid"));
    item->setIcon(koIcon("grid"));

    connect(m_miscPage, SIGNAL(unitChanged(KoUnit)), m_gridPage, SLOT(slotUnitChanged(KoUnit)));

    m_docPage = new KoConfigDocumentPage( parent->koDocument() );
    item = addPage( m_docPage, i18nc( "@title:tab Document settings page", "Document" ) );
    item->setHeader( i18n( "Document Settings" ) );
    item->setIcon(koIcon("document-properties"));

    m_authorPage = new KoConfigAuthorPage();
    item = addPage(m_authorPage, i18nc("@title:tab Author page", "Author"));
    item->setHeader(i18n("Author"));
    item->setIcon(koIcon("user-identity"));

    connect( this, SIGNAL( okClicked() ), this, SLOT( slotApply() ) );
    connect( this, SIGNAL( defaultClicked() ), this, SLOT( slotDefault() ) );
    connect( this, SIGNAL( applyClicked() ), this, SLOT( slotApply() ) );
}

void KoPAConfigureDialog::slotApply()
{
    m_docPage->apply();
    m_gridPage->apply();
    m_miscPage->apply();
    m_authorPage->apply();
}

void KoPAConfigureDialog::slotDefault()
{
    QWidget* curr = currentPage()->widget();

    if (curr == m_gridPage) {
        m_gridPage->slotDefault();
    }
    else if (curr == m_docPage) {
        m_docPage->slotDefault();
    }
}
