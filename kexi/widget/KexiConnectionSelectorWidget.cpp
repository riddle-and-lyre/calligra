/* This file is part of the KDE project
   Copyright (C) 2003-2011 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2012 Dimitrios T. Tanis <dimitrios.tanis@kdemail.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KexiConnectionSelectorWidget.h"
#include "KexiFileWidget.h"
#include "ui_KexiConnectionSelector.h"
#include "kexiprjtypeselector.h"
#include "kexidbconnectionwidget.h"

#include <db/drivermanager.h>
#include <db/connectiondata.h>
#include <db/utils.h>
#include <kexiutils/utils.h>

#include <core/kexi.h>

#include <KoIcon.h>

#include <klocale.h>
#include <kdebug.h>
#include <kurlcombobox.h>
#include <kdialog.h>

#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QPixmap>
#include <QFrame>
#include <QStackedWidget>
#include <QKeyEvent>

class KexiConnectionSelector : public QWidget, public Ui_KexiConnectionSelector
{
public:
    explicit KexiConnectionSelector(QWidget *parent)
        : QWidget(parent)
    {
        setupUi(this);
        setObjectName("conn_sel");
        lblIcon->setPixmap(DesktopIcon(Kexi::serverIconName()));
        lblIcon->setFixedSize(lblIcon->pixmap()->size());
        btn_add->setToolTip(i18n("Add a new database connection"));
        btn_edit->setToolTip(i18n("Edit selected database connection"));
        btn_remove->setToolTip(i18n("Remove selected database connections"));
    }
    ~KexiConnectionSelector()
    {
    }
};

/*================================================================*/

ConnectionDataLVItem::ConnectionDataLVItem(KexiDB::ConnectionData *data,
        const KexiDB::Driver::Info &info, QTreeWidget *list)
    : QTreeWidgetItem(list)
    , m_data(data)
{
    update(info);
}

ConnectionDataLVItem::~ConnectionDataLVItem()
{
}

void ConnectionDataLVItem::update(const KexiDB::Driver::Info &info)
{
    setText(0, m_data->caption + "  ");
    const QString sfile = i18n("File");
    QString drvname = info.caption.isEmpty() ? m_data->driverName : info.caption;
    if (info.fileBased) {
        setText(1, sfile + " (" + drvname + ")  ");
    } else {
        setText(1, drvname + "  ");
    }
    setText(2, (info.fileBased ? (QString("<") + sfile.toLower() + ">") : m_data->serverInfoString(true)) + "  ");
}

/*================================================================*/

//! @internal
class KexiConnectionSelectorWidget::Private
{
public:
    Private()
        : conn_sel_shown(false)
        , file_sel_shown(false)
        , confirmOverwrites(true)
    {
    }

    KexiConnectionSelector *remote;
    QWidget *openExistingWidget;
    KexiPrjTypeSelector *prjTypeSelector;
    QString startDirOrVariable;
    KAbstractFileWidget::OperationMode fileAccessType;
    QStackedWidget *stack;
    QPointer<KexiDBConnectionSet> conn_set;
    KexiDB::DriverManager manager;
    bool conn_sel_shown; //!< helper
    bool file_sel_shown;
    bool confirmOverwrites;
    KexiUtils::PaintBlocker *descGroupBoxPaintBlocker;
    bool isConnectionSelected;
};

/*================================================================*/

KexiConnectionSelectorWidget::KexiConnectionSelectorWidget(
    KexiDBConnectionSet &conn_set,
    const QString &startDirOrVariable, KAbstractFileWidget::OperationMode fileAccessType, QWidget *parent)
    : QWidget(parent)
    , d(new Private())
{
    d->conn_set = &conn_set;
    d->startDirOrVariable = startDirOrVariable;
    d->fileAccessType = fileAccessType;
    m_errorMessagePopup = 0;
    setWindowIcon(Kexi::defaultFileBasedDriverIcon());

    QBoxLayout *globalLyr = new QVBoxLayout(this);

    //create header with radio buttons
    d->openExistingWidget = new QWidget(this);
    d->openExistingWidget->setObjectName("openExistingWidget");
    QVBoxLayout *openExistingWidgetLyr = new QVBoxLayout(d->openExistingWidget);
    openExistingWidgetLyr->setContentsMargins(0, 0, 0, 0);
    d->prjTypeSelector = new KexiPrjTypeSelector(d->openExistingWidget);
    connect(d->prjTypeSelector->buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(slotPrjTypeSelected(QAbstractButton*)));
    openExistingWidgetLyr->addWidget(d->prjTypeSelector);
    d->prjTypeSelector->setContentsMargins(0, 0, 0, KDialog::spacingHint());
    //openExistingWidgetLyr->addSpacing(KDialog::spacingHint());
    QFrame *line = new QFrame(d->openExistingWidget);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    openExistingWidgetLyr->addWidget(line);
    globalLyr->addWidget(d->openExistingWidget);

    d->stack = new QStackedWidget(this);
    d->stack->setObjectName("stack");
    globalLyr->addWidget(d->stack, 1);

    fileWidget = 0;

    d->remote = new KexiConnectionSelector(d->stack);
    connect(d->remote->btn_add, SIGNAL(clicked()), this, SLOT(slotRemoteAddBtnClicked()));
    connect(d->remote->btn_edit, SIGNAL(clicked()), this, SLOT(slotRemoteEditBtnClicked()));
    connect(d->remote->btn_remove, SIGNAL(clicked()), this, SLOT(slotRemoteRemoveBtnClicked()));
    d->stack->addWidget(d->remote);
    if (d->remote->layout()) {
        d->remote->layout()->setMargin(0);
    }
    connect(d->remote->list, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(slotConnectionItemExecuted(QTreeWidgetItem*)));
    connect(d->remote->list, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotConnectionSelectionChanged()));
    d->remote->list->installEventFilter(this);
    d->descGroupBoxPaintBlocker = new KexiUtils::PaintBlocker(d->remote->descGroupBox);
    d->descGroupBoxPaintBlocker->setEnabled(false);
    d->isConnectionSelected = false;
}

KexiConnectionSelectorWidget::~KexiConnectionSelectorWidget()
{
    delete d;
}

void KexiConnectionSelectorWidget::showAdvancedConn()
{
    d->prjTypeSelector->option_server->setChecked(true);
    slotPrjTypeSelected(d->prjTypeSelector->option_server);
}

void KexiConnectionSelectorWidget::slotPrjTypeSelected(QAbstractButton *btn)
{
    if (btn == d->prjTypeSelector->option_file) { //file-based prj type
        showSimpleConn();
    } else if (btn == d->prjTypeSelector->option_server) { //server-based prj type
        if (KexiDB::hasDatabaseServerDrivers()) {
            if (!d->conn_sel_shown) {
                d->conn_sel_shown = true;
                //show connections (on demand):
                foreach (KexiDB::ConnectionData *connData, d->conn_set->list()) {
                    addConnectionData(connData);
                    //   else {
                    //this error should be more verbose:
                    //    kWarning() << "no driver found for '" << it.current()->driverName << "'!";
                    //   }
                }
                if (d->remote->list->topLevelItemCount() > 0) {
                    d->remote->list->sortByColumn(0, Qt::AscendingOrder);
                    d->remote->list->resizeColumnToContents(0); // name
                    d->remote->list->resizeColumnToContents(1); // type
                    d->remote->list->topLevelItem(0)->setSelected(true);
                }
                d->remote->descGroupBox->layout()->setMargin(2);
                d->remote->list->setFocus();
                slotConnectionSelectionChanged();
            }
            d->stack->setCurrentWidget(d->remote);
        } else {
            if (!m_errorMessagePopup) {
                QWidget *errorMessagePopupParent = new QWidget(this);
                QVBoxLayout *vbox = new QVBoxLayout(errorMessagePopupParent);
                m_errorMessagePopup = new KexiServerDriverNotFoundMessage(errorMessagePopupParent);
                vbox->addWidget(m_errorMessagePopup);
                vbox->addStretch(0);
                d->stack->addWidget(errorMessagePopupParent);
                m_errorMessagePopup->setAutoDelete(false);
                d->stack->setCurrentWidget(m_errorMessagePopup->parentWidget());
                m_errorMessagePopup->animatedShow();
            } else {
                d->stack->setCurrentWidget(m_errorMessagePopup->parentWidget());
            }
        }
    }
}

ConnectionDataLVItem *KexiConnectionSelectorWidget::addConnectionData(KexiDB::ConnectionData *data)
{
    const KexiDB::Driver::Info info(d->manager.driverInfo(data->driverName));
    return new ConnectionDataLVItem(data, info, d->remote->list);
}

void KexiConnectionSelectorWidget::showSimpleConn()
{
    d->prjTypeSelector->option_file->setChecked(true);
    if (!d->file_sel_shown) {
        d->file_sel_shown = true;
        fileWidget = new KexiFileWidget(
            KUrl(d->startDirOrVariable),
            d->fileAccessType == KAbstractFileWidget::Opening
            ? KexiFileWidget::Opening : KexiFileWidget::SavingFileBasedDB,
            d->stack);
        fileWidget->setOperationMode(d->fileAccessType);
        fileWidget->setObjectName("openFileWidget");
        fileWidget->setConfirmOverwrites(d->confirmOverwrites);
        d->stack->addWidget(fileWidget);

        for (QWidget *w = parentWidget(); w; w = w->parentWidget()) {
            if (w->windowType() == Qt::Dialog) {
#ifdef __GNUC__
#warning TODO KFileWidget    connect(m_fileDlg, SIGNAL(rejected()), qobject_cast<QDialog*>(w), SLOT(reject()));
#else
#pragma WARNING( TODO KFileWidget    connect(m_fileDlg, SIGNAL(rejected()), qobject_cast<QDialog*>(w), SLOT(reject())); )
#endif
                break;
            }
        }
    }
    d->stack->setCurrentWidget(fileWidget);
    connect(fileWidget->locationEdit()->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(slotConnectionSelected()));
}

KexiConnectionSelectorWidget::ConnectionType KexiConnectionSelectorWidget::selectedConnectionType() const
{
    return (d->stack->currentWidget() == fileWidget) ? FileBased : ServerBased;
}

KexiDB::ConnectionData *KexiConnectionSelectorWidget::selectedConnectionData() const
{
    QList<QTreeWidgetItem *> items = d->remote->list->selectedItems();
    if (items.isEmpty()) {
        return 0;
    }
    ConnectionDataLVItem *item = static_cast<ConnectionDataLVItem *>(items.first());
    if (!item) {
        return 0;
    }
    return item->data();
}

QString KexiConnectionSelectorWidget::selectedFileName()
{
    if (selectedConnectionType() != KexiConnectionSelectorWidget::FileBased) {
        return QString();
    } else if (fileWidget->selectedFile().isEmpty()) {
        KUrl path = fileWidget->baseUrl();
        const QString firstUrl(fileWidget->locationEdit()->lineEdit()->text());
        if (QDir::isAbsolutePath(firstUrl)) {
            path = KUrl::fromPath(firstUrl);
        } else {
            path.addPath(firstUrl);
        }
        return path.toLocalFile();
    }

    return fileWidget->highlightedFile(); //ok? fileWidget->selectedFile();
}

void KexiConnectionSelectorWidget::setSelectedFileName(const QString &fileName)
{
    if (selectedConnectionType() != KexiConnectionSelectorWidget::FileBased) {
        return;
    }
    return fileWidget->setSelection(fileName);
}

void KexiConnectionSelectorWidget::slotConnectionItemExecuted(QTreeWidgetItem *item)
{
    emit connectionItemExecuted(static_cast<ConnectionDataLVItem *>(item));
    slotConnectionSelected();
}

void KexiConnectionSelectorWidget::slotConnectionItemExecuted()
{
    QList<QTreeWidgetItem *> items = d->remote->list->selectedItems();
    if (items.isEmpty()) {
        return;
    }
    slotConnectionItemExecuted(items.first());
    slotConnectionSelected();
}

void KexiConnectionSelectorWidget::slotConnectionSelectionChanged()
{
    QList<QTreeWidgetItem *> items = d->remote->list->selectedItems();
    if (items.isEmpty()) {
        return;
    }
    ConnectionDataLVItem *item = static_cast<ConnectionDataLVItem *>(items.first());
    d->remote->btn_edit->setEnabled(item);
    d->remote->btn_remove->setEnabled(item);
    QString desc;
    if (item) {
        desc = item->data()->description;
    }
    d->descGroupBoxPaintBlocker->setEnabled(desc.isEmpty());
    d->remote->descriptionLabel->setText(desc);
    slotConnectionSelected();
    emit connectionItemHighlighted(item);
}

QTreeWidget *KexiConnectionSelectorWidget::connectionsList() const
{
    return d->remote->list;
}

void KexiConnectionSelectorWidget::setFocus()
{
    QWidget::setFocus();
    if (d->stack->currentWidget() == fileWidget) {
        fileWidget->setFocus();
    } else {
        d->remote->list->setFocus();
    }
}

void KexiConnectionSelectorWidget::hideHelpers()
{
    d->openExistingWidget->hide();
}

void KexiConnectionSelectorWidget::setConfirmOverwrites(bool set)
{
    d->confirmOverwrites = set;
    if (fileWidget) {
        fileWidget->setConfirmOverwrites(d->confirmOverwrites);
    }
}

bool KexiConnectionSelectorWidget::confirmOverwrites() const
{
    return d->confirmOverwrites;
}

void KexiConnectionSelectorWidget::slotRemoteAddBtnClicked()
{
    KexiDB::ConnectionData data;
    KexiDBConnectionDialog dlg(this, data, QString(),
                               KGuiItem(i18n("&Add"), koIconName("dialog-ok"), i18n("Add database connection")));
    dlg.setWindowTitle(i18nc("@title:window", "Add a New Database Connection"));
    if (QDialog::Accepted != dlg.exec()) {
        return;
    }

    //store this conn. data
    KexiDB::ConnectionData *newData
        = new KexiDB::ConnectionData(*dlg.currentProjectData().connectionData());
    if (!d->conn_set->addConnectionData(newData)) {
        //! @todo msg?
        delete newData;
        return;
    }

    ConnectionDataLVItem *item = addConnectionData(newData);
    d->remote->list->clearSelection();
    item->setSelected(true);
    slotConnectionSelectionChanged();
}

void KexiConnectionSelectorWidget::slotRemoteEditBtnClicked()
{
    QList<QTreeWidgetItem *> items = d->remote->list->selectedItems();
    if (items.isEmpty()) {
        return;
    }
    ConnectionDataLVItem *item = static_cast<ConnectionDataLVItem *>(items.first());
    if (!item) {
        return;
    }
    KexiDBConnectionDialog dlg(this, *item->data(), QString(),
                               KGuiItem(i18n("&Save"), koIconName("document-save"),
                                        i18n("Save changes made to this database connection")));
    dlg.setWindowTitle(i18nc("@title:window", "Edit Database Connection"));
    if (QDialog::Accepted != dlg.exec()) {
        return;
    }

    if (!d->conn_set->saveConnectionData(item->data(), *dlg.currentProjectData().connectionData())) {
        //! @todo msg?
        return;
    }
    const KexiDB::Driver::Info info(d->manager.driverInfo(item->data()->driverName));
    item->update(info);
    slotConnectionSelectionChanged(); //to update descr. edit
}

void KexiConnectionSelectorWidget::slotRemoteRemoveBtnClicked()
{
    QList<QTreeWidgetItem *> items = d->remote->list->selectedItems();
    if (items.isEmpty()) {
        return;
    }
    ConnectionDataLVItem *item = static_cast<ConnectionDataLVItem *>(items.first());
    if (!item) {
        return;
    }
    if (KMessageBox::Continue != KMessageBox::warningContinueCancel(this,
            i18n(
                "Do you want to remove database connection \"%1\" from the list of available connections?",
                item->data()->serverInfoString(true)),
            QString(), //caption
            KStandardGuiItem::remove(), KStandardGuiItem::cancel(),
            QString(), //dont'ask name
            KMessageBox::Notify | KMessageBox::Dangerous)) {
        return;
    }

    QTreeWidgetItem *nextItem = d->remote->list->itemBelow(item);
    if (!nextItem) {
        nextItem = d->remote->list->itemAbove(item);
    }
    if (!d->conn_set->removeConnectionData(item->data())) {
        return;
    }

    if (nextItem) {
        nextItem->setSelected(true);
    }

    delete item;

    slotConnectionSelectionChanged();
}

void KexiConnectionSelectorWidget::hideConnectonIcon()
{
    d->remote->lblIcon->setFixedWidth(0);
    d->remote->lblIcon->setPixmap(QPixmap());
}

void KexiConnectionSelectorWidget::hideDescription()
{
    d->remote->lblIcon->hide();
    d->remote->label->hide();
}

bool KexiConnectionSelectorWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if ((ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return)
                && ke->modifiers() == Qt::NoModifier) {
            slotConnectionItemExecuted();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void KexiConnectionSelectorWidget::slotConnectionSelected()
{
    QList<QTreeWidgetItem *> items;
    QLineEdit *lineEdit;
    switch (selectedConnectionType()) {
    case KexiConnectionSelectorWidget::FileBased:
        lineEdit = fileWidget->locationEdit()->lineEdit();
        d->isConnectionSelected = !lineEdit->text().isEmpty();
        break;
    case KexiConnectionSelectorWidget::ServerBased:
        items = d->remote->list->selectedItems();
        d->isConnectionSelected = !items.isEmpty();
        break;
    default:;
    }
    emit connectionSelected(d->isConnectionSelected);
}

bool KexiConnectionSelectorWidget::hasSelectedConnection() const
{
    return d->isConnectionSelected;
}
