#ifndef MQTTBROKERMANAGER_H
#define MQTTBROKERMANAGER_H

#include <QObject>
#include <QProcess>
#include <QtMqtt/QMqttClient>
#include <QtMqtt/QMqttSubscription>
#include <QTimer>
#include <QVariantList>
#include "messagedata.h"

namespace {

QString mqttErrorToErrorStr(QMqttClient::ClientError err)
{
    qDebug() << "MQTT client error:" << err;
    switch (err) {
    case QMqttClient::NoError:
        return "No Error";
    case QMqttClient::InvalidProtocolVersion:
        return "Invalid Protocol Version";
    case QMqttClient::IdRejected:
        return "ID Rejected";
    case QMqttClient::ServerUnavailable:
        return "Server Unavailable";
    case QMqttClient::BadUsernameOrPassword:
        return "Bad Username or Password";
    case QMqttClient::NotAuthorized:
        return "Not Authorized";
    case QMqttClient::TransportInvalid:
        return "Transport Invalid";
    case QMqttClient::ProtocolViolation:
        return "Protocol Violation";
    case QMqttClient::UnknownError:
        return "Unknown Error";
    default:
        return QString("Error code: %1").arg(err);
    }
}
} //namespace

class MqttBrokerManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY runningStatusChanged)
    Q_PROPERTY(QVariantList messageHistory READ messageHistory NOTIFY messageHistoryChanged)
    Q_PROPERTY(QString brokerExecutablePath READ brokerExecutablePath WRITE setBrokerExecutablePath NOTIFY brokerExecutablePathChanged)
    Q_PROPERTY(QString subscriptionTopic READ subscriptionTopic WRITE setSubscriptionTopic NOTIFY subscriptionTopicChanged)
    Q_PROPERTY(bool isViewLocked READ isViewLocked NOTIFY viewLockChanged)
    Q_PROPERTY(int bufferedMessageCount READ bufferedMessageCount NOTIFY bufferedMessageCountChanged)

public:
    explicit MqttBrokerManager(QObject *parent = nullptr);
    ~MqttBrokerManager() { stopBroker(); }

    bool         inline isRunning()            const { return m_isRunning; }
    QVariantList inline messageHistory()       const { return m_messageHistory; }
    QString      inline brokerExecutablePath() const { return m_brokerExecutablePath; }
    QString      inline subscriptionTopic()    const { return m_subscriptionTopic; }
    bool         inline isViewLocked()         const { return m_viewLocked; }
    int          inline bufferedMessageCount() const { return m_bufferedMessages.size(); }
    
    // Added Q_INVOKABLE to make this callable from QML
    Q_INVOKABLE inline QString formatTimestamp(const QDateTime &timestamp) const { 
        return timestamp.toString("yyyy-MM-dd HH:mm:ss.zzz"); 
    }

public slots:
    void clearHistory();
    void setBrokerExecutablePath(const QString &path);
    void setSubscriptionTopic(const QString &topic);
    void updateSubscription();
    void applyBufferedMessages();

    void inline toggleBroker()   { m_isRunning ? stopBroker() : startBroker(); }
    void inline toggleViewLock() { setViewLocked(!m_viewLocked); }
    
signals:
    void runningStatusChanged();
    void messageHistoryChanged();
    void messageReceived(const QString &topic, const QString &message);
    void brokerExecutablePathChanged();
    void subscriptionTopicChanged();
    void viewLockChanged();
    void bufferedMessageCountChanged();

private slots:
    void onBrokerProcessStateChanged(QProcess::ProcessState newState);
    void onBrokerStandardOutput();
    void onBrokerStandardError();
    void onMqttConnected();
    void onMqttDisconnected();
    void onMqttMessageReceived(const QByteArray &message, const QMqttTopicName &topic);
    void inline onMqttError(QMqttClient::ClientError error) { addSystemTraceMessage(QString("MQTT Error: %1").arg(mqttErrorToErrorStr(error))); }

private:

    void setViewLocked(bool locked);
    void startBroker();
    void stopBroker();
    void connectToMqtt();
    void disconnectFromMqtt();
    void addToMessageHistory(const QString &topic, const QString &message);
    void inline addSystemTraceMessage(const QString &message) { addToMessageHistory("system_trace", message); }

    QProcess *m_brokerProcess;
    QMqttClient *m_mqttClient;
    QMqttSubscription *m_subscription;
    bool m_isRunning;
    QVariantList m_messageHistory;
    QTimer m_connectionTimer;
    QString m_brokerExecutablePath;
    QString m_subscriptionTopic;
    bool m_viewLocked;
    QVariantList m_bufferedMessages;
};

#endif // MQTTBROKERMANAGER_H
