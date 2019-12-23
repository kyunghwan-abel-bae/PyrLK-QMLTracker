import QtQuick 2.12
import QtQuick.Window 2.12
import QtMultimedia 5.12

import opencv.filter.pyrlk 1.0

Window {
    id: root

    visible: true

    title: qsTr("QML Tracker with PyrLK")

    Component.onCompleted: init()

    function init() {
        root.showMaximized()
    }

    Camera {
        id: camera

        objectName: "qrCameraQML"

        Component.onCompleted: {
            start()
        }
    }

    VideoOutput {
        id: output

        source: camera

        filters: Filter {
            id: filter
            active: true
        }

        anchors.fill: parent

        MouseArea {
            id: mr

            anchors.fill: parent

            onClicked: {
                var mX, mY

                mX = mouse.x
                mY = mouse.y

                var width_rate = output.sourceRect.width / output.width
                var height_rate = output.sourceRect.height / output.height

                var contentHeight = output.sourceRect.height
                filter.AddPoint((mX*width_rate), (contentHeight-mY*height_rate))
            }
        }
    }
}
