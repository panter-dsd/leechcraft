import QtQuick 1.0

Rectangle {
    id: rootRect
    anchors.fill: parent
    focus: true
    color: "#e0000000"

    signal closeRequested()
    signal itemSelected(string id)
    signal categorySelected(int index)

    Keys.onEscapePressed: rootRect.closeRequested()

    Rectangle {
        id: catsContainer

        color: "#99222222"
        width: 200

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        ListView {
            id: catsView
            anchors.fill: parent
            anchors.topMargin: 5
            currentIndex: -1

            highlight: Rectangle {
                width: catsView.width
                height: 30

                color: "#A51E00"
                radius: 5
                y: catsView.currentItem.y

                Behavior on y {
                    PropertyAnimation {}
                }
            }

            highlightFollowsCurrentItem: false

            model: catsModel

            delegate: Rectangle {
                id: catsViewDelegate

                width: catsView.width
                height: 30
                radius: 5

                color: (index != catsView.currentIndex && categoryMouseArea.containsMouse) ? "#aa000000" : "#00000000"
                Behavior on color { PropertyAnimation {} }

                Image {
                    id: categoryIconImage
                    width: 24
                    height: 24
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 5

                    source: "image://appicon/" + categoryIcon
                    smooth: true
                }

                Text {
                    text: categoryName

                    font.italic: true
                    font.pointSize: 12
                    color: "#cccccc"

                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: categoryIconImage.right
                    anchors.leftMargin: 5
                }

                MouseArea {
                    id: categoryMouseArea
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        appsFilterInput.text = "";
                        rootRect.categorySelected(index);
                    }
                }
            }
        }
    }

    Rectangle {
        id: appsFilterInputContainer

        anchors.left: catsContainer.right
        anchors.leftMargin: 5
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.right: parent.right
        height: 22

        color: "#cccccccc"

        TextEdit {
            id: appsFilterInput
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 4
            font.pointSize: 12
            font.italic: true
            textFormat: TextEdit.PlainText
        }

        Binding {
            target: itemsModelFilter
            property: "appFilterText"
            value: appsFilterInput.text
        }
    }

    GridView {
        id: itemsView
        anchors.left: catsContainer.right
        anchors.top: appsFilterInputContainer.bottom
        anchors.bottom: itemDescriptionLabel.top
        anchors.right: parent.right
        anchors.margins: 5

        cellWidth: 192
        cellHeight: 160

        model: itemsModel

        delegate: Rectangle {
            id: itemsViewDelegate

            width: itemsView.cellWidth
            height: itemsView.cellHeight
            radius: 5

            color: itemMouseArea.containsMouse ? "#AAA51E00" : "#33222222"
            Behavior on color { PropertyAnimation {} }

            Image {
                width: 96
                height: 96
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 16

                source: "image://appicon/" + itemIcon
                smooth: true
            }

            Text {
                text: itemName

                width: parent.width
                font.pointSize: 10
                color: "#eeeeee"

                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 16

                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
            }

            MouseArea {
                id: itemMouseArea
                anchors.fill: parent
                hoverEnabled: true

                onClicked: rootRect.itemSelected(itemID)

                onEntered: itemDescriptionLabel.text = itemDescription
                onExited: itemDescriptionLabel.text = ""
            }
        }
    }

    Text {
        id: itemDescriptionLabel

        height: 16

        color: "#aaaaaa"

        anchors.left: catsContainer.right
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        clip: true
    }
}