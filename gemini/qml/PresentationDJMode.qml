/* This file is part of the KDE project
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
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
import "components"
import org.calligra.CalligraComponents 0.1

Rectangle {
    id: base;
    //property alias canvas: presentationModel.canvas;
    color: "#22282f";
    Item {
        id: currentSlideContainer;
        anchors {
            fill: parent;
            topMargin: Constants.GridHeight + Constants.DefaultMargin;
            leftMargin: Constants.DefaultMargin;
            rightMargin: parent.width / 3;
            bottomMargin: Constants.GridHeight + Constants.DefaultMargin;
        }
        Rectangle {
            anchors.fill: parent;
            opacity: 0.6;
        }
        Thumbnail {
            anchors.fill: parent;
            content: presentationModel.thumbnail(0);
        }
    }
    Item {
        id: nextSlideContainer;
        anchors {
            top: parent.top;
            topMargin: Constants.GridHeight + Constants.DefaultMargin;
            left: currentSlideContainer.right;
            leftMargin: Constants.DefaultMargin;
            right: parent.right;
            rightMargin: Constants.DefaultMargin;
        }
        height: width * (currentSlideContainer.height / currentSlideContainer.width);
        Rectangle {
            anchors.fill: parent;
            opacity: 0.6;
        }
        Thumbnail {
            anchors.fill: parent;
            content: presentationModel.thumbnail(1);
        }
    }
    Button {
        anchors {
            top: parent.top;
            right: parent.right;
            margins: Constants.DefaultMargin;
        }
        width: Constants.GridWidth * 2
        text: "End Presentation"
        textColor: "white";
        onClicked: mainPageStack.pop();
    }
    Label {
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
            margins: Constants.DefaultMargin;
        }
        font: Settings.theme.font("pageHeader");
        color: "white";
        Timer {
            interval: 500;
            running: true;
            repeat: true;
            triggeredOnStart: true;
            onTriggered: parent.text = Qt.formatTime(new Date(), "hh:mm:ss");
        }
    }
    Row {
        anchors {
            left: parent.left;
            bottom: parent.bottom;
            margins: Constants.DefaultMargin;
        }
        height: Constants.GridHeight;
        spacing: Constants.DefaultMargin;
        Button {
            height: parent.height;
            width: height;
            text: "(T)";
        }
        Button {
            height: parent.height;
            width: height;
            text: "(L)";
        }
        Button {
            height: parent.height;
            width: height;
            text: "(H)";
        }
    }
    Row {
        anchors {
            horizontalCenter: parent.horizontalCenter;
            bottom: parent.bottom;
            margins: Constants.DefaultMargin;
        }
        height: Constants.GridHeight;
        spacing: Constants.DefaultMargin;
        Button {
            height: parent.height;
            width: height;
            image: Settings.theme.icon("back");
        }
        Button {
            height: parent.height;
            width: height;
            image: Settings.theme.icon("forward");
        }
    }
    Button {
        anchors {
            right: parent.right;
            bottom: parent.bottom;
            margins: Constants.DefaultMargin;
        }
        height: Constants.GridHeight;
        width: height;
        text: "(fx)";
    }
}
