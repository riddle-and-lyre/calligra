/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006-2007 Martin Pfeiffer <hubipete@gmx.net>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef FORMULAELEMENT_H
#define FORMULAELEMENT_H

#include "BasicElement.h"
#include "kformula_export.h"

namespace FormulaShape {

/**
 * @short The element of a formula at the highest position.
 *
 * A formula consists of a tree of elements. The FormulaElement is the root of this
 * tree and therefore is the only element that doesn't have a parent element.
 * It's functionality is reduced to layouting its children in a different way. It is
 * the element with highest size and can also dictate the size to all other elements. 
 */
class KOFORMULA_EXPORT FormulaElement : public BasicElement {
public:
    /// The standard constructor
    FormulaElement();

    /**
     * Move the FormulaCursor left
     * @param cursor The FormulaCursor to be moved
     * @param from The BasicElement which was the last owner of the FormulaCursor
     */
    void moveLeft( FormulaCursor* cursor, BasicElement* from );

    /**
     * Move the FormulaCursor right 
     * @param cursor The FormulaCursor to be moved
     * @param from The BasicElement which was the last owner of the FormulaCursor
     */
    void moveRight( FormulaCursor* cursor, BasicElement* from );

    /**
     * Move the FormulaCursor up 
     * @param cursor The FormulaCursor to be moved
     * @param from The BasicElement which was the last owner of the FormulaCursor
     */
    void moveUp( FormulaCursor* cursor, BasicElement* from );

    /**
     * Move the FormulaCursor down 
     * @param cursor The FormulaCursor to be moved
     * @param from The BasicElement which was the last owner of the FormulaCursor
     */
    void moveDown( FormulaCursor* cursor, BasicElement* from );
    
    /// @return The element's ElementType
    ElementType elementType() const;

protected:
    bool readMathMLContent( const KoXmlElement& parent );
    void writeMathMLContent( KoXmlWriter* writer ) const;

private:
    QList<BasicElement*> m_childElements;
};

} // namespace FormulaShape

#endif // FORMULAELEMENT_H
