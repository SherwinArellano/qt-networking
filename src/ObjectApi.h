#ifndef OBJECTAPI_H
#define OBJECTAPI_H

#include <QObject>
#include <QNetworkReply>
#include <QVariantList>
#include <QRestReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <concepts>
#include <utility>

#include "HttpClient.h"

struct ErrorResult {
    int status;
    QString message;
    QNetworkReply *reply;
};

template <typename Cb>
concept ErrorCbType = std::invocable<Cb, const ErrorResult &>;

class ObjectApi : public QObject
{
    Q_OBJECT
public:
    explicit ObjectApi(HttpClient *client, QObject *parent = nullptr);

    template <typename SuccessCb, ErrorCbType ErrorCb>
    requires std::invocable<SuccessCb, const QVariantList &>
    void getMany(SuccessCb &&successCb, ErrorCb &&errorCb)
    {
        if (!client) {
            ErrorResult er{0, "HttpClient is null", nullptr};
            std::forward<ErrorCb>(errorCb)(er);
            return;
        }

        client->get("objects", [
            success = std::forward<SuccessCb>(successCb),
            error = std::forward<ErrorCb>(errorCb)
        ](QRestReply &reply) mutable -> void {
            ErrorResult er{
                reply.httpStatus(),
                reply.errorString(),
                reply.networkReply()
            };

            if (!reply.isSuccess()) {
                error(er);
                return;
            }

            auto doc = reply.readJson();
            if (!doc) {
                er.message = "Invalid JSON response";
                error(er);
                return;
            }

            if (doc->isArray()) {
                success(doc->array().toVariantList());
            } else {
                er.message = "Unexpected JSON type";
                error(er);
            }
        });
    }

private:
    HttpClient *client;
};

#endif // OBJECTAPI_H
