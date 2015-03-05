/*
 *  Copyright (c) 2014 Dmitry Kazakov <dimula73@gmail.com>
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

#include "kis_gamma_exposure_action.h"

#include <QApplication>
#include <QIcon>

#include <klocale.h>
#include <kis_canvas2.h>
#include "kis_cursor.h"
#include "KisViewManager.h"
#include "kis_input_manager.h"
#include "krita_utils.h"
#include "kis_exposure_gamma_correction_interface.h"

class KisGammaExposureAction::Private
{
public:
    Private(KisGammaExposureAction *qq) : q(qq), baseExposure(0.0), baseGamma(0.0) {}

    KisGammaExposureAction *q;
    Shortcuts mode;

    qreal baseExposure;
    qreal baseGamma;

    void addExposure(qreal diff);
    void addGamma(qreal diff);
};

void KisGammaExposureAction::Private::addExposure(qreal diff)
{
    KisExposureGammaCorrectionInterface *interface =
        q->inputManager()->canvas()->exposureGammaCorrectionInterface();

    if (!interface->canChangeExposureAndGamma()) {
        return;
    }

    interface->setCurrentExposure(interface->currentExposure() + diff);
}

void KisGammaExposureAction::Private::addGamma(qreal diff)
{
    KisExposureGammaCorrectionInterface *interface =
        q->inputManager()->canvas()->exposureGammaCorrectionInterface();

    if (!interface->canChangeExposureAndGamma()) {
        return;
    }

    interface->setCurrentGamma(interface->currentGamma() + diff);
}

KisGammaExposureAction::KisGammaExposureAction()
    : KisAbstractInputAction("Exposure or Gamma")
    , d(new Private(this))
{
    setName(i18n("Exposure and Gamma"));
    setDescription(i18n("The <i>Exposure and Gamma</i> action changes the display mode of the canvas."));

    QHash< QString, int > shortcuts;
    shortcuts.insert(i18n("Exposure Mode"), ExposureShortcut);
    shortcuts.insert(i18n("Gamma Mode"), GammaShortcut);

    shortcuts.insert(i18n("Exposure +0.5"), AddExposure05Shortcut);
    shortcuts.insert(i18n("Exposure -0.5"), RemoveExposure05Shortcut);
    shortcuts.insert(i18n("Gamma +0.5"), AddGamma05Shortcut);
    shortcuts.insert(i18n("Gamma -0.5"), RemoveGamma05Shortcut);

    shortcuts.insert(i18n("Exposure +0.2"), AddExposure02Shortcut);
    shortcuts.insert(i18n("Exposure -0.2"), RemoveExposure02Shortcut);
    shortcuts.insert(i18n("Gamma +0.2"), AddGamma02Shortcut);
    shortcuts.insert(i18n("Gamma -0.2"), RemoveGamma02Shortcut);

    shortcuts.insert(i18n("Reset Exposure and Gamma"), ResetExposureAndGammaShortcut);
    setShortcutIndexes(shortcuts);
}

KisGammaExposureAction::~KisGammaExposureAction()
{
    delete d;
}

int KisGammaExposureAction::priority() const
{
    return 5;
}

void KisGammaExposureAction::activate(int shortcut)
{
    if (shortcut == ExposureShortcut) {
        QApplication::setOverrideCursor(KisCursor::changeExposureCursor());
    } else { /* if (shortcut == GammaShortcut) */
        QApplication::setOverrideCursor(KisCursor::changeGammaCursor());
    }
}

void KisGammaExposureAction::deactivate(int shortcut)
{
    Q_UNUSED(shortcut);
    QApplication::restoreOverrideCursor();
}

void KisGammaExposureAction::begin(int shortcut, QEvent *event)
{
    KisAbstractInputAction::begin(shortcut, event);

    KisExposureGammaCorrectionInterface *interface =
        inputManager()->canvas()->exposureGammaCorrectionInterface();

    switch (shortcut) {
    case ExposureShortcut:
        d->baseExposure = interface->currentExposure();
        d->mode = (Shortcuts)shortcut;
        break;
    case GammaShortcut:
        d->baseGamma = interface->currentGamma();
        d->mode = (Shortcuts)shortcut;
        break;

    case AddExposure05Shortcut:
        d->addExposure(0.5);
        break;
    case RemoveExposure05Shortcut:
        d->addExposure(-0.5);
        break;
    case AddGamma05Shortcut:
        d->addGamma(0.5);
        break;
    case RemoveGamma05Shortcut:
        d->addGamma(-0.5);
        break;

    case AddExposure02Shortcut:
        d->addExposure(0.2);
        break;
    case RemoveExposure02Shortcut:
        d->addExposure(-0.2);
        break;
    case AddGamma02Shortcut:
        d->addGamma(0.2);
        break;
    case RemoveGamma02Shortcut:
        d->addGamma(-0.2);
        break;

    case ResetExposureAndGammaShortcut: {
        KisExposureGammaCorrectionInterface *interface =
            inputManager()->canvas()->exposureGammaCorrectionInterface();
        if (!interface->canChangeExposureAndGamma()) {
            break;
        }

        interface->setCurrentGamma(1.0);
        interface->setCurrentExposure(0.0);
        break;
    }
    }
}

void KisGammaExposureAction::mouseMoved(const QPointF &lastPos, const QPointF &pos)
{
    QPointF diff = -(pos - lastPos);

    const int step = 200;

    KisExposureGammaCorrectionInterface *interface =
        inputManager()->canvas()->exposureGammaCorrectionInterface();

    if (!interface->canChangeExposureAndGamma()) {
        return;
    }

    if (d->mode == ExposureShortcut) {
        d->baseExposure += qreal(diff.y()) / step;
        interface->setCurrentExposure(d->baseExposure);
    } else if (d->mode == GammaShortcut) {
        d->baseGamma += qreal(diff.y()) / step;
        interface->setCurrentGamma(d->baseGamma);
    }
}

bool KisGammaExposureAction::isShortcutRequired(int shortcut) const
{
    Q_UNUSED(shortcut);
    return false;
}
