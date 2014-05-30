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
import org.calligra.CalligraComponents 0.1 as Calligra

Page {
    id: base;
    property string pageName: "MainPage";
    Connections {
        target: Settings;
        onCurrentFileChanged: {
            if(Settings.currentFileClass === WORDS_MIME_TYPE) {
                viewLoader.sourceComponent = wordsView;
            } else if(Settings.currentFileClass === STAGE_MIME_TYPE) {
                viewLoader.sourceComponent = stageView;
            } else {
                console.debug("BANG!");
            }
            viewLoader.item.source = Settings.currentFile;
        }
    }
    Loader {
        id: viewLoader;
        anchors {
            top: toolbar.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
    }
    Component { id: stageView; StageDocumentPage {} }
    Component { id: wordsView; WordsDocumentPage {} }
    Rectangle {
        id: toolbar;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: Constants.GridHeight * 2 / 3;
        color: "whitesmoke";
        Row {
            anchors.left: parent.left;
            height: parent.height;
            spacing: Constants.DefaultMargin;
            Button {
                height: parent.height;
                width: height * 1.5;
                color: "dodgerblue";
                image: Settings.theme.icon("krita_sketch");
            }
            Button {
                height: parent.height;
                width: height * 2;
                text: "Undo";
                textColor: "black";
            }
            Button {
                height: parent.height;
                width: height * 2;
                text: "Redo";
                textColor: "black";
            }
        }
        Row {
            anchors.centerIn: parent;
            height: parent.height;
        }
        Row {
            anchors.right: parent.right;
            height: parent.height;
            spacing: Constants.DefaultMargin;
            Button {
                height: parent.height; width: height;
                image: Settings.theme.icon("add");
                color: "gray";
            }
            Button {
                height: parent.height; width: height;
                image: Settings.theme.icon("paint");
                color: "gray";
            }
            Button {
                opacity: switchToDesktopAction.enabled ? 1 : 0.2;
                Behavior on opacity { PropertyAnimation { duration: 200; } }
                height: parent.height;
                width: height;
                image: Settings.theme.icon("switch");
                color: "gray";
                onClicked: switchToDesktopAction.trigger();
            }
        }
    }
}
