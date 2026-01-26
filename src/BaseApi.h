#pragma once

#include <QObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRestReply>

#include "ApiTypes.h"
#include "HttpClient.h"

class BaseApi : public QObject
{
    Q_OBJECT

public:
    explicit BaseApi(HttpClient* client, QObject* parent = nullptr)
        : QObject(parent), m_client(client) {}

protected:
    HttpClient* client() const { return m_client; }

    bool ensureClient(ErrorCb& errorCb) const
    {
        if (m_client) return true;
        emitError(errorCb, ErrorResult{0, "HttpClient is null", nullptr});
        return false;
    }

    static void emitError(ErrorCb& errorCb, const ErrorResult& err)
    {
        if (errorCb) errorCb(err);
    }

    static ErrorResult fromReply(QRestReply& reply, const QString& messageOverride = {})
    {
        return ErrorResult{
            reply.httpStatus(),
            messageOverride.isEmpty() ? reply.errorString() : messageOverride,
            reply.networkReply()
        };
    }

    template <typename Fn>
    static void withJson(QRestReply& reply, ErrorCb& errorCb, Fn&& fn)
    {
        if (!reply.isSuccess()) {
            emitError(errorCb, fromReply(reply));
            return;
        }

        auto doc = reply.readJson();
        if (!doc) {
            emitError(errorCb, fromReply(reply, "Invalid JSON response"));
            return;
        }

        fn(*doc);
    }

    template <typename Fn>
    static void expectArray(QRestReply& reply, ErrorCb& errorCb, Fn&& fn)
    {
        withJson(reply, errorCb, [&](const QJsonDocument& doc) {
            if (!doc.isArray()) {
                emitError(errorCb, fromReply(reply, "Unexpected JSON type"));
                return;
            }
            fn(doc.array());
        });
    }

    template <typename Fn>
    static void expectObject(QRestReply& reply, ErrorCb& errorCb, Fn&& fn)
    {
        withJson(reply, errorCb, [&](const QJsonDocument& doc) {
            if (!doc.isObject()) {
                emitError(errorCb, fromReply(reply, "Unexpected JSON type"));
                return;
            }
            fn(doc.object());
        });
    }

private:
    HttpClient* m_client = nullptr;
};
