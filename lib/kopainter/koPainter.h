/* This file is part of the KDE project
  Copyright (c) 2002 Igor Janssen (rm@kde.org)

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

#ifndef __ko_painter_h__
#define __ko_painter_h__

#define LIBART_COMPILATION

#include <koColor.h>
#include <support/art_vpath.h>

class QWidget;
class KoVectorPath;
class KoOutline;
class KoFill;
class QColor;

class KoPainter
{
public:
  KoPainter(QWidget *aWidget, int w, int h);
  KoPainter(QImage *aBuffer);
  ~KoPainter();

  QImage *image() const {return mBuffer; }

  void outline(KoOutline *aOutline);
  void fill(KoFill *aFill);

  void resize(const int w, const int h);
  void resize(const QSize &size);

  void fillAreaRGB(const QRect &r, const KoColor &c);
  void drawRectRGB(const QRect &r, const KoColor &c);
  void drawHorizLineRGB(const int x1, const int x2, const int y, const QColor &c);
  void drawVertLineRGB(const int x, const int y1, const int y2, const QColor &c);

  void drawVectorPathOutline(KoVectorPath *vp);
  void drawVectorPathFill(KoVectorPath *vp);
  void drawVectorPath(KoVectorPath *vp);
  void drawImage(QImage *img, int alpha, QWMatrix &m);

  void blit();

private:
  void memset(QRgb *p, int n, QRgb c);

private:
  QWidget       *mWidget;
  QImage        *mBuffer;
  int            mWidth;
  int            mHeight;
  bool           mDeleteBuffer;

  KoOutline     *mOutline;
  KoFill        *mFill;
};

#endif
