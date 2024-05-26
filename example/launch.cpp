#include "launch.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QDateTime>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

Launch::Launch() {
    imageLeft = std::make_shared<ImageProvider>();
    imageRight = std::make_shared<ImageProvider>();
}

int Launch::runLoop(int argc, char *argv[], std::function<void()> v) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) {
                QCoreApplication::exit(-1);
            }
        }, Qt::QueuedConnection);

    qmlRegisterType<LiveImage>("ImageAdapter", 1, 0, "LiveImage");
    engine.rootContext()->setContextProperty("provider1", (ImageProvider *) imageLeft.get());
    engine.rootContext()->setContextProperty("provider2", (ImageProvider *) imageRight.get());

    gst_init(NULL, NULL);
    gst_debug_set_active(TRUE);
    gst_debug_set_default_threshold(GST_LEVEL_WARNING);

    loop = g_main_loop_new(NULL, FALSE);

    auto tr = std::thread([&] {
        v();
        QCoreApplication::exit(-1);
    });
    tr.detach();
    engine.load(url);
    auto ret = app.exec();
    return ret;
}
