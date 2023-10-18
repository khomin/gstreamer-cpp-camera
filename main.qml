import QtQuick 2.15
import QtQuick.Window 2.15
import "."
import ImageAdapter 1.0

Window {
    id: window
    width: 1280 / 2;
    height: 720 / 2;
    visible: true
    title: qsTr("Hello World")
    color: "black"

    Row{
        LiveImage {
            width: window.width / 2
            height: window.height
            image: provider1.image
        }
        LiveImage {
            width: window.width / 2
            height: window.height
            image: provider2.image
        }
    }
}
