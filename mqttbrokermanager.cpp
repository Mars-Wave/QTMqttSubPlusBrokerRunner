#include "mqttbrokermanager.h"
#include <QDateTime>
#include <QDebug>

MqttBrokerManager::MqttBrokerManager(QObject* parent)
    : QObject(parent)
    , m_brokerProcess(new QProcess(this))
    , m_mqttClient(new QMqttClient(this))
    , m_subscription(nullptr)
    , m_isRunning(false)
    , m_brokerExecutablePath("nanoMQ_attempt.exe") // Default value
    , m_subscriptionTopic("#") // Default to all topics
    , m_viewLocked(false)
{
    m_mqttClient->setHostname("localhost");
    m_mqttClient->setPort(1883);
    m_mqttClient->setUsername("user");
    m_mqttClient->setPassword("pass");

    connect(m_brokerProcess,
            &QProcess::stateChanged,
            this,
            &MqttBrokerManager::onBrokerProcessStateChanged);
    connect(m_brokerProcess,
            &QProcess::readyReadStandardOutput,
            this,
            &MqttBrokerManager::onBrokerStandardOutput);
    connect(m_brokerProcess,
            &QProcess::readyReadStandardError,
            this,
            &MqttBrokerManager::onBrokerStandardError);

    connect(m_mqttClient, &QMqttClient::connected, this, &MqttBrokerManager::onMqttConnected);
    connect(m_mqttClient, &QMqttClient::disconnected, this, &MqttBrokerManager::onMqttDisconnected);
    connect(m_mqttClient,
            &QMqttClient::messageReceived,
            this,
            &MqttBrokerManager::onMqttMessageReceived);
    connect(m_mqttClient, &QMqttClient::errorChanged, this, &MqttBrokerManager::onMqttError);

    // Setup connection timer to allow broker to start before connecting
    m_connectionTimer.setSingleShot(true);
    m_connectionTimer.setInterval(1000); // 1 second delay
    connect(&m_connectionTimer, &QTimer::timeout, this, &MqttBrokerManager::connectToMqtt);
}

void MqttBrokerManager::clearHistory()
{
    m_messageHistory.clear();
    emit messageHistoryChanged();
}



void MqttBrokerManager::setViewLocked(bool locked)
{
    if (m_viewLocked != locked) {
        m_viewLocked = locked;
        emit viewLockChanged();
        
        if (!m_viewLocked && !m_bufferedMessages.isEmpty()) {
            applyBufferedMessages();
        }
    }
}



void MqttBrokerManager::applyBufferedMessages()
{
    if (m_bufferedMessages.isEmpty()) {
        return;
    }
    
    // Prepend all buffered messages to the message history
    for (const QVariant& msg : m_bufferedMessages) {
        m_messageHistory.prepend(msg);
    }
    
    // Clear the buffer
    m_bufferedMessages.clear();
    emit bufferedMessageCountChanged();
    emit messageHistoryChanged();
}

void MqttBrokerManager::addToMessageHistory(const QString& topic, const QString& message)
{
    MessageData* messageData = new MessageData(topic, message, QDateTime::currentDateTime(), this);
    QVariant msgVariant = QVariant::fromValue(messageData);
    
    if (m_viewLocked) {
        // In locked mode, add to buffer instead
        m_bufferedMessages.prepend(msgVariant);
        emit bufferedMessageCountChanged();
    } else {
        // Normal mode, add to message history
        m_messageHistory.prepend(msgVariant);
        emit messageHistoryChanged();

        // Keep history at reasonable size
        const int MAX_HISTORY = 1000;
        if (m_messageHistory.size() > MAX_HISTORY) {
            m_messageHistory.removeLast();
            emit messageHistoryChanged();
        }
    }
}

void MqttBrokerManager::setBrokerExecutablePath(const QString &path)
{
    if (m_brokerExecutablePath != path) {
        m_brokerExecutablePath = path;
        emit brokerExecutablePathChanged();
        addSystemTraceMessage(QString("Broker executable changed to: %1").arg(path));
    }
}

void MqttBrokerManager::setSubscriptionTopic(const QString &topic)
{
    if (m_subscriptionTopic != topic) {
        m_subscriptionTopic = topic;
        emit subscriptionTopicChanged();
        addSystemTraceMessage(QString("Subscription topic changed to: %1").arg(topic));
        
        // If already connected, update the subscription
        if (m_mqttClient->state() == QMqttClient::Connected) {
            updateSubscription();
        }
    }
}

void MqttBrokerManager::updateSubscription()
{
    // If we already have a subscription, unsubscribe first
    if (m_subscription) {
        m_subscription->unsubscribe();
        m_subscription = nullptr;
    }
    
    // Create new subscription with current topic
    if (m_mqttClient->state() == QMqttClient::Connected) {
        m_subscription = m_mqttClient->subscribe(QMqttTopicFilter(m_subscriptionTopic), 0);
        
        if (m_subscription) {
            addSystemTraceMessage(QString("Subscribed to MQTT topic: %1").arg(m_subscriptionTopic));
        } else {
            addSystemTraceMessage(QString("Failed to subscribe to MQTT topic: %1").arg(m_subscriptionTopic));
        }
    }
}

void MqttBrokerManager::startBroker()
{
    addSystemTraceMessage(QString("Starting MQTT broker using: %1").arg(m_brokerExecutablePath));

    m_brokerProcess->start(m_brokerExecutablePath);

    // Wait before attempting to connect to allow broker startup
    m_connectionTimer.start();
}

void MqttBrokerManager::connectToMqtt()
{
    addSystemTraceMessage("Connecting to MQTT broker");
    m_mqttClient->connectToHost();
}

void MqttBrokerManager::stopBroker()
{
    addSystemTraceMessage("Stopping MQTT broker");

    // First disconnect MQTT client
    disconnectFromMqtt();

    // Then terminate the broker process
    if (m_brokerProcess->state() != QProcess::NotRunning) {
        m_brokerProcess->terminate();
        if (!m_brokerProcess->waitForFinished(3000)) { // Wait 3 seconds
            m_brokerProcess->kill();
        }
    }

    m_isRunning = false;
    emit runningStatusChanged();
}

void MqttBrokerManager::disconnectFromMqtt()
{
    addSystemTraceMessage("Disconnecting from MQTT...");

    if (m_subscription) {
        m_subscription->unsubscribe();
        m_subscription = nullptr;
    }

    if (m_mqttClient->state() == QMqttClient::Connected) {
        m_mqttClient->disconnectFromHost();
    }
}

void MqttBrokerManager::onBrokerProcessStateChanged(QProcess::ProcessState newState)
{
    QString stateStr;
    switch (newState) {
    case QProcess::NotRunning:
        stateStr = "Not Running";
        break;
    case QProcess::Starting:
        stateStr = "Starting";
        break;
    case QProcess::Running:
        stateStr = "Running";
        break;
    }

    addSystemTraceMessage(QString("Broker process state: %1").arg(stateStr));
}

void MqttBrokerManager::onBrokerStandardOutput()
{
    QByteArray output = m_brokerProcess->readAllStandardOutput();
    QString outputStr = QString::fromLocal8Bit(output).trimmed();

    if (!outputStr.isEmpty()) {
        QStringList lines = outputStr.split("\n");
        for (const QString& line : lines) {
            if (!line.trimmed().isEmpty()) {
                addSystemTraceMessage(QString("Broker: %1").arg(line));
            }
        }
    }
}

void MqttBrokerManager::onBrokerStandardError()
{
    QByteArray error = m_brokerProcess->readAllStandardError();
    QString errorStr = QString::fromLocal8Bit(error).trimmed();

    if (!errorStr.isEmpty()) {
        QStringList lines = errorStr.split("\n");
        for (const QString& line : lines) {
            if (!line.trimmed().isEmpty()) {
                addSystemTraceMessage(QString("Broker ERROR: %1").arg(line));
            }
        }
    }
}

void MqttBrokerManager::onMqttConnected()
{
    addSystemTraceMessage("MQTT client connected to broker");

    // Subscribe to the configured topic
    m_subscription = m_mqttClient->subscribe(QMqttTopicFilter(m_subscriptionTopic), 0);

    if (!m_subscription) {
        addSystemTraceMessage(QString("Failed to subscribe to MQTT topic: %1").arg(m_subscriptionTopic));
        return;
    }

    addSystemTraceMessage(QString("Subscribed to MQTT topic: %1").arg(m_subscriptionTopic));

    m_isRunning = true;
    emit runningStatusChanged();
}

void MqttBrokerManager::onMqttDisconnected()
{
    addSystemTraceMessage("MQTT client disconnected from broker");
    m_subscription = nullptr;
}

void MqttBrokerManager::onMqttMessageReceived(const QByteArray& message, const QMqttTopicName& topic)
{
    QString messageStr = QString::fromUtf8(message);
    QString topicStr = topic.name();

    qDebug() << "Received message on topic:" << topicStr << "Message:" << messageStr;

    addToMessageHistory(topicStr, messageStr);
    emit messageReceived(topicStr, messageStr);
}
