#ifndef MQTTBROKERMANAGER_H
#define MQTTBROKERMANAGER_H

#include <QObject>
#include <QProcess>
#include <QtMqtt/QMqttClient>
#include <QtMqtt/QMqttSubscription>
#include <QTimer>
#include <QVariantList>
#include "messagedata.h"

class MqttBrokerManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY runningStatusChanged)
    Q_PROPERTY(QVariantList messageHistory READ messageHistory NOTIFY messageHistoryChanged)
    Q_PROPERTY(QString brokerExecutablePath READ brokerExecutablePath WRITE setBrokerExecutablePath NOTIFY brokerExecutablePathChanged)
    Q_PROPERTY(QString subscriptionTopic READ subscriptionTopic WRITE setSubscriptionTopic NOTIFY subscriptionTopicChanged)
    Q_PROPERTY(bool isViewLocked READ isViewLocked WRITE setViewLocked NOTIFY viewLockChanged)
    Q_PROPERTY(int bufferedMessageCount READ bufferedMessageCount NOTIFY bufferedMessageCountChanged)

public:
    explicit MqttBrokerManager(QObject *parent = nullptr);
    ~MqttBrokerManager();

    bool isRunning() const;
    QVariantList messageHistory() const;
    QString brokerExecutablePath() const;
    QString subscriptionTopic() const;
    bool isViewLocked() const;
    int bufferedMessageCount() const;

public slots:
    void toggleBroker();
    void clearHistory();
    QString formatTimestamp(const QDateTime &timestamp) const;
    void setBrokerExecutablePath(const QString &path);
    void setSubscriptionTopic(const QString &topic);
    void updateSubscription();
    void setViewLocked(bool locked);
    void toggleViewLock();
    void applyBufferedMessages();
    
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
    void onMqttError(QMqttClient::ClientError error);

private:
    void startBroker();
    void stopBroker();
    void connectToMqtt();
    void disconnectFromMqtt();
    void addToMessageHistory(const QString &topic, const QString &message);
    void addSystemTraceMessage(const QString &message);

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
