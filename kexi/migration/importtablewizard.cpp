/* This file is part of the KDE project
   Copyright (C) 2009 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2014 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "importtablewizard.h"
#include "migratemanager.h"
#include "keximigrate.h"
#include "keximigratedata.h"
#include "AlterSchemaWidget.h"

#include <KoIcon.h>

#include <QSet>
#include <QVBoxLayout>
#include <QListWidget>
#include <QStringList>
#include <QProgressBar>
#include <QCheckBox>

#include <kpushbutton.h>
#include <kdebug.h>

#include <db/drivermanager.h>
#include <db/driver.h>
#include <db/connectiondata.h>
#include <db/utils.h>
#include <db/drivermanager.h>
#include <core/kexidbconnectionset.h>
#include <core/kexi.h>
#include <core/kexipartmanager.h>
#include <kexiutils/utils.h>
#include <kexidbdrivercombobox.h>
#include <kexitextmsghandler.h>
#include <kexipart.h>
#include <KexiMainWindowIface.h>
#include <kexiproject.h>
#include <widget/kexicharencodingcombobox.h>
#include <widget/kexiprjtypeselector.h>
#include <widget/KexiConnectionSelectorWidget.h>
#include <widget/KexiProjectSelectorWidget.h>
#include <widget/KexiDBTitlePage.h>
#include <widget/KexiFileWidget.h>
#include <widget/KexiNameWidget.h>

using namespace KexiMigration;

#define ROWS_FOR_PREVIEW 3

ImportTableWizard::ImportTableWizard(KexiDB::Connection *curDB, QWidget *parent, QMap<QString, QString> *args, Qt::WFlags flags)
    : KAssistantDialog(parent, flags),
      m_args(args)
{
    m_currentDatabase = curDB;
    m_migrateDriver = 0;
    m_prjSet = 0;
    m_migrateManager = new MigrateManager();
    m_importComplete = false;

    KexiMainWindowIface::global()->setReasonableDialogSize(this);

    setupIntroPage();
    setupSrcConn();
    setupSrcDB();
    setupTableSelectPage();
    setupAlterTablePage();
    setupImportingPage();
    setupFinishPage();
    setValid(m_srcConnPageItem, false);

    connect(this, SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)), this, SLOT(slot_currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)));
    //! @todo Change this to message prompt when we move to non-dialog wizard.
    connect(m_srcConnSel, SIGNAL(connectionSelected(bool)), this, SLOT(slotConnPageItemSelected(bool)));
}

ImportTableWizard::~ImportTableWizard()
{
    delete m_migrateManager;
    delete m_prjSet;
    delete m_srcConnSel;

}

void ImportTableWizard::back()
{
    KAssistantDialog::back();
}

void ImportTableWizard::next()
{
    if (currentPage() == m_srcConnPageItem) {
        if (fileBasedSrcSelected()) {
            setAppropriate(m_srcDBPageItem, false);
        } else {
            setAppropriate(m_srcDBPageItem, true);
        }
    } else if (currentPage() == m_importingPageItem) {
        if (doImport()) {
            KAssistantDialog::next();
            return;
        }
    } else if (currentPage() == m_alterTablePageItem) {
        if (m_alterSchemaWidget->nameExists(m_alterSchemaWidget->nameWidget()->nameText())) {
            KMessageBox::information(this,
                                     i18n("<resource>%1</resource> name is already used by an existing table. "
                                          "Enter different table name to continue.", m_alterSchemaWidget->nameWidget()->nameText()),
                                     i18n("Name Already Used"));
            return;
        }
    }

    KAssistantDialog::next();
}

void ImportTableWizard::accept()
{
    if (m_args) {
        if (m_finishCheckBox->isChecked()) {
            m_args->insert("destinationTableName", m_alterSchemaWidget->nameWidget()->nameText());
        } else {
            m_args->remove("destinationTableName");
        }
    }

    QDialog::accept();
}

void ImportTableWizard::reject()
{
    QDialog::reject();
}

//===========================================================
//
void ImportTableWizard::setupIntroPage()
{
    m_introPageWidget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout();

    m_introPageWidget->setLayout(vbox);

    KexiUtils::setStandardMarginsAndSpacing(vbox);

    QLabel *lblIntro = new QLabel(m_introPageWidget);
    lblIntro->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    lblIntro->setWordWrap(true);
    lblIntro->setText(
        i18n("<para>Table Importing Assistant allows you to import a table from an existing "
             "database into the current Kexi project.</para>"
             "<para>Click <interface>Next</interface> button to continue or "
             "<interface>Cancel</interface> button to exit this assistant.</para>"));
    vbox->addWidget(lblIntro);

    m_introPageItem = new KPageWidgetItem(m_introPageWidget,
                                          i18n("Welcome to the Table Importing Assistant"));
    addPage(m_introPageItem);
}

void ImportTableWizard::setupSrcConn()
{
    m_srcConnPageWidget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(m_srcConnPageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);

    m_srcConnSel = new KexiConnectionSelectorWidget(Kexi::connset(),
            "kfiledialog:///ProjectMigrationSourceDir",
            KAbstractFileWidget::Opening, m_srcConnPageWidget);

    m_srcConnSel->hideConnectonIcon();
    m_srcConnSel->showSimpleConn();

    QSet<QString> excludedFilters;
    //! @todo remove when support for kexi files as source prj is added in migration
    excludedFilters
            << KexiDB::defaultFileBasedDriverMimeType()
            << "application/x-kexiproject-shortcut"
            << "application/x-kexi-connectiondata";
    m_srcConnSel->fileWidget->setExcludedFilters(excludedFilters);

    vbox->addWidget(m_srcConnSel);

    m_srcConnPageItem = new KPageWidgetItem(m_srcConnPageWidget, i18n("Select Location for Source Database"));
    addPage(m_srcConnPageItem);
}

void ImportTableWizard::setupSrcDB()
{
    // arrivesrcdbPage creates widgets on that page
    m_srcDBPageWidget = new QWidget(this);
    m_srcDBName = NULL;

    m_srcDBPageItem = new KPageWidgetItem(m_srcDBPageWidget, i18n("Select Source Database"));
    addPage(m_srcDBPageItem);
}

void ImportTableWizard::setupTableSelectPage()
{
    m_tablesPageWidget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(m_tablesPageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);

    m_tableListWidget = new QListWidget(this);
    m_tableListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(m_tableListWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotTableListWidgetSelectionChanged()));

    vbox->addWidget(m_tableListWidget);

    m_tablesPageItem = new KPageWidgetItem(m_tablesPageWidget, i18n("Select the Table to Import"));
    addPage(m_tablesPageItem);
}

//===========================================================
//
void ImportTableWizard::setupImportingPage()
{
    m_importingPageWidget = new QWidget(this);
    m_importingPageWidget->hide();
    QVBoxLayout *vbox = new QVBoxLayout(m_importingPageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);
    m_lblImportingTxt = new QLabel(m_importingPageWidget);
    m_lblImportingTxt->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_lblImportingTxt->setWordWrap(true);

    m_lblImportingErrTxt = new QLabel(m_importingPageWidget);
    m_lblImportingErrTxt->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_lblImportingErrTxt->setWordWrap(true);

    m_progressBar = new QProgressBar(m_importingPageWidget);
    m_progressBar->setRange(0, 100);
    m_progressBar->hide();

    vbox->addWidget(m_lblImportingTxt);
    vbox->addWidget(m_lblImportingErrTxt);
    vbox->addStretch(1);

    QWidget *options_widget = new QWidget(m_importingPageWidget);
    vbox->addWidget(options_widget);
    QVBoxLayout *options_vbox = new QVBoxLayout(options_widget);
    options_vbox->setSpacing(KDialog::spacingHint());
    m_importOptionsButton = new KPushButton(koIcon("configure"), i18n("Advanced Options"), options_widget);
    connect(m_importOptionsButton, SIGNAL(clicked()), this, SLOT(slotOptionsButtonClicked()));
    options_vbox->addWidget(m_importOptionsButton);
    options_vbox->addStretch(1);

    vbox->addWidget(m_progressBar);
    vbox->addStretch(2);
    m_importingPageWidget->show();

    m_importingPageItem = new KPageWidgetItem(m_importingPageWidget, i18n("Importing"));
    addPage(m_importingPageItem);
}

void ImportTableWizard::setupAlterTablePage()
{
    m_alterTablePageWidget = new QWidget(this);
    m_alterTablePageWidget->hide();

    QVBoxLayout *vbox = new QVBoxLayout(m_alterTablePageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);

    m_alterSchemaWidget = new KexiMigration::AlterSchemaWidget(this);
    vbox->addWidget(m_alterSchemaWidget);
    m_alterTablePageWidget->show();

    m_alterTablePageItem = new KPageWidgetItem(m_alterTablePageWidget, i18n("Alter the Detected Table Design"));
    addPage(m_alterTablePageItem);
}

void ImportTableWizard::setupFinishPage()
{
    m_finishPageWidget = new QWidget(this);
    m_finishPageWidget->hide();
    QVBoxLayout *vbox = new QVBoxLayout(m_finishPageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);
    m_finishLbl = new QLabel(m_finishPageWidget);
    m_finishLbl->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_finishLbl->setWordWrap(true);

    vbox->addWidget(m_finishLbl);
    m_finishCheckBox = new QCheckBox(i18n("Open imported table"),
                                     m_finishPageWidget);
    vbox->addSpacing(KDialog::spacingHint());
    vbox->addWidget(m_finishCheckBox);
    vbox->addStretch(1);

    m_finishPageItem = new KPageWidgetItem(m_finishPageWidget, i18n("Success"));
    addPage(m_finishPageItem);
}

void ImportTableWizard::slot_currentPageChanged(KPageWidgetItem *curPage, KPageWidgetItem *prevPage)
{
    Q_UNUSED(prevPage);
    if (curPage == m_introPageItem) {
    } else if (curPage == m_srcConnPageItem) {
        arriveSrcConnPage();
    } else if (curPage == m_srcDBPageItem) {
        arriveSrcDBPage();
    } else if (curPage == m_tablesPageItem) {
        arriveTableSelectPage(prevPage);
    } else if (curPage == m_alterTablePageItem) {
        if (prevPage == m_tablesPageItem) {
            arriveAlterTablePage();
        }
    } else if (curPage == m_importingPageItem) {
        arriveImportingPage();
    } else if (curPage == m_finishPageItem) {
        arriveFinishPage();
    }
}

void ImportTableWizard::arriveSrcConnPage()
{
    kDebug();
}

void ImportTableWizard::arriveSrcDBPage()
{
    if (fileBasedSrcSelected()) {
        //! @todo Back button doesn't work after selecting a file to import
    } else if (!m_srcDBName) {
        m_srcDBPageWidget->hide();
        kDebug() << "Looks like we need a project selector widget!";

        KexiDB::ConnectionData *condata = m_srcConnSel->selectedConnectionData();
        if (condata) {
            m_prjSet = new KexiProjectSet(condata);
            QVBoxLayout *vbox = new QVBoxLayout(m_srcDBPageWidget);
            KexiUtils::setStandardMarginsAndSpacing(vbox);
            m_srcDBName = new KexiProjectSelectorWidget(m_srcDBPageWidget, m_prjSet);
            vbox->addWidget(m_srcDBName);
            m_srcDBName->label()->setText(i18n("Select source database you wish to import:"));
        }
        m_srcDBPageWidget->show();
    }
}

void ImportTableWizard::arriveTableSelectPage(KPageWidgetItem *prevPage)
{
    if (prevPage == m_alterTablePageItem) {
        if (m_tableListWidget->count() == 1) { //we was skiping it before
            back();
        }
    } else {
        Kexi::ObjectStatus result;
        KexiUtils::WaitCursor wait;
        m_tableListWidget->clear();
        m_migrateDriver = prepareImport(result);

        if (m_migrateDriver) {
            if (!m_migrateDriver->connectSource()) {
                kWarning() << "unable to connect to database";
                return;
            }

            QStringList tableNames;
            if (m_migrateDriver->tableNames(tableNames)) {
                m_tableListWidget->addItems(tableNames);
            }
            if (m_tableListWidget->item(0)) {
                m_tableListWidget->item(0)->setSelected(true);
                if (m_tableListWidget->count() == 1) {
                    KexiUtils::removeWaitCursor();
                    next();
                }
            }
        } else {
            kWarning() << "No driver for selected source";
            QString errMessage = result.message.isEmpty() ? i18n("Unknown error") : result.message;
            QString errDescription = result.description.isEmpty() ? errMessage : result.description;
            KMessageBox::error(this, errMessage, errDescription);
            setValid(m_tablesPageItem, false);
        }
        KexiUtils::removeWaitCursor();
    }
}

void ImportTableWizard::arriveAlterTablePage()
{
//! @todo handle errors
    if (m_tableListWidget->selectedItems().isEmpty()) {
        return;
    }
//! @todo (js) support multiple tables?
#if 0
    foreach (QListWidgetItem *table, m_tableListWidget->selectedItems()) {
        m_importTableName = table->text();
    }
#else
    m_importTableName = m_tableListWidget->selectedItems().first()->text();
#endif

    KexiDB::TableSchema *ts = new KexiDB::TableSchema();
    if (!m_migrateDriver->readTableSchema(m_importTableName, *ts)) {
        delete ts;
        return;
    }

    kDebug() << ts->fieldCount();

    setValid(m_alterTablePageItem, ts->fieldCount() > 0);
    if (isValid(m_alterTablePageItem)) {
        connect(m_alterSchemaWidget->nameWidget(), SIGNAL(textChanged()), this, SLOT(slotNameChanged()), Qt::UniqueConnection);
    }

    QString baseName;
    if (fileBasedSrcSelected()) {
        baseName = QFileInfo(m_srcConnSel->selectedFileName()).baseName();
    } else {
        baseName = m_srcDBName->selectedProjectData()->databaseName();
    }

    QString suggestedCaption = i18nc("<basename-filename> <tablename>", "%1 %2", baseName, m_importTableName);
    m_alterSchemaWidget->setTableSchema(ts, suggestedCaption);

    if (!m_migrateDriver->readFromTable(m_importTableName)) {
        return;
    }

    if (!m_migrateDriver->moveFirst()) {
        back();
        KMessageBox::information(this, i18n("No data has been found in table <resource>%1</resource>. Select different table or cancel importing.",
                                            m_importTableName));
    }
    QList<KexiDB::RecordData> data;
    for (uint i = 0; i < ROWS_FOR_PREVIEW; ++i) {
        KexiDB::RecordData row;
        row.resize(ts->fieldCount());
        for (uint j = 0; j < ts->fieldCount(); ++j) {
            row[j] = m_migrateDriver->value(j);
        }
        data.append(row);
        if (!m_migrateDriver->moveNext()) { // rowCount < 3 (3 is default)
            m_alterSchemaWidget->model()->setRowCount(i + 1);
            break;
        }
    }
    m_alterSchemaWidget->setData(data);
}

void ImportTableWizard::arriveImportingPage()
{
    m_importingPageWidget->hide();
#if 0
    if (checkUserInput()) {
        //setNextEnabled(m_importingPageWidget, true);
        enableButton(KDialog::User2, true);
    } else {
        //setNextEnabled(m_importingPageWidget, false);
        enableButton(KDialog::User2, false);
    }
#endif

    QString txt;

    txt = i18nc("@info Table import wizard, final message", "<para>All required information has now been gathered. "
                "Click <interface>Next</interface> button to start importing table <resource>%1</resource>.</para>"
                "<note>Depending on size of the table this may take some time.</note>", m_alterSchemaWidget->nameWidget()->nameText());

    m_lblImportingTxt->setText(txt);

    //temp. hack for MS Access driver only
    //! @todo for other databases we will need KexiMigration::Conenction
    //! and KexiMigration::Driver classes
    bool showOptions = false;
    if (fileBasedSrcSelected()) {
        Kexi::ObjectStatus result;
        KexiMigrate *sourceDriver = prepareImport(result);
        if (sourceDriver) {
            showOptions = !result.error()
                          && sourceDriver->propertyValue("source_database_has_nonunicode_encoding").toBool();
            KexiMigration::Data *data = sourceDriver->data();
            sourceDriver->setData(0);
            delete data;
        }
    }
    if (showOptions) {
        m_importOptionsButton->show();
    } else {
        m_importOptionsButton->hide();
    }

    m_importingPageWidget->show();
}

void ImportTableWizard::arriveFinishPage()
{
    m_finishLbl->setText(i18n("Table <resource>%1</resource> has been imported.",
                              m_alterSchemaWidget->nameWidget()->nameText()));

    enableButton(KDialog::User3, false);
    enableButton(KDialog::Cancel, false);
}

bool ImportTableWizard::fileBasedSrcSelected() const
{
    return m_srcConnSel->selectedConnectionType() == KexiConnectionSelectorWidget::FileBased;
}

KexiMigrate *ImportTableWizard::prepareImport(Kexi::ObjectStatus &result)
{
    // Find a source (migration) driver name
    QString sourceDriverName;

    sourceDriverName = driverNameForSelectedSource();
    if (sourceDriverName.isEmpty())
        result.setStatus(i18n("No appropriate migration driver found."),
                         m_migrateManager->possibleProblemsInfoMsg());

    // Get a source (migration) driver
    KexiMigrate *sourceDriver = 0;
    if (!result.error()) {
        sourceDriver = m_migrateManager->driver(sourceDriverName);
        if (!sourceDriver || m_migrateManager->error()) {
            kDebug() << "Import migrate driver error...";
            result.setStatus(m_migrateManager);
        }
    }

    // Set up source (migration) data required for connection
    if (sourceDriver && !result.error()) {
#if 0
        // Setup progress feedback for the GUI
        if (sourceDriver->progressSupported()) {
            m_progressBar->updateGeometry();
            disconnect(sourceDriver, SIGNAL(progressPercent(int)),
                       this, SLOT(progressUpdated(int)));
            connect(sourceDriver, SIGNAL(progressPercent(int)),
                    this, SLOT(progressUpdated(int)));
            progressUpdated(0);
        }
#endif

        bool keepData = true;

#if 0
        if (m_importTypeStructureAndDataCheckBox->isChecked()) {
            kDebug() << "Structure and data selected";
            keepData = true;
        } else if (m_importTypeStructureOnlyCheckBox->isChecked()) {
            kDebug() << "structure only selected";
            keepData = false;
        } else {
            kDebug() << "Neither radio button is selected (not possible?) presume keep data";
            keepData = true;
        }
#endif

        KexiMigration::Data *md = new KexiMigration::Data();

        if (fileBasedSrcSelected()) {
            KexiDB::ConnectionData *conn_data = new KexiDB::ConnectionData();
            conn_data->setFileName(m_srcConnSel->selectedFileName());
            md->source = conn_data;
            md->sourceName.clear();
        } else {
            md->source = m_srcConnSel->selectedConnectionData();
            md->sourceName = m_srcDBName->selectedProjectData()->databaseName();

        }

        md->keepData = keepData;
        sourceDriver->setData(md);

        return sourceDriver;
    }
    return 0;
}

//===========================================================
//
QString ImportTableWizard::driverNameForSelectedSource()
{
    if (fileBasedSrcSelected()) {
        KMimeType::Ptr ptr = KMimeType::findByFileContent(m_srcConnSel->selectedFileName());
        if (!ptr
                || ptr.data()->name() == "application/octet-stream"
                || ptr.data()->name() == "text/plain") {
            //try by URL:
            ptr = KMimeType::findByUrl(m_srcConnSel->selectedFileName());
        }
        return ptr ? m_migrateManager->driverForMimeType(ptr.data()->name()) : QString();
    }

    return m_srcConnSel->selectedConnectionData() ? m_srcConnSel->selectedConnectionData()->driverName : QString();
}

bool ImportTableWizard::doImport()
{
    m_importComplete = true;
    KexiGUIMessageHandler msg;

    KexiProject *project = KexiMainWindowIface::global()->project();
    if (!project) {
        msg.showErrorMessage(i18n("No project available."));
        return false;
    }

    KexiPart::Part *part = Kexi::partManager().partForClass("org.kexi-project.table");
    if (!part) {
        msg.showErrorMessage(&Kexi::partManager());
        return false;
    }

    if (!m_alterSchemaWidget->newSchema()) {
        msg.showErrorMessage(i18n("No table was selected to import."));
        return false;
    }

    KexiPart::Item *partItemForSavedTable = project->createPartItem(part->info(), m_alterSchemaWidget->newSchema()->name());
    if (!partItemForSavedTable) {
        msg.showErrorMessage(project);
        return false;
    }

    //Create the table
    if (!m_currentDatabase->createTable(m_alterSchemaWidget->newSchema(), true)) {
        msg.showErrorMessage(i18n("Unable to create table <resource>%1</resource>.",
                                  m_alterSchemaWidget->newSchema()->name()));
        return false;
    }

    //Import the data
    QApplication::setOverrideCursor(Qt::BusyCursor);
    QList<QVariant> row;
    m_migrateDriver->moveFirst();
    KexiDB::TransactionGuard tg(*m_currentDatabase);
    if (m_currentDatabase->error()) {
        QApplication::restoreOverrideCursor();
        return false;
    }
    do  {
        for (unsigned int i = 0; i < m_alterSchemaWidget->newSchema()->fieldCount(); ++i) {
            row.append(m_migrateDriver->value(i));
        }
        m_currentDatabase->insertRecord(*(m_alterSchemaWidget->newSchema()), row);
        row.clear();
    } while (m_migrateDriver->moveNext());
    if (!tg.commit()) {
        QApplication::restoreOverrideCursor();
        return false;
    }
    QApplication::restoreOverrideCursor();

    //Done so save part and update gui
    partItemForSavedTable->setIdentifier(m_alterSchemaWidget->newSchema()->id());
    project->addStoredItem(part->info(), partItemForSavedTable);

    return true;
}

void ImportTableWizard::slotConnPageItemSelected(bool isSelected)
{
    setValid(m_srcConnPageItem, isSelected);
}

void ImportTableWizard::slotTableListWidgetSelectionChanged()
{
    setValid(m_tablesPageItem, !m_tableListWidget->selectedItems().isEmpty());
}

void ImportTableWizard::slotNameChanged()
{
    setValid(m_alterTablePageItem, !m_alterSchemaWidget->nameWidget()->captionText().isEmpty());
}
