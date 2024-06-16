#include "launch.h"
#include "utils/gst_static_plugins.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QDateTime>
#include <QFile>
#include <QJsonObject>
#include <QThread>
#include <QJsonDocument>

Launch::Launch() {
    imageLeft = std::make_shared<ImageProvider>();
    imageRight = std::make_shared<ImageProvider>();
}

int Launch::runLoop(int argc, char *argv[], std::function<void()> v) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    control = std::make_shared<Control>();
    const QUrl url(QStringLiteral("qrc:/main.qml"));


    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) {
                QCoreApplication::exit(-1);
            }
        }, Qt::QueuedConnection);

    qmlRegisterType<LiveImage>("ImageAdapter", 1, 0, "LiveImage");
    engine.rootContext()->setContextProperty("provider1", (ImageProvider *) imageLeft.get());
    engine.rootContext()->setContextProperty("provider2", (ImageProvider *) imageRight.get());
    engine.rootContext()->setContextProperty("control", (Control *) control.get());

    gst_init(NULL, NULL);
    gst_static_plugin_register();
    gst_debug_set_active(TRUE);
    gst_debug_set_default_threshold(GST_LEVEL_WARNING);

    loop = g_main_loop_new(NULL, FALSE);

    auto tr = QThread::create([&] {
        v();
        QCoreApplication::exit(-1);
    });
    engine.load(url);
    tr->start();
    auto ret = app.exec();
    tr->deleteLater();
    return ret;
}
