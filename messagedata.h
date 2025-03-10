#ifndef MESSAGEDATA_H
#define MESSAGEDATA_H

#include <QString>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>

class MessageData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDateTime timestamp READ timestamp CONSTANT)
    Q_PROPERTY(QString topicName READ topicName CONSTANT)
    Q_PROPERTY(QString messageContent READ messageContent CONSTANT)
    Q_PROPERTY(bool isJsonMessage READ isJsonMessage CONSTANT)
    Q_PROPERTY(QString prettyJsonString READ prettyJsonString CONSTANT)

public:
    MessageData(QObject* parent = nullptr) 
        : QObject(parent), m_timestamp(QDateTime::currentDateTime()), m_isJsonMessage(false) {}
    
    MessageData(const QString& topic, const QString& message, const QDateTime& time = QDateTime::currentDateTime(), QObject* parent = nullptr)
        : QObject(parent), m_timestamp(time), m_topicName(topic), m_messageContent(message), m_isJsonMessage(false)
    {
        // Try to parse JSON message
        QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());
        m_isJsonMessage = !jsonDoc.isNull();
        if (m_isJsonMessage) {
            m_prettyJsonString = QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Indented));
        }
    }

    // Getters
    QDateTime timestamp() const { return m_timestamp; }
    QString topicName() const { return m_topicName; }
    QString messageContent() const { return m_messageContent; }
    bool isJsonMessage() const { return m_isJsonMessage; }
    QString prettyJsonString() const { return m_prettyJsonString; }

private:
    QDateTime m_timestamp;
    QString m_topicName;
    QString m_messageContent;
    bool m_isJsonMessage;
    QString m_prettyJsonString;
};

Q_DECLARE_METATYPE(MessageData)

#endif // MESSAGEDATA_H
