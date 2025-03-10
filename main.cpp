#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQuickControls2/QQuickStyle>  // Fixed include path
#include "mqttbrokermanager.h"
#include "messagedata.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Set application style to one that allows customization
    QQuickStyle::setStyle("Fusion");

    // Register MessageData type with QML system
    qRegisterMetaType<MessageData>();
    qmlRegisterType<MessageData>("MqttApp", 1, 0, "MessageData");

    // Create the MQTT broker manager
    MqttBrokerManager brokerManager;

    QQmlApplicationEngine engine;
    
    // Expose the broker manager to QML
    engine.rootContext()->setContextProperty("brokerManager", &brokerManager);
    
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("mqtt_sub_plus_brokerProcess", "Main");

    return app.exec();
}
