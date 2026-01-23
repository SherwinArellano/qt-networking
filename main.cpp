#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include "src/ApiClient.h"
#include "src/HttpClient.h"
#include "src/ObjectApi.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);


    ApiClient api;
    engine.rootContext()->setContextProperty("api", &api);

    engine.loadFromModule("Networking", "Main");

    HttpClient httpClient{QUrl("https://api.restful-api.dev")};
    ObjectApi objectApi{&httpClient};

    objectApi.getMany([](const QVariantList& objects) {
        qDebug() << "Objects loaded from source:" << objects.length();
    }, [](const ErrorResult& er) {
        qDebug() << "[ERR] Objects not loaded:" << er.status << er.message;
    });

    return app.exec();
}
