/* This file is part of the KDE project
  Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
  Copyright (C) 2002 - 2009, 2011 Dag Andersen <danders@get2net.dk>

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

#ifndef KPLATOWORK_VIEW
#define KPLATOWORK_VIEW

#include <KoView.h>

#include <QStackedWidget>

class QPrinter;
class QPrintDialog;
class QMenu;
class QActionGroup;

class KActionCollection;

class KoStore;
class KoView;

class KPlatoWork_MainWindow;

namespace KPlato
{

class Document;
class GanttView;
class ScheduleManager;

class Node;
class Schedule;
class Resource;

}
using namespace KPlato;

/// Then namespace for the KPlato work package handler
namespace KPlatoWork
{

class Part;
class View;
class AbstractView;
class TaskWorkPackageView;
class TaskWPGanttView;

//-------------
class View : public QStackedWidget
{
    Q_OBJECT

public:
    explicit View( Part* part, QWidget *parent, KActionCollection *collection );
    ~View();

    Part *part() const;

    virtual void setupPrinter( QPrinter &printer, QPrintDialog &printDialog );
    virtual void print( QPrinter &printer, QPrintDialog &printDialog );

    QMenu *popupMenu( const QString& name, const QPoint &pos );

//    virtual ViewAdaptor* dbusObject();

    virtual bool loadContext();
    virtual void saveContext() const;

    ScheduleManager *currentScheduleManager() const;
    long currentScheduleId() const;
    
    TaskWorkPackageView *createTaskWorkPackageView();
    TaskWPGanttView *createGanttView();

    KPlatoWork_MainWindow *kplatoWorkMainWindow() const;
    
    Node *currentNode() const;
    Document *currentDocument() const;

Q_SIGNALS:
    void currentScheduleManagerChanged( ScheduleManager *sm );
    void openInternalDocument( KoStore * );
    void sigUpdateReadWrite( bool );

    void viewDocument( Document *doc );

public Q_SLOTS:
    void slotEditCut();
    void slotEditCopy();
    void slotEditPaste();

    void slotConfigure();

    void slotPopupMenu( const QString& menuname, const QPoint &pos );

    void slotTaskProgress();
    void slotTaskCompletion();

protected Q_SLOTS:
    void slotCurrentChanged( int index );
    void slotProgressChanged( int value );

    void slotEditDocument();
    void slotEditDocument( Document *doc );
    void slotViewDocument();
    void slotRemoveDocument();
    
    void slotSendPackage();
    void slotPackageSettings();
    void slotTaskDescription();
    void slotRemoveCurrentPackage();
    void slotRemoveSelectedPackages();
    void slotSelectionChanged();

    void slotViewList();
    void slotViewGantt();

protected:
    virtual void updateReadWrite( bool readwrite );

    QAction *addScheduleAction( Schedule *sch );
    void setLabel();
    AbstractView *currentView() const;

private:
    void createViews();
    
private:
    Part *m_part;

    QActionGroup *m_scheduleActionGroup;
    QMap<QAction*, Schedule*> m_scheduleActions;
    ScheduleManager *m_manager;
    
    bool m_readWrite;
    
    // ------ Edit
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
    QAction *actionRemoveSelectedPackages;

    // ------ View
    QAction *actionViewList;
    QAction *actionViewGantt;

    // ------ Settings
    QAction *actionConfigure;

    QAction *actionViewDocument;
    QAction *actionEditDocument;
    QAction *actionRemoveDocument;

    QAction *actionSendPackage;
    QAction *actionPackageSettings;
    QAction *actionTaskCompletion;
    QAction *actionViewDescription;
    QAction *actionRemoveCurrentPackage;
};

} //KplatoWork namespace

#endif
