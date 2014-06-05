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
import "panels"
import org.calligra 1.0
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
    ToolManager {
        id: toolManager;
        onCurrentToolChanged: console.debug("Current tool is now " + currentTool.toolId());
    }
    Loader {
        id: viewLoader;
        anchors {
            top: toolbar.top;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
    }
    Component { id: stageView; StageDocumentPage {} }
    Component { id: wordsView; WordsDocumentPage {} }
    Item {
        id: toolbar;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: Constants.ToolbarHeight;
        Rectangle {
            anchors.fill: parent;
            color: Settings.theme.color("components/toolbar/base");
            opacity: 0.96;
        }
        Button {
            id: appButton;
            height: parent.height;
            width: Constants.ToolbarHeight * 1.2790698;
            color: "#f2b200";
            image: Settings.theme.icon("krita_sketch");
        }
        Row {
            anchors {
                left: appButton.right;
                leftMargin: 20;
                verticalCenter: parent.verticalCenter;
            }
            height: Constants.ToolbarHeight - 32;
            spacing: 8;
            Button {
                height: parent.height;
                width: height * 2;
                text: "Save";
                textColor: Settings.theme.color("components/toolbar/text");
            }
            Button {
                height: parent.height;
                width: height * 2;
                text: "Undo";
                textColor: Settings.theme.color("components/toolbar/text");
            }
//             Button {
//                 height: parent.height;
//                 width: height * 2;
//                 text: "Redo";
//                 textColor: Settings.theme.color("components/toolbar/text");
//             }
            Button {
                height: parent.height;
                width: height * 1.5;
                textColor: Settings.theme.color("components/toolbar/text");
                text: "Done"
                visible: viewLoader.item ? !viewLoader.item.navigateMode : false;
                onClicked: {
                    toolManager.requestToolChange("PageToolFactory_ID");
                    viewLoader.item.navigateMode = true;
                }
            }
        }
        Row {
            id: toolbarTextTool
            anchors.centerIn: parent;
            height: Constants.ToolbarHeight - 32;
            opacity: (toolManager.currentTool !== null && toolManager.currentTool.toolId() === "TextToolFactory_ID") ? 1 : 0;
            spacing: 4;
            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
            Button {
                image: Settings.theme.icon("SVG-Icon-TextStyle-1");
                imageMargin: 2;
                height: parent.height; width: height;
                textColor: Settings.theme.color("components/toolbar/text");
                checkable: true;
                radius: 4;
                TextStylePanel {
                    document: viewLoader.item ? viewLoader.item.document : null;
                    textEditor: viewLoader.item ? viewLoader.item.textEditor : null;
                }
            }
            Rectangle { color: Settings.theme.color("components/toolbar/text"); height: parent.height; width: 1; }
            Button {
                text: "(font name)"
                height: parent.height; width: Constants.GridWidth;
                textColor: Settings.theme.color("components/toolbar/text");
            }
            Button {
                text: "(size)"
                height: parent.height; width: Constants.GridWidth / 2;
                textColor: Settings.theme.color("components/toolbar/text");
            }
            Rectangle { color: Settings.theme.color("components/toolbar/text"); height: parent.height; width: 1; }
            Button {
                image: Settings.theme.icon("SVG-Icon-Bold-1");
                imageMargin: 4;
                height: parent.height; width: height;
                textColor: Settings.theme.color("components/toolbar/text");
            }
            Button {
                image: Settings.theme.icon("SVG-Icon-Italic-1");
                imageMargin: 4;
                height: parent.height; width: height;
                textColor: Settings.theme.color("components/toolbar/text");
            }
            Button {
                image: Settings.theme.icon("SVG-Icon-Underline-1");
                imageMargin: 4;
                height: parent.height; width: height;
                textColor: Settings.theme.color("components/toolbar/text");
            }
            Rectangle { color: Settings.theme.color("components/toolbar/text"); height: parent.height; width: 1; }
            Button {
                image: Settings.theme.icon("SVG-Icon-BulletList-1");
                imageMargin: 4;
                height: parent.height; width: height;
                textColor: Settings.theme.color("components/toolbar/text");
            }
            Button {
                image: Settings.theme.icon("SVG-Icon-TextAlignment-1");
                imageMargin: 4;
                height: parent.height; width: height;
                textColor: Settings.theme.color("components/toolbar/text");
            }
        }
        Row {
            id: toolbarImageTool
            anchors.centerIn: parent;
            height: Constants.ToolbarHeight - 32;
            opacity: (toolManager.currentTool !== null && toolManager.currentTool.toolId() === "InteractionTool") ? 1 : 0;
            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
            Button {
                text: "(img)"
                height: parent.height; width: Constants.GridWidth;
                textColor: Settings.theme.color("components/toolbar/text");
            }
            Rectangle { color: Settings.theme.color("components/toolbar/text"); height: parent.height; width: 1; }
            Button {
                text: "(pencol)"
                height: parent.height; width: Constants.GridWidth;
                textColor: Settings.theme.color("components/toolbar/text");
            }
            Rectangle { color: Settings.theme.color("components/toolbar/text"); height: parent.height; width: 1; }
            Button {
                text: "(bgcol)"
                height: parent.height; width: Constants.GridWidth;
                textColor: Settings.theme.color("components/toolbar/text");
            }
            Rectangle { color: Settings.theme.color("components/toolbar/text"); height: parent.height; width: 1; }
            Button {
                text: "(line)"
                height: parent.height; width: Constants.GridWidth;
                textColor: Settings.theme.color("components/toolbar/text");
            }
            Rectangle { color: Settings.theme.color("components/toolbar/text"); height: parent.height; width: 1; }
            Button {
                text: "(shadow)"
                height: parent.height; width: Constants.GridWidth;
                textColor: Settings.theme.color("components/toolbar/text");
            }
            Rectangle { color: Settings.theme.color("components/toolbar/text"); height: parent.height; width: 1; }
            Button {
                text: "(?)"
                height: parent.height; width: Constants.GridWidth;
                textColor: Settings.theme.color("components/toolbar/text");
            }
            Rectangle { color: Settings.theme.color("components/toolbar/text"); height: parent.height; width: 1; }
            Button {
                height: parent.height; width: height;
                image: Settings.theme.icon("SVG-Icon-Options-1");
                checkable: true;
                radius: 4;
                ImageToolsPanel {}
            }
        }
        Row {
            anchors {
                right: parent.right;
                verticalCenter: parent.verticalCenter;
                rightMargin: 10
            }
            height: Constants.ToolbarHeight - 22;
            spacing: 10;
            Button {
                height: parent.height; width: height;
                image: Settings.theme.icon("SVG-Icon-AddShape-1");
            }
            Button {
                height: parent.height; width: height;
                image: Settings.theme.icon("SVG-Icon-AddNote-1");
                checkable: true;
                radius: 4;
                checkedColor: "#3C00adf5";
                NotesPanel {}
            }
            Button {
                height: parent.height;
                width: height;
                image: Settings.theme.icon("SVG-Icon-Options-1");
                checkable: true;
                radius: 4;
                checkedColor: "#3C00adf5";
                OptionsPanel {}
            }
        }
    }
}
