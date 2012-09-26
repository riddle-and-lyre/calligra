/* This file is part of the KDE project
 * Copyright (C) 2012 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

import QtQuick 1.1
import "../../components"

Item {
    id: base
    property bool fullView: true;
    function apply() {
        toolManager.currentTool.crop();
    }
    ExpandingListView {
        id: cropTypeList;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
            margins: Constants.DefaultMargin;
        }
        currentIndex: toolManager.currentTool.cropType !== undefined ? toolManager.currentTool.cropType : 0;
        onCurrentIndexChanged: if(toolManager.currentTool && toolManager.currentTool.cropType !== undefined) toolManager.currentTool.cropType = currentIndex;
        model: ListModel {
            ListElement {
                text: "Layer";
            }
            ListElement {
                text: "Image";
            }
        }
    }
    Column {
        anchors {
            top: cropTypeList.bottom;
            left: parent.left;
            right: parent.right;
            margins: Constants.DefaultMargin;
        }
        spacing: Constants.DefaultMargin;

        RangeInput {
            id: widthInput;
            width: parent.width;
            placeholder: "Width";
            //min: 0; max: sketchView.doc.image.width(); decimals: 0;
            min: 0; max: 100000; decimals: 0;
            value: toolManager.currentTool.cropWidth !== undefined ? toolManager.currentTool.cropWidth : 0;
            onValueChanged: if(toolManager.currentTool) toolManager.currentTool.cropWidth = value;
        }
        RangeInput {
            id: heightInput;
            width: parent.width;
            placeholder: "Height";
            //min: 0; max: sketchView.doc.image.width(); decimals: 0;
            min: 0; max: 100000; decimals: 0;
            value: toolManager.currentTool.cropHeight !== undefined ? toolManager.currentTool.cropHeight : 0;
            onValueChanged: if(toolManager.currentTool) toolManager.currentTool.cropHeight = value;
        }
        RangeInput {
            width: parent.width;
            placeholder: "Ratio";
            min: 0; max: 9999; decimals: 2;
            value: toolManager.currentTool.ratio !== undefined ? toolManager.currentTool.ratio : 0;
            onValueChanged: if(toolManager.currentTool) toolManager.currentTool.ratio = value;
        }
        RangeInput {
            width: parent.width;
            placeholder: "X";
            //min: 0; max: sketchView.doc.image.width(); decimals: 0;
            min: 0; max: 100000; decimals: 0;
            value: toolManager.currentTool.cropX !== undefined ? toolManager.currentTool.cropX : 0;
            onValueChanged: if(toolManager.currentTool) toolManager.currentTool.cropX = value;
        }
        RangeInput {
            width: parent.width;
            placeholder: "Y";
            //min: 0; max: sketchView.doc.image.width(); decimals: 0;
            min: 0; max: 100000; decimals: 0;
            value: toolManager.currentTool.cropY !== undefined ? toolManager.currentTool.cropY : 0;
            onValueChanged: if(toolManager.currentTool) toolManager.currentTool.cropY = value;
        }
    }
}
