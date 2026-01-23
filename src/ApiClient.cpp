#include "ApiClient.h"
#include <QRestReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

ApiClient::ApiClient(QObject *parent)
    : QObject(parent),
    m_api(QUrl("https://api.restful-api.dev"))
{
    QHttpHeaders h;
    h.append("Accept", "application/json");
    m_api.setCommonHeaders(h);

    // Qt 6.7+ convenience: applies to future requests created by the factory
    m_api.setTransferTimeout(std::chrono::seconds(15));
}

void ApiClient::getObjects()
{
    const auto request = m_api.createRequest("objects");

    m_rest.get(request, this, [this](QRestReply &reply) {
        if (!reply.isSuccess()) {
            emit networkError(reply.errorString(), reply.httpStatus());
            return;
        }

        auto doc = reply.readJson();
        if (!doc) {
            emit networkError("Invalid JSON response", reply.httpStatus());
            return;
        }

        if (doc->isArray()) {
            emit objectsReady(doc->array().toVariantList());
        } else if (doc->isObject()) {
            emit objectsReady(doc->object().toVariantMap());
        } else {
            emit networkError("Unexpected JSON type", reply.httpStatus());
        }
    });
}
