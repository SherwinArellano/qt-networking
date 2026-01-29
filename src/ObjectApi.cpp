#include "ObjectApi.h"
#include <QJsonDocument>
#include <QJsonObject>

void ObjectApi::getMany(std::function<void(const QVariantList&)> successCb, ErrorCb errorCb)
{
    if (!ensureClient(errorCb)) return;

    client()->get("objects", [
        successCb = std::move(successCb),
        errorCb = std::move(errorCb)
    ](QRestReply& reply) mutable {
        expectArray(reply, errorCb, [&](const QJsonArray& arr) {
            if (successCb) successCb(arr.toVariantList());
        });
    });
}

void ObjectApi::get(const QString& id, std::function<void(const QVariantMap&)> successCb, ErrorCb errorCb)
{
    if (!ensureClient(errorCb)) return;

    client()->get("objects/" + id, [
        successCb = std::move(successCb),
        errorCb = std::move(errorCb)
    ](QRestReply& reply) mutable {
        expectObject(reply, errorCb, [&](const QJsonObject& obj) {
            if (successCb) successCb(obj.toVariantMap());
        });
    });
}

void ObjectApi::post(const QVariantMap& obj, std::function<void(const QVariantMap&)> successCb, ErrorCb errorCb)
{
    if (!ensureClient(errorCb)) return;

    const QJsonDocument body(QJsonObject::fromVariantMap(obj));

    client()->post("objects", body.toJson(), [
        successCb = std::move(successCb),
        errorCb = std::move(errorCb)
    ](QRestReply& reply) mutable {
        expectObject(reply, errorCb, [&](const QJsonObject& obj) {
            if (successCb) successCb(obj.toVariantMap());
        });
    });
}

void ObjectApi::put(const QString& id, const QVariantMap& obj, std::function<void(const QVariantMap&)> successCb, ErrorCb errorCb)
{
    if (!ensureClient(errorCb)) return;

    const QJsonDocument body(QJsonObject::fromVariantMap(obj));

    client()->put("objects/" + id, body.toJson(), [
        successCb = std::move(successCb),
        errorCb = std::move(errorCb)
    ](QRestReply& reply) mutable {
        expectObject(reply, errorCb, [&](const QJsonObject& obj) {
            if (successCb) successCb(obj.toVariantMap());
        });
    });
}

void ObjectApi::patch(const QString& id, const QVariantMap& obj, std::function<void(const QVariantMap&)> successCb, ErrorCb errorCb)
{
    if (!ensureClient(errorCb)) return;

    const QJsonDocument body(QJsonObject::fromVariantMap(obj));

    client()->patch("objects/" + id, body.toJson(), [
        successCb = std::move(successCb),
        errorCb = std::move(errorCb)
    ](QRestReply& reply) mutable {
        expectObject(reply, errorCb, [&](const QJsonObject& obj) {
            if (successCb) successCb(obj.toVariantMap());
        });
    });
}

void ObjectApi::remove(const QString& id, std::function<void(bool)> successCb, ErrorCb errorCb)
{
    if (!ensureClient(errorCb)) return;

    client()->remove("objects/" + id, [
        successCb = std::move(successCb),
        errorCb   = std::move(errorCb)
    ](QRestReply& reply) mutable {
        if (!reply.isSuccess()) {
            emitError(errorCb, fromReply(reply));
            return;
        }
        if (successCb) successCb(true);
    });
}
