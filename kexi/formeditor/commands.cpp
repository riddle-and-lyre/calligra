/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#include <qdom.h>
#include <qwidget.h>
#include <qlayout.h>

#include <kdebug.h>
#include <klocale.h>
#include <kpopupmenu.h>

#include "formIO.h"
#include "container.h"
#include "objecttree.h"
#include "formmanager.h"
#include "form.h"
#include "objpropbuffer.h"
#include "kexiproperty.h"
#include "widgetlibrary.h"

#include "commands.h"

namespace KFormDesigner {

// PropertyCommand

PropertyCommand::PropertyCommand(ObjectPropertyBuffer *buf, const QString &wname, const QVariant &oldValue, const QVariant &value, const QCString &property)
  : KCommand(), m_buffer(buf), m_value(value), m_property(property)
{
	m_oldvalues.insert(wname, oldValue);
}

PropertyCommand::PropertyCommand(ObjectPropertyBuffer *buf, const QMap<QString, QVariant> &oldvalues, const QVariant &value, const QCString &property)
  : KCommand(), m_buffer(buf), m_value(value), m_oldvalues(oldvalues), m_property(property)
{}

void
PropertyCommand::setValue(const QVariant &value)
{
	m_value = value;
}

void
PropertyCommand::execute()
{
	m_buffer->m_undoing = true;
	QWidget *w = m_buffer->m_manager->activeForm()->objectTree()->lookup(m_oldvalues.begin().key())->widget();
	m_buffer->m_manager->activeForm()->setSelWidget(w);
	m_buffer->setWidget(w);

	if(m_oldvalues.count() >= 2)
	{
		QMap<QString, QVariant>::Iterator it = m_oldvalues.begin();
		++it;
		for(; it != m_oldvalues.end(); ++it)
		{
			QWidget *widg = m_buffer->m_manager->activeForm()->objectTree()->lookup(it.key())->widget();
			m_buffer->m_manager->activeForm()->addSelectedWidget(widg);
			m_buffer->addWidget(w);
		}
	}

	(*m_buffer)[m_property]->setValue(m_value);
	m_buffer->m_undoing = false;
}

void
PropertyCommand::unexecute()
{
	m_buffer->m_undoing = true;
	QWidget *w = m_buffer->m_manager->activeForm()->objectTree()->lookup(m_oldvalues.begin().key())->widget();
	m_buffer->m_manager->activeForm()->setSelWidget(w);
	m_buffer->setWidget(w);

	(*m_buffer)[m_property]->setValue(m_oldvalues.begin().data());
	m_buffer->m_undoing = false;

	if(m_oldvalues.count() >= 2)
	{
		QMap<QString, QVariant>::Iterator it = m_oldvalues.begin();
		++it;
		for(; it != m_oldvalues.end(); ++it)
		{
			QWidget *widg = m_buffer->m_manager->activeForm()->objectTree()->lookup(it.key())->widget();
			m_buffer->m_manager->activeForm()->addSelectedWidget(widg);
			m_buffer->addWidget(widg);
			widg->setProperty(m_property, it.data());
		}
	}
}

QString
PropertyCommand::name() const
{
	if(m_oldvalues.count() >= 2)
		return i18n("Change %1 of multiple widgets" ).arg(m_property);
	else
		return i18n("Change %1 of widget %2" ).arg(m_property).arg(m_oldvalues.begin().key());
}

// GeometryPropertyCommand (for multiples widgets)

GeometryPropertyCommand::GeometryPropertyCommand(ObjectPropertyBuffer *buf, const QStringList &names, QPoint oldPos)
 : KCommand(), m_buffer(buf), m_names(names), m_oldPos(oldPos)
{}

void
GeometryPropertyCommand::execute()
{
	m_buffer->m_undoing = true;
	int dx = m_pos.x() - m_oldPos.x();
	int dy = m_pos.y() - m_oldPos.y();

	for(QStringList::Iterator it = m_names.begin(); it != m_names.end(); ++it)
	{
		QWidget *w = m_buffer->m_manager->activeForm()->objectTree()->lookup(*it)->widget();
		w->move(w->x() + dx, w->y() + dy);
	}
	m_buffer->m_undoing = false;
}

void
GeometryPropertyCommand::unexecute()
{
	m_buffer->m_undoing = true;
	int dx = m_pos.x() - m_oldPos.x();
	int dy = m_pos.y() - m_oldPos.y();

	for(QStringList::Iterator it = m_names.begin(); it != m_names.end(); ++it)
	{
		QWidget *w = m_buffer->m_manager->activeForm()->objectTree()->lookup(*it)->widget();
		w->move(w->x() - dx, w->y() - dy);
	}
	m_buffer->m_undoing = false;
}

void
GeometryPropertyCommand::setPos(QPoint pos)
{
	m_pos = pos;
}

QString
GeometryPropertyCommand::name() const
{
	return i18n("Move multiple widgets");
}

// LayoutPropertyCommand

LayoutPropertyCommand::LayoutPropertyCommand(ObjectPropertyBuffer *buf, const QString &name, const QVariant &oldValue, const QVariant &value)
 : PropertyCommand(buf, name, oldValue, value, "layout")
{
	m_form = buf->m_manager->activeForm();
	Container *m_container = m_form->objectTree()->lookup(name)->container();
	for(ObjectTreeItem *it = m_container->tree()->children()->first(); it; it = m_container->tree()->children()->next())
		m_geometries.insert(it->name(), it->widget()->geometry());
}

void
LayoutPropertyCommand::execute()
{
	PropertyCommand::execute();
}

void
LayoutPropertyCommand::unexecute()
{
	Container *m_container = m_form->objectTree()->lookup(m_oldvalues.begin().key())->container();
	m_container->setLayout(Container::NoLayout);
	for(QMap<QString,QRect>::Iterator it = m_geometries.begin(); it != m_geometries.end(); ++it)
	{
		ObjectTreeItem *tree = m_container->form()->objectTree()->lookup(it.key());
		if(tree)
			tree->widget()->setGeometry(it.data());
	}

	PropertyCommand::unexecute();
}

QString
LayoutPropertyCommand::name() const
{
	return i18n("Change layout of %1").arg(m_oldvalues.begin().key());
}


// InsertWidgetCommand

InsertWidgetCommand::InsertWidgetCommand(Container *container, QPoint p)
  : KCommand(), m_point(p)
{
	m_containername = container->widget()->name();
	m_form = container->form();
	m_class = container->form()->manager()->insertClass();
	m_insertRect = container->m_insertRect;
}

void
InsertWidgetCommand::execute()
{
	if (!m_form->objectTree())
		return;
	Container *m_container = m_form->objectTree()->lookup(m_containername)->container();
	if(m_name.isEmpty())
		m_name = m_container->form()->objectTree()->genName(m_container->form()->manager()->lib()->displayName(m_class));

	QWidget *w = m_container->form()->manager()->lib()->createWidget(m_class, m_container->m_container, m_name.latin1(), m_container);

	if(!w)
		return;

	m_container->m_insertRect = m_insertRect;
	if(!m_container->m_insertRect.isValid())
	{
		QSize s = w->sizeHint();
		if(s.isEmpty())
			s = QSize(20, 20);
		m_container->m_insertRect = QRect(m_point.x(), m_point.y(), s.width(), s.height());
	}
	w->move(m_container->m_insertRect.x(), m_container->m_insertRect.y());
	w->resize(m_container->m_insertRect.width()-1, m_container->m_insertRect.height()-1);
	w->show();
	m_container->m_container->repaint();

	m_container->m_insertRect = QRect();
	m_container->form()->manager()->stopInsert();

	if (!m_container->form()->objectTree()->lookup(m_name))
	{
		EventEater *eater = new EventEater(w, m_container);
		m_container->form()->objectTree()->addChild(m_container->m_tree, new ObjectTreeItem(m_container->form()->manager()->lib()->displayName(m_class), m_name, w, eater));
	}


	ObjectTreeItem *item = m_container->form()->objectTree()->lookup(m_name);
	QStringList list(m_container->form()->manager()->lib()->autoSaveProperties(w->className()));
	for(QStringList::Iterator it = list.begin(); it != list.end(); ++it)
		item->addModProperty(*it, w->property((*it).latin1()));

	m_container->reloadLayout();

	m_container->setSelectedWidget(w, false);
	m_form->manager()->lib()->startEditing(w->className(), w, m_container);
	kdDebug() << "Container::eventFilter(): widget added " << this << endl;
}

void
InsertWidgetCommand::unexecute()
{
	QWidget *m_widget = m_form->objectTree()->lookup(m_name)->widget();
	Container *m_container = m_form->objectTree()->lookup(m_containername)->container();
	m_container->m_selected.clear();
	m_container->m_selected.append(m_widget);
	m_container->deleteItem();
}

QString
InsertWidgetCommand::name() const
{
	if(!m_name.isEmpty())
		return i18n("Insert %1").arg(m_name);
	else
		return i18n("Insert widget");
}

/// CreateLayoutCommand ///////////////

CreateLayoutCommand::CreateLayoutCommand(int layoutType, QtWidgetList &list, Form *form)
 : m_form(form), m_type(layoutType)
{
	QtWidgetList *m_list;
	switch(layoutType)
	{
		case Container::HBox:
		case Container::Grid:
			m_list = new HorWidgetList(); break;
		case Container::VBox:
			m_list = new VerWidgetList(); break;
	}
	for(QWidget *w = list.first(); w; w = list.next())
		m_list->append(w);
	m_list->sort();

	for(QWidget *w = m_list->first(); w; w = m_list->next())
		m_pos.insert(w->name(), w->geometry());
	ObjectTreeItem *item = form->objectTree()->lookup(m_list->first()->name());
	if(item && item->parent()->container())
		m_containername = item->parent()->name();
	delete m_list;
}

void
CreateLayoutCommand::execute()
{
	WidgetLibrary *lib = m_form->manager()->lib();
	if(!lib)  return;
	Container *container = m_form->objectTree()->lookup(m_containername)->container();
	if(!container)
		container = m_form->toplevelContainer();

	QString classname;
	switch(m_type)
	{
		case Container::HBox:
			classname = "HBox"; break;
		case Container::VBox:
			classname = "VBox"; break;
		case Container::Grid:
			classname = "Grid"; break;
		default: break;
	}

	if(m_name.isEmpty())
		m_name = m_form->objectTree()->genName(classname);
	QWidget *w = lib->createWidget(classname, container->widget(), m_name.latin1(), container);
	ObjectTreeItem *tree = m_form->objectTree()->lookup(w->name());
	if(!tree)
		return;

	container->setSelectedWidget(0, false);
	w->move(m_pos.begin().data().topLeft());
	w->show();

	for(QMap<QString,QRect>::Iterator it = m_pos.begin(); it != m_pos.end(); ++it)
	{
		ObjectTreeItem *item = m_form->objectTree()->lookup(it.key());
		if(item && item->widget())
		{
			item->widget()->reparent(w, item->widget()->pos(), true);
			item->eventEater()->setContainer(tree->container());
			m_form->objectTree()->reparent(item->name(), m_name);
		}
	}

	tree->container()->setLayout((Container::LayoutType)m_type);
	tree->widget()->resize(tree->container()->layout()->sizeHint());
	container->setSelectedWidget(w, false);
	m_form->manager()->windowChanged(m_form->toplevelContainer()->widget());
}

void
CreateLayoutCommand::unexecute()
{
	ObjectTreeItem *parent = m_form->objectTree()->lookup(m_containername);
	if(!parent)
		parent = m_form->objectTree();

	for(QMap<QString,QRect>::Iterator it = m_pos.begin(); it != m_pos.end(); ++it)
	{
		ObjectTreeItem *item = m_form->objectTree()->lookup(it.key());
		if(item && item->widget())
		{
			kdDebug() << m_form->objectTree()->lookup(m_containername) << endl;
			item->widget()->reparent(parent->widget(), QPoint(0,0), true);
			item->eventEater()->setContainer(parent->container());
			item->widget()->setGeometry(m_pos[it.key()]);
			m_form->objectTree()->reparent(item->name(), m_containername);
		}
	}

	if(!parent->container())
		return;
	QWidget *w = m_form->objectTree()->lookup(m_name)->widget();
	parent->container()->setSelectedWidget(w, false);
	parent->container()->deleteItem();
	m_form->manager()->windowChanged(m_form->toplevelContainer()->widget());
}

QString
CreateLayoutCommand::name() const
{
	switch(m_type)
	{
		case Container::HBox:
			return i18n("Lay out widgets horizontally");
		case Container::VBox:
			return i18n("Lay out widgets vertically");
		case Container::Grid:
			return i18n("Lay out widgets in a grid");
		default:
			return i18n("Create Layout");
	}
}

// PasteWidgetCommand

PasteWidgetCommand::PasteWidgetCommand(QDomDocument &domDoc, Container *container, QPoint p)
  : m_point(p)
{
	m_data = domDoc.toCString();
	m_containername = container->widget()->name();
	m_form = container->form();
}

void
PasteWidgetCommand::execute()
{
	Container *m_container = m_form->objectTree()->lookup(m_containername)->container();
	QString errMsg;
	int errLine;
	int errCol;
	QDomDocument domDoc("UI");
	bool parsed = domDoc.setContent(m_data, false, &errMsg, &errLine, &errCol);

	if(!parsed)
	{
		kdDebug() << "WidgetWatcher::load(): " << errMsg << endl;
		kdDebug() << "WidgetWatcher::load(): line: " << errLine << " col: " << errCol << endl;
		return;
	}

	if(!domDoc.namedItem("UI").hasChildNodes())
		return;
	if(domDoc.namedItem("UI").firstChild().nextSibling().isNull())
	{
		QDomElement widg = domDoc.namedItem("UI").firstChild().toElement();
		if(m_point.isNull())
			m_container->form()->pasteWidget(widg, m_container);
		else
			m_container->form()->pasteWidget(widg, m_container,  m_point);
	}
	else for(QDomNode n = domDoc.namedItem("UI").firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if(n.toElement().tagName() != "widget")
			continue;
		QDomElement widg = n.toElement();
		m_container->form()->pasteWidget(widg, m_container);
	}

	m_names.clear();
	for(QDomNode n = domDoc.namedItem("UI").firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if(n.toElement().tagName() != "widget")
			break;
		for(QDomNode m = n.firstChild(); !m.isNull(); n = m.nextSibling())
		{
			if((m.toElement().tagName() == "property") && (m.toElement().attribute("name") == "name"))
			{
				m_names.append(m.toElement().text());
				break;
			}
		}
	}
}

void
PasteWidgetCommand::unexecute()
{
	Container *m_container = m_form->objectTree()->lookup(m_containername)->container();
	m_container->m_selected.clear();
	for(QStringList::Iterator it = m_names.begin(); it != m_names.end(); ++it)
	{
		QWidget *w = m_container->form()->objectTree()->lookup(*it)->widget();
		m_container->m_selected.append(w);
	}

	m_container->deleteItem();
}

QString
PasteWidgetCommand::name() const
{
	return i18n("Paste");
}

// DeleteWidgetCommand

DeleteWidgetCommand::DeleteWidgetCommand(QtWidgetList &list, Form *form)
 : KCommand(), m_form(form)
{
	m_domDoc = QDomDocument("UI");
	m_domDoc.appendChild(m_domDoc.createElement("UI"));

	QDomElement parent = m_domDoc.namedItem("UI").toElement();

	for(QWidget *w = list.first(); w; w = list.next())
	{
		ObjectTreeItem *item = m_form->objectTree()->lookup(w->name());
		if (!item)
			return;
		m_containers.insert(item->name(), m_form->parentContainer(item->widget())->widget()->name());
		m_parents.insert(item->name(), item->parent()->name());
		FormIO::saveWidget(item, parent, m_domDoc);
	}
}

void
DeleteWidgetCommand::execute()
{
	QMap<QString,QString>::Iterator it;
	for(it = m_containers.begin(); it != m_containers.end(); ++it)
	{
		ObjectTreeItem *item = m_form->objectTree()->lookup(it.key());
		if (!item || !item->widget())
			continue;

		Container *cont = m_form->parentContainer(item->widget());
		cont->m_selected.clear();
		cont->m_selected.append(item->widget());
		cont->deleteItem();
	}
}

void
DeleteWidgetCommand::unexecute()
{
	QString wname;
	m_form->setInteractiveMode(false);
	for(QDomNode n = m_domDoc.namedItem("UI").firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if(n.toElement().tagName() != "widget")
			continue;
		for(QDomNode m = n.firstChild(); !m.isNull(); n = m.nextSibling())
		{
			if((m.toElement().tagName() == "property") && (m.toElement().attribute("name") == "name"))
			{
				wname = m.toElement().text();
				break;
			}
		}

		Container *cont = m_form->objectTree()->lookup(m_containers[wname])->container();
		ObjectTreeItem *parent = m_form->objectTree()->lookup(m_parents[wname]);
		QDomElement widg = n.toElement();
		if(parent)
			FormIO::loadWidget(cont, m_form->manager()->lib(), widg, parent->widget());
		else
			FormIO::loadWidget(cont, m_form->manager()->lib(), widg);
	}
	m_form->setInteractiveMode(true);
}

QString
DeleteWidgetCommand::name() const
{
	return i18n("Delete widget");
}

// CutWidgetCommand

CutWidgetCommand::CutWidgetCommand(QtWidgetList &list, Form *form)
 : DeleteWidgetCommand(list, form)
{}

void
CutWidgetCommand::execute()
{
	DeleteWidgetCommand::execute();
	m_data = m_form->manager()->m_domDoc.toCString();
	m_form->manager()->m_domDoc.setContent(m_domDoc.toCString());
}

void
CutWidgetCommand::unexecute()
{
	DeleteWidgetCommand::unexecute();
	m_form->manager()->m_domDoc.setContent(m_data);
}

QString
CutWidgetCommand::name() const
{
	return i18n("Cut");
}

}

