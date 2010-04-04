/*
 *  Copyright (c) 2009 Dmitry Kazakov <dimula73@gmail.com>
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

#include <QtGlobal>
#include "kis_memento_manager.h"
#include "kis_memento.h"


//#define DEBUG_MM

#ifdef DEBUG_MM
#define DEBUG_LOG_TILE_ACTION(action, tile, col, row)                   \
    printf("### MementoManager (0x%X): %s "             \
           "\ttile:\t0x%X (%d, %d) ###\n", (quintptr)this, action,  \
           (quintptr)tile, col, row)

#define DEBUG_LOG_SIMPLE_ACTION(action)                 \
    printf("### MementoManager (0x%X): %s\n", (quintptr)this, action)

#define DEBUG_DUMP_MESSAGE(action) do {                                 \
        printf("\n### MementoManager (0x%X): %s \t\t##########\n",  \
               (quintptr)this, action);                                 \
        debugPrintInfo();                                               \
        printf("##################################################################\n\n"); \
    } while(0)

#else

#define DEBUG_LOG_TILE_ACTION(action, tile, col, row)
#define DEBUG_LOG_SIMPLE_ACTION(action)
#define DEBUG_DUMP_MESSAGE(action)
#endif


KisMementoManager::KisMementoManager()
        : m_headsHashTable(0)
{
    /**
     * Get an initial memento to show we want history for ALL the devices.
     * If we don't do it, we won't be able to start history later,
     * as we don't have any special api for this.
     * It shouldn't create much overhead for unversioned devices as
     * no tile-duplication accurs without calling a commit()
     * method regularily.
     */
    (void) getMemento();
}

KisMementoManager::KisMementoManager(const KisMementoManager& rhs)
        : m_index(rhs.m_index),
        m_revisions(rhs.m_revisions),
        m_cancelledRevisions(rhs.m_cancelledRevisions),
        m_headsHashTable(rhs.m_headsHashTable, 0),
        m_currentMemento(rhs.m_currentMemento)
{
    Q_ASSERT_X(m_currentMemento,
               "KisMementoManager", "(impossible happened) "
               "Seems like a device was created without history support");
}

KisMementoManager::~KisMementoManager()
{
    // Nothing to be done here. Happily...
    // Everything is done by QList and KisSharedPtr...
    DEBUG_LOG_SIMPLE_ACTION("died\n");
}

/**
 *  TODO: We assume that a tile won't be COWed until commit.
 *        So to  say,  we assume  that  registerTileChange()
 *        will be called once a commit
 */
void KisMementoManager::registerTileChange(KisTile *tile)
{
    if (!m_currentMemento) return;
    m_cancelledRevisions.clear();

    DEBUG_LOG_TILE_ACTION("reg. [C]", tile, tile->col(), tile->row());

    KisMementoItemSP mi = new KisMementoItem();
    mi->changeTile(tile);
    m_index.append(mi);
    m_currentMemento->updateExtent(mi->col(), mi->row());
}

void KisMementoManager::registerTileDeleted(KisTile *tile)
{
    if (!m_currentMemento) return;
    m_cancelledRevisions.clear();

    DEBUG_LOG_TILE_ACTION("reg. [D]", tile, tile->col(), tile->row());

    KisMementoItemSP mi = new KisMementoItem();
    mi->deleteTile(tile, m_headsHashTable.defaultTileData());
    m_index.append(mi);
    m_currentMemento->updateExtent(mi->col(), mi->row());
}
void KisMementoManager::commit()
{
    if (m_index.isEmpty()) return;

    KisMementoItemSP mi;
    KisMementoItemSP parentMI;
    bool newTile;
    foreach(mi, m_index) {
        parentMI = m_headsHashTable.getTileLazy(mi->col(), mi->row(), newTile);
        if(newTile)
            parentMI->commit();

        mi->setParent(parentMI);
        mi->commit();
        m_headsHashTable.deleteTile(mi->col(), mi->row());
        m_headsHashTable.addTile(mi);
    }
    KisHistoryItem hItem;
    hItem.itemList = m_index;
    hItem.memento = m_currentMemento.data();
    
    m_revisions.append(hItem);
    m_index.clear();

    // Waking up pooler to prepare copies for us
    globalTileDataStore.kickPooler();
}

KisTileSP KisMementoManager::getCommitedTile(qint32 col, qint32 row)
{
    /**
     * Our getOldTile  mechanism is supposed to return  current tile, if
     * no history  exists. So we return  zero here if  nothing is found,
     * but data manager  will make all work on  getting current tile for
     * us
     */
    if(!m_currentMemento || !m_currentMemento->valid())
        return 0;

    KisMementoItemSP mi = m_headsHashTable.getExistedTile(col, row);
    return mi ? mi->tile(0) : 0;
}

KisMementoSP KisMementoManager::getMemento()
{
    commit();
    m_currentMemento = new KisMemento(this);

    DEBUG_DUMP_MESSAGE("GET_MEMENTO");

    return m_currentMemento;
}

#define forEachReversed(iter, list) \
        for(iter=list.end(); iter-- != list.begin();)

#define saveAndClearMemento(memento) KisMementoSP _mem = (memento); memento=0
#define restoreMemento(memento) (memento) = _mem

void KisMementoManager::rollback(KisTileHashTable *ht)
{
    commit();

    if (! m_revisions.size()) return;

    KisHistoryItem changeList = m_revisions.takeLast();

    KisMementoItemSP mi;
    KisMementoItemSP parentMI;
    KisMementoItemList::iterator iter;

    saveAndClearMemento(m_currentMemento);
    forEachReversed(iter, changeList.itemList) {
        mi=*iter;
        parentMI = mi->parent();

        if (mi->type() == KisMementoItem::CHANGED)
            ht->deleteTile(mi->col(), mi->row());
        if (parentMI->type() == KisMementoItem::CHANGED)
            ht->addTile(parentMI->tile(this));

        m_headsHashTable.deleteTile(parentMI->col(), parentMI->row());
        m_headsHashTable.addTile(parentMI);

        // This is not necessary
        //mi->setParent(0);
    }
    /**
     * FIXME: tricky hack alert.
     * We have  just deleted some tiles  from original hash  table.  And they
     * accurately reported to us about  their death, adding their bodies into
     * m_index list. We don't need them actually... :)
     *
     * PS: It could cause some race condition... But for now we can insist on
     * serialization  of rollback()/rollforward()  requests.Speaking  truly i
     * can't see sny sense in calling rollback() concurrently.
     * PPS:   We   prevented   addition   of  their   bodies   with   zeroing
     * m_currentMemento. All dead tiles are going to /dev/null :)
     */
    //m_index.clear();
    restoreMemento(m_currentMemento);

    m_cancelledRevisions.prepend(changeList);
    DEBUG_DUMP_MESSAGE("UNDONE");
}

void KisMementoManager::rollforward(KisTileHashTable *ht)
{
    commit();

    if (! m_cancelledRevisions.size()) return;

    KisHistoryItem changeList = m_cancelledRevisions.takeFirst();

    KisMementoItemSP mi;

    saveAndClearMemento(m_currentMemento);
    foreach(mi, changeList.itemList) {
        if (mi->parent()->type() == KisMementoItem::CHANGED)
            ht->deleteTile(mi->col(), mi->row());
        if (mi->type() == KisMementoItem::CHANGED)
            ht->addTile(mi->tile(this));
    }
    /**
     * FIXME: tricky hack alert.
     * We have  just deleted some tiles  from original hash  table.  And they
     * accurately reported to us about  their death, adding their bodies into
     * m_index list. We don't need them actually... :)
     *
     * PS: It could cause some race condition... But for now we can insist on
     * serialization  of rollback()/rollforward()  requests.Speaking  truly i
     * can't see sny sense in calling rollback() concurrently.
     * PPS:   We   prevented   addition   of  their   bodies   with   zeroing
     * m_currentMemento. All dead tiles are going to /dev/null :)
     */
    //m_index.clear();

    m_index = changeList.itemList;
    m_currentMemento = changeList.memento;
    commit();
    restoreMemento(m_currentMemento);
    DEBUG_DUMP_MESSAGE("REDONE");
}

void KisMementoManager::setDefaultTileData(KisTileData *defaultTileData)
{
    m_headsHashTable.setDefaultTileData(defaultTileData);
}


void KisMementoManager::debugPrintInfo()
{
    printf("KisMementoManager stats:\n");
    printf("Index list\n");
    KisHistoryItem changeList;
    KisMementoItemSP mi;
    foreach(mi, m_index) {
        mi->debugPrintInfo();
    }

    printf("Revisions list:\n");
    qint32 i = 0;
    foreach(changeList, m_revisions) {
        printf("--- revision #%d ---\n", i++);
        foreach(mi, changeList.itemList) {
            mi->debugPrintInfo();
        }
    }

    printf("\nCancelled revisions list:\n");
    i = 0;
    foreach(changeList, m_cancelledRevisions) {
        printf("--- revision #%d ---\n", m_revisions.size() + i++);
        foreach(mi, changeList.itemList) {
            mi->debugPrintInfo();
        }
    }

    printf("----------------\n");
    m_headsHashTable.debugPrintInfo();
}

void KisMementoManager::removeMemento(KisMemento* memento)
{
    if (memento == m_currentMemento) { // This happen when a memento is not put on the stack
        commit();
    }
    for (int i = 0; i < m_revisions.size(); ++i) {
        if (m_revisions[i].memento == memento) {
            Q_ASSERT(i == 0);
            m_revisions.takeAt(i);
            return;
        }
    }
}
