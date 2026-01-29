#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include "src/ApiClient.h"
#include "src/HttpClient.h"
#include "src/ObjectApi.h"

static void printJson(const char* tag, const QVariantMap& obj)
{
    const QJsonDocument doc(QJsonObject::fromVariantMap(obj));
    qDebug().noquote() << tag;
    qDebug().noquote() << QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
}

static void printErr(const char* tag, const ErrorResult& er)
{
    qDebug() << tag << "[ERR]" << er.status << er.message;
}

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

    objectApi.getMany([](const QVariantList &objects) {
        qDebug() << "[GET many] count =" << objects.length();
    }, [](const ErrorResult &er) {
        printErr("[GET many]", er);
    });

    objectApi.get("7", [](const QVariantMap &object) {
        qDebug() << "[GET 7] name =" << object.value("name").toString();
    }, [](const ErrorResult &er) {
        printErr("[GET 7]", er);
    });

    QVariantMap body;
    body["name"] = "Qt Test Item";
    {
        QVariantMap data;
        data["year"] = 2026;
        data["price"] = 123.45;
        data["note"] = "created by Qt";
        body["data"] = data;
    }

    objectApi.post(body, [&objectApi](const QVariantMap& created) {
        // POST
        printJson("[POST created]", created);

        const QString id = created.value("id").toString();
        if (id.isEmpty()) {
            qDebug() << "[POST created] Missing id in response";
            return;
        }

        // PUT: full update (replace)
        QVariantMap putBody;
        putBody["name"] = "Qt Test Item (PUT)";
        {
            QVariantMap data;
            data["year"] = 2026;
            data["price"] = 999.99;
            data["cpu"] = "QtCore";
            putBody["data"] = data;
        }

        objectApi.put(id, putBody, [&objectApi, id](const QVariantMap& putObj) {
            printJson("[PUT updated]", putObj);

            // PATCH: partial update
            QVariantMap patchBody;
            patchBody["name"] = "Qt Test Item (PATCH)";

            objectApi.patch(id, patchBody, [&objectApi, id](const QVariantMap& patchObj) {
                printJson("[PATCH updated]", patchObj);

                // DELETE
                objectApi.remove(id, [id](bool ok) {
                    qDebug() << "[DELETE]" << id << "ok =" << ok;
                }, [](const ErrorResult& er) {
                    printErr("[DELETE]", er);
                });
            }, [](const ErrorResult& er) { printErr("[PATCH]", er); }
            );
        }, [](const ErrorResult& er) { printErr("[PUT]", er); }
        );
    },
    // POST error
    [](const ErrorResult& er) { printErr("[POST]", er); }
    );

    return app.exec();
}
