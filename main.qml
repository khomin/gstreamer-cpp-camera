import QtQuick 2.15
import QtQuick.Window 2.15
import "."
import MyApp.Images

Window {
    width: 1280 / 2;
    height: 720 / 2;
    visible: true
    title: qsTr("Hello World")
    color: "black"

    Row{
        LiveImage {
            width: 1280;
            height: 720;
            x: 0
            y: 0
            image: LiveImageProvider.image
        }
    }
}
