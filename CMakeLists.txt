cmake_minimum_required(VERSION 3.16)

project(mqtt_sub_plus_brokerProcess VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Mqtt QuickControls2)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appmqtt_sub_plus_brokerProcess
    main.cpp
    mqttbrokermanager.cpp
    mqttbrokermanager.h
    messageData.h
)

qt_add_qml_module(appmqtt_sub_plus_brokerProcess
    URI mqtt_sub_plus_brokerProcess
    VERSION 1.0
    QML_FILES
        Main.qml
)


target_link_libraries(appmqtt_sub_plus_brokerProcess
    PRIVATE Qt6::Quick Qt6::Mqtt Qt6::QuickControls2
)

include(GNUInstallDirs)
