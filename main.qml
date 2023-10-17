import QtQuick 2.15
import QtQuick.Window 2.15
import "."
import ImageAdapter

Window {
    width: 1280 / 2;
    height: 720 / 2;
    visible: true
    title: qsTr("Hello World")
    color: "black"

    Row{
        LiveImage {
            width: 200;//1280;
            height: 200;//720;
            image: provider1.image
        }
        LiveImage {
            width: 200;//1280;
            height: 200;//720;
            image: provider2.image
        }
    }
}
