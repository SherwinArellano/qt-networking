#include "ObjectApi.h"
#include <QRestReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

ObjectApi::ObjectApi(HttpClient *client, QObject *parent)
    : client{client}, QObject{parent}
{}
