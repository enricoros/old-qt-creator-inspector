/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

import Qt 4.6

Rectangle {
    id: itemsView

    // public
    
    function expandAll() {
        expandAllEntries();
        scrollbar.moveHandle(0, true)
    }

    signal itemSelected(int itemLibId)
    signal itemDragged(int itemLibId)

    signal stopDragAndDrop

    // internal
    
    signal expandAllEntries

    ItemsViewStyle { id: style }

    property int entriesPerRow: Math.max(1, Math.floor((itemsFlickable.width - 2) / style.cellWidth))
    property int cellWidth: Math.floor((itemsFlickable.width - 2) / entriesPerRow)
    property int cellHeight: style.cellHeight

    color: style.backgroundColor

    /* workaround: without this, a completed drag and drop operation would
                   result in the drag being continued when QmlView re-gains
                   focus */
    MouseRegion {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: if (!pressed) itemsView.stopDragAndDrop
    }

    Component {
        id: sectionDelegate

        SectionView {
            id: section

            entriesPerRow: itemsView.entriesPerRow
            cellWidth: itemsView.cellWidth
            cellHeight: itemsView.cellHeight

            width: itemsFlickable.width
            itemHighlight: selector

            onItemSelected: itemsView.itemSelected(itemLibId)
            onItemDragged: itemsView.itemDragged(itemLibId)

            Connections {
                target: itemsView
                onExpandAllEntries: section.expand();
            }
        }
    }

    Flickable {
        id: itemsFlickable

        anchors.top: parent.top
        anchors.topMargin: 2
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: scrollbar.left
        anchors.rightMargin: 2
        clip: true

        interactive: false
        contentHeight: col.height

        Column {
            id: col

            Repeater {
                model: itemLibraryModel
                delegate: sectionDelegate
            }
        }

        Selector {
            id: selector
	    
            z: -1
            flickable: itemsFlickable

            width: itemsView.cellWidth
            height: itemsView.cellHeight
        }
    }

    Scrollbar {
        id: scrollbar

        anchors.top: parent.top
        anchors.topMargin: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 2
        anchors.left: parent.right
        anchors.leftMargin: -10
        anchors.right: parent.right

        flickable: itemsFlickable
    }
}

