/*
 *  Copyright (c) 2008 Cyrille Berger <cberger@cberger.net>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _KIS_SPECIFIC_COLOR_SELECTOR_WIDGET_H_
#define _KIS_SPECIFIC_COLOR_SELECTOR_WIDGET_H_

#include <QWidget>

#include <KoColor.h>

class KoColorSpace;
class KoColorDisplayRendererInterface;
class QVBoxLayout;
class KisColorInput;
class KisColorSpaceSelector;
class QCheckBox;
class KisSignalCompressor;

class KisSpecificColorSelectorWidget : public QWidget
{
    Q_OBJECT
public:
    KisSpecificColorSelectorWidget(QWidget *parent);
    ~KisSpecificColorSelectorWidget();
    bool customColorSpaceUsed();
public Q_SLOTS:
    void setDisplayRenderer(KoColorDisplayRendererInterface *displayRenderer);
    void setColorSpace(const KoColorSpace *);
    void setColor(const KoColor &);
private Q_SLOTS:
    void update();
    void updateTimeout();
    void setCustomColorSpace(const KoColorSpace *);
Q_SIGNALS:
    void colorChanged(const KoColor &);
    void updated();
private:
    QList<KisColorInput *> m_inputs;
    const KoColorSpace *m_colorSpace;
    QVBoxLayout *m_layout;
    KoColor m_color;
    bool m_updateAllowed;
    KisSignalCompressor *m_updateCompressor;
    KisColorSpaceSelector *m_colorspaceSelector;
    bool m_customColorSpaceSelected;
    QCheckBox *m_chkShowColorSpaceSelector;
    KoColorDisplayRendererInterface *m_displayRenderer;
    KoColorDisplayRendererInterface *m_fallbackRenderer;
};

#endif
