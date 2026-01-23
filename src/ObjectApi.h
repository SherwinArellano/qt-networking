#ifndef OBJECTAPI_H
#define OBJECTAPI_H

#include <QObject>
#include <QNetworkReply>
#include <QVariantList>
#include <QRestReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QPointer>
#include <functional>

#include "HttpClient.h"

struct ErrorResult {
    int status = 0;
    QString message;
    QPointer<QNetworkReply> reply;
};

class ObjectApi : public QObject
{
    Q_OBJECT

public:
    using ErrorCb = std::function<void(const ErrorResult&)>;

    explicit ObjectApi(HttpClient* client, QObject* parent = nullptr);

    void getMany(std::function<void(const QVariantList&)> successCb, ErrorCb errorCb);

    void get(const QString &id, std::function<void(const QVariantMap&)> successCb, ErrorCb errorCb);

private:
    HttpClient* client = nullptr;
};


#endif // OBJECTAPI_H
