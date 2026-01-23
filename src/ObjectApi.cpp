#include "ObjectApi.h"
#include <QRestReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

ObjectApi::ObjectApi(HttpClient *client, QObject *parent)
    : client{client}, QObject{parent}
{}

void ObjectApi::getMany(std::function<void(const QVariantList&)> successCb, ErrorCb errorCb)
{
    if (!client) {
        if (errorCb) errorCb(ErrorResult{0, "HttpClient is null", nullptr});
        return;
    }

    client->get("objects", [
        successCb = std::move(successCb),
        errorCb = std::move(errorCb)
    ](QRestReply& reply) mutable {
        if (!reply.isSuccess()) {
            if (errorCb) {
                errorCb(ErrorResult{
                    reply.httpStatus(),
                    reply.errorString(),
                    reply.networkReply()
                });
            }
            return;
        }

        auto doc = reply.readJson();
        if (!doc || !doc->isArray()) {
            if (errorCb) {
                errorCb(ErrorResult{
                    reply.httpStatus(),
                    doc ? "Unexpected JSON type" : "Invalid JSON response",
                    reply.networkReply()
                });
            }
            return;
        }

        if (successCb) successCb(doc->array().toVariantList());
    });
}

void ObjectApi::get(const QString &id, std::function<void (const QVariantMap &)> successCb, ErrorCb errorCb)
{
    if (!client) {
        if (errorCb) errorCb(ErrorResult{0, "HttpClient is null", nullptr});
        return;
    }

    client->get("objects/" + id, [
        successCb = std::move(successCb),
        errorCb = std::move(errorCb)
    ](QRestReply& reply) mutable {
        if (!reply.isSuccess()) {
            if (errorCb) {
                errorCb(ErrorResult{
                    reply.httpStatus(),
                    reply.errorString(),
                    reply.networkReply()
                });
            }
            return;
        }

        auto doc = reply.readJson();
        if (!doc || !doc->isObject()) {
            if (errorCb) {
                errorCb(ErrorResult{
                    reply.httpStatus(),
                    doc ? "Unexpected JSON type" : "Invalid JSON response",
                    reply.networkReply()
                });
            }
            return;
        }

        if (successCb) successCb(doc->object().toVariantMap());
    });
}
