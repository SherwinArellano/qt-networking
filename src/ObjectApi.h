#ifndef OBJECTAPI_H
#define OBJECTAPI_H

#include <QVariantList>
#include <QVariantMap>
#include <functional>

#include "BaseApi.h"

class ObjectApi : public BaseApi
{
    Q_OBJECT

public:
    explicit ObjectApi(HttpClient* client, QObject* parent = nullptr)
        : BaseApi(client, parent) {}

    void getMany(std::function<void(const QVariantList&)> successCb, ErrorCb errorCb);
    void get(const QString& id, std::function<void(const QVariantMap&)> successCb, ErrorCb errorCb);
    void post(const QVariantMap& obj, std::function<void(const QVariantMap&)> successCb, ErrorCb errorCb);
    void put(const QString& id, const QVariantMap& obj, std::function<void(const QVariantMap&)> successCb, ErrorCb errorCb);
    void patch(const QString& id, const QVariantMap& obj, std::function<void(const QVariantMap&)> successCb, ErrorCb errorCb);
    void remove(const QString& id, std::function<void(bool)> successCb, ErrorCb errorCb);
};

#endif // OBJECTAPI_H
