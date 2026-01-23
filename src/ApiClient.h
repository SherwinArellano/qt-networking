#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequestFactory>
#include <QRestAccessManager>

class ApiClient : public QObject {
    Q_OBJECT
public:
    explicit ApiClient(QObject *parent = nullptr);

    Q_INVOKABLE void getObjects();

signals:
    void objectsReady(QVariant data);
    void networkError(QString message, int httpStatus);

private:
    QNetworkAccessManager m_nam;
    QRestAccessManager m_rest{&m_nam};
    QNetworkRequestFactory m_api;
};

#endif // APICLIENT_H
