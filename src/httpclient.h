#pragma once

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkRequestFactory>
#include <QRestAccessManager>
#include <QRestReply>
#include <QTimer>
#include <QUrl>
#include <QDebug>
#include <concepts>
#include <functional>

struct RetryPolicy {
    int maxAttempts = 1; // 1 = no retry
    int baseDelayMs = 200;
    double multiplier = 2.0;
    int maxDelayMs = 5000;

    bool retryOnNetworkError = true; // e.g. httpStatus <= 0
    QList<int> retryHttpStatus = { 408, 429, 500, 502, 503, 504 };

    std::function<bool(const QRestReply&)> shouldRetry = {}; // optional override
};

class RequestHandle : public QObject {
    Q_OBJECT
public:
    explicit RequestHandle(QObject* parent = nullptr) : QObject(parent) {}
    Q_INVOKABLE void abort() { m_aborted = true; }
    bool aborted() const { return m_aborted; }

signals:
    void attempt(int n);
    void finished(QRestReply &reply);
    void failed(QString message, int httpStatus);

private:
    friend class HttpClient;
    bool m_aborted = false;
};

class HttpClient : public QObject
{
    Q_OBJECT

public:
    explicit HttpClient(const QUrl& baseUrl = {}, QObject *parent = nullptr);

    void setBaseUrl(const QUrl& baseUrl);
    void setBearerToken(const QByteArray& token);
    void clearBearerToken();

    QRestAccessManager& rest() { return m_rest; }
    QNetworkRequestFactory& factory() { return m_factory; }

    template<typename Functor>
    requires std::invocable<Functor, QRestReply &>
    RequestHandle* get(const QString& urlOrPath, Functor&& callback)
    {
        // Since Qt's MOC doesn't like having a default parameter with a
        // RetryPolicy initialization, we use function overloading instead
        // I.e. Qt complains on `RetryPolicy policy = {}` as default parameter
        return get(urlOrPath, std::forward<Functor>(callback), RetryPolicy{});
    }

    template<typename Functor>
    requires std::invocable<Functor, QRestReply &>
    RequestHandle* get(const QString& urlOrPath, Functor&& callback, RetryPolicy policy)
    {
        auto* handle = new RequestHandle(this);

        auto doAttempt = [
            this,
            handle,
            urlOrPath,
            cb = std::forward<Functor>(callback),
            &policy
        ](auto&& self, int attemptNo) mutable -> void {
            if (handle->aborted()) return;
            emit handle->attempt(attemptNo);

            const QNetworkRequest req = buildRequest(urlOrPath);
            const QUrl url = req.url();
            if (!url.isValid()) {
                emit handle->failed("Invalid URL", 0);
                return;
            }

            qDebug() << QStringLiteral("[NETWORK] Fetch (%1): %2").arg(attemptNo).arg(req.url().toString()).toStdString();

            m_rest.get(req, handle, [this, handle, cb, policy, attemptNo, self](QRestReply &reply) mutable {
               if (handle->aborted()) return;

               if (reply.isSuccess()) {
                   emit handle->finished(reply);
                   cb(reply);
                   return;
               }

               const bool willRetry = shouldRetry(reply, policy, attemptNo);
               if (!willRetry) {
                   emit networkError(reply.errorString(), reply.httpStatus());
                   emit handle->failed(reply.errorString(), reply.httpStatus());
                   cb(reply); // invoke callback on failure
                   return;
               }

               const int delay = retryDelayMs(policy, attemptNo);
               QTimer::singleShot(delay, handle, [handle, self, attemptNo]() mutable {
                   if (handle->aborted()) return;
                   self(self, attemptNo + 1);
               });
            });
        };

        doAttempt(doAttempt, 1);
        return handle;
    }

signals:
    void networkError(QString message, int httpStatus);

private:
    QNetworkRequest buildRequest(const QString& urlOrPath) const;

    static int retryDelayMs(const RetryPolicy& policy, int attemptNo);
    bool shouldRetry(const QRestReply& reply, const RetryPolicy& policy, int attemptNo) const;

private:
    QNetworkAccessManager m_nam;
    QRestAccessManager m_rest;
    QNetworkRequestFactory m_factory;
};
