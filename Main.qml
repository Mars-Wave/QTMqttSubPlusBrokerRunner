import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
// Remove GraphicalEffects import that's causing issues

ApplicationWindow {
    width: 900
    height: 600
    visible: true
    title: qsTr("MQTT Broker Manager")
    color: "#1E1E1E" // Dark background
    
    // Define the app color scheme
    QtObject {
        id: theme
        property color darkBackground: "#1E1E1E"
        property color mediumBackground: "#252526"
        property color lightBackground: "#2D2D30"
        property color headerBackground: "#333333"
        property color primary: "#0078D7"
        property color accent: "#FF8C00"  // Orange accent
        property color statusRunning: "#8A2BE2"  // Changed to violet
        property color textPrimary: "#FFFFFF"
        property color textSecondary: "#CCCCCC"
        property color textMuted: "#888888"
        property color borderColor: "#3F3F3F"
        property color buttonBackground: "#3A3A3A"
        property color buttonHover: "#4A4A4A"
        property color buttonPressed: "#555555"
        property color notificationBackground: Qt.rgba(1, 0.55, 0, 0.85) // Semi-transparent orange
        property color headerBackgroundActive: Qt.alpha(theme.accent, 0.15)  // Match the scrollbar's translucent orange
    }

    // Custom button style
    Component {
        id: styledButtonStyle
        
        Rectangle {
            id: buttonBackground
            implicitWidth: Math.max(120, contentItem.implicitWidth + 20)
            implicitHeight: 36
            color: control.pressed ? theme.buttonPressed : control.hovered ? theme.buttonHover : theme.buttonBackground
            border.width: 1
            border.color: control.pressed ? theme.accent : control.hovered ? Qt.lighter(theme.accent, 1.2) : "#555555"
            radius: 4
            
            // Subtle gradient
            gradient: Gradient {
                GradientStop { position: 0.0; color: control.pressed ? Qt.darker(buttonBackground.color, 1.05) : buttonBackground.color }
                GradientStop { position: 1.0; color: control.pressed ? buttonBackground.color : Qt.darker(buttonBackground.color, 1.1) }
            }
            
            // Simpler shadow using only Rectangle elements - no GraphicalEffects needed
            Rectangle {
                visible: !control.pressed
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.bottom
                }
                height: 2
                color: "black"
                opacity: 0.2
                radius: 2
            }
            
            // Highlight effect on top
            Rectangle {
                width: parent.width
                height: 1
                color: control.pressed ? Qt.rgba(1, 1, 1, 0.05) : Qt.rgba(1, 1, 1, 0.1)
                anchors.top: parent.top
            }
            
            // Content
            Label {
                id: contentItem
                anchors.centerIn: parent
                text: control.text
                font.pixelSize: 14
                font.weight: Font.Medium
                color: control.pressed ? theme.accent : theme.textPrimary
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
            
            // Animation
            Behavior on color {
                ColorAnimation { duration: 150 }
            }
        }
    }
    
    // Error handling for component application
    Component.onCompleted: {
        try {
            // This part was causing issues - let's comment it out for now
            // for (var i = 0; i < Controls.length; i++) {
            //     if (Controls[i] instanceof Button) {
            //         Controls[i].background = styledButtonStyle.createObject(Controls[i], { control: Controls[i] })
            //     }
            // }
        } catch (e) {
            console.error("Error applying button styles:", e)
        }
    }

    FileDialog {
        id: fileDialog
        title: "Choose MQTT Broker Executable"
        nameFilters: ["Executable files (*.exe)"]
        onAccepted: {
            brokerManager.setBrokerExecutablePath(selectedFile.toString().replace(/^(file:\/{3})/, ""))
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16

        // App Header
        Rectangle {
            Layout.fillWidth: true
            height: 50
            color: brokerManager.isRunning ? theme.headerBackgroundActive : theme.headerBackground
            radius: 4

            // Smooth color transition animation
            Behavior on color {
                ColorAnimation { duration: 800 }  // Slow, smooth transition
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                spacing: 12
                
                Label {
                    text: "MQTT broker launcher + sub manager"  // Updated text
                    font {
                        pointSize: 14
                        weight: Font.DemiBold
                    }
                    color: theme.textPrimary
                }
                
                Item { Layout.fillWidth: true }
                
                Label {
                    text: "Status: " + (brokerManager.isRunning ? "Running" : "Stopped")
                    color: brokerManager.isRunning ? theme.statusRunning : theme.textSecondary  // Changed to violet
                    font.bold: true
                }
            }
        }

        // Control Panel
        Rectangle {
            Layout.fillWidth: true
            color: theme.lightBackground
            radius: 4
            height: controlLayout.height + 24
            
            ColumnLayout {
                id: controlLayout
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    margins: 12
                }
                spacing: 12
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    
                    Button {
                        id: toggleButton
                        text: brokerManager.isRunning ? "Stop Broker" : "Start Broker"
                        
                        background: Rectangle {
                            implicitWidth: 120
                            implicitHeight: 36
                            color: theme.buttonBackground
                            border.width: 1
                            border.color: toggleButton.pressed ? theme.accent : "#555555"
                            radius: 4
                            
                            // Minimalist indicator - just a subtle left border
                            Rectangle {
                                width: 3
                                anchors {
                                    left: parent.left
                                    top: parent.top
                                    bottom: parent.bottom
                                }
                                color: brokerManager.isRunning ? "#FF4136" : "#4CAF50"  // Red for stop, green for start
                                radius: 2
                            }
                        }
                        
                        contentItem: Label {
                            anchors.centerIn: parent
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            text: toggleButton.text
                            font.pixelSize: 13  // Slightly smaller font
                            font.weight: Font.Medium
                            color: theme.textPrimary
                        }
                        
                        onClicked: {
                            brokerManager.toggleBroker()
                        }
                    }
                    
                    Button {
                        text: "Clear History"
                        
                        background: Rectangle {
                            implicitWidth: 120
                            implicitHeight: 36
                            color: theme.buttonBackground
                            border.width: 1
                            border.color: parent.pressed ? theme.accent : "#555555"
                            radius: 4
                        }
                        
                        contentItem: Label {
                            anchors.centerIn: parent
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            text: parent.text
                            font.pixelSize: 13  // Slightly smaller font
                            font.weight: Font.Medium
                            color: theme.textPrimary
                        }
                        
                        onClicked: {
                            brokerManager.clearHistory()
                        }
                    }
                }
                
                GridLayout {
                    Layout.fillWidth: true
                    columns: 3
                    columnSpacing: 12
                    rowSpacing: 12
                    
                    Label {
                        text: "Broker executable:"
                        color: theme.textSecondary
                    }
                    
                    TextField {
                        id: executablePathField
                        Layout.fillWidth: true
                        text: brokerManager.brokerExecutablePath
                        readOnly: true
                        color: theme.textPrimary
                        
                        background: Rectangle {
                            implicitHeight: 36
                            color: "#252526"
                            border.width: 1
                            border.color: parent.activeFocus ? theme.accent : "#555555"
                            radius: 4
                        }
                    }
                    
                    Button {
                        text: "Browse..."
                        
                        background: Rectangle {
                            implicitWidth: 100
                            implicitHeight: 36
                            color: parent.pressed ? theme.buttonPressed : parent.hovered ? theme.buttonHover : theme.buttonBackground
                            border.width: 1
                            border.color: parent.pressed ? theme.accent : parent.hovered ? Qt.lighter(theme.accent, 1.2) : "#555555"
                            radius: 4
                        }
                        
                        contentItem: Label {
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            text: parent.text
                            font.pixelSize: 14
                            font.weight: Font.Medium
                            color: parent.pressed ? theme.accent : theme.textPrimary
                        }
                        
                        onClicked: fileDialog.open()
                    }
                    
                    Label {
                        text: "Subscription topic:"
                        color: theme.textSecondary
                    }
                    
                    TextField {
                        id: topicField
                        Layout.fillWidth: true
                        text: brokerManager.subscriptionTopic
                        placeholderText: "Enter subscription topic (e.g., # for all topics)"
                        color: theme.textPrimary
                        
                        background: Rectangle {
                            implicitHeight: 36
                            color: "#252526"
                            border.width: 1
                            border.color: parent.activeFocus ? theme.accent : "#555555"
                            radius: 4
                        }
                        
                        onEditingFinished: {
                            brokerManager.setSubscriptionTopic(text)
                        }
                    }
                    
                    Button {
                        text: "Apply"
                        enabled: brokerManager.isRunning && topicField.text !== brokerManager.subscriptionTopic
                        
                        background: Rectangle {
                            implicitWidth: 100
                            implicitHeight: 36
                            opacity: parent.enabled ? 1.0 : 0.5
                            color: parent.pressed ? theme.buttonPressed : parent.hovered ? theme.buttonHover : theme.buttonBackground
                            border.width: 1
                            border.color: parent.pressed ? theme.accent : parent.hovered ? Qt.lighter(theme.accent, 1.2) : "#555555"
                            radius: 4
                        }
                        
                        contentItem: Label {
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            text: parent.text
                            font.pixelSize: 14
                            font.weight: Font.Medium
                            opacity: parent.enabled ? 1.0 : 0.5
                            color: parent.pressed ? theme.accent : theme.textPrimary
                        }
                        
                        onClicked: {
                            brokerManager.setSubscriptionTopic(topicField.text)
                        }
                        
                        ToolTip.visible: hovered
                        ToolTip.text: "Update subscription while broker is running"
                        ToolTip.delay: 500
                    }
                }
            }
        }

        // Message view with elegant styling
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: theme.mediumBackground
            radius: 4
            border.width: 1
            border.color: theme.borderColor
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 0
                spacing: 0
                
                // Column headers
                Rectangle {
                    id: listHeader
                    Layout.fillWidth: true
                    height: 36
                    color: theme.headerBackground
                    radius: 4
                    
                    Rectangle {
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                        height: parent.radius
                        color: theme.headerBackground
                    }
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12
                        spacing: 8
                        
                        Label {
                            Layout.preferredWidth: 180
                            text: "Timestamp"
                            font.bold: true
                            color: theme.textSecondary
                        }
                        
                        Label {
                            Layout.preferredWidth: 150
                            text: "Topic"
                            font.bold: true
                            color: theme.textSecondary
                        }
                        
                        Label {
                            Layout.fillWidth: true
                            text: "Message"
                            font.bold: true
                            color: theme.textSecondary
                        }
                    }
                }

                // New messages notification banner with translucency
                Rectangle {
                    id: newMessagesBanner
                    Layout.fillWidth: true
                    height: 36
                    visible: brokerManager.bufferedMessageCount > 0
                    color: theme.notificationBackground  // Use the semi-transparent orange
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12
                        
                        Label {
                            Layout.fillWidth: true
                            text: brokerManager.bufferedMessageCount + " new message" + 
                                  (brokerManager.bufferedMessageCount !== 1 ? "s" : "") + " arrived"
                            font.bold: true
                            color: "#FFFFFF"
                        }
                        
                        Button {
                            text: "Show Messages"
                            
                            background: Rectangle {
                                implicitWidth: 120
                                implicitHeight: 30
                                color: parent.pressed ? Qt.darker(theme.buttonBackground, 1.1) : theme.buttonBackground
                                border.width: 1
                                border.color: theme.accent
                                radius: 4
                            }
                            
                            contentItem: Label {
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                text: parent.text
                                font.pixelSize: 13
                                font.weight: Font.Medium
                                color: theme.textPrimary
                            }
                            
                            onClicked: {
                                brokerManager.applyBufferedMessages()
                            }
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    ListView {
                        id: messageListView
                        anchors.fill: parent
                        clip: true
                        model: brokerManager.messageHistory
                        
                        // Now explicitly set to render top to bottom (new messages at top)
                        verticalLayoutDirection: ListView.TopToBottom
                        
                        // Same delegate as before
                        delegate: Item {
                            width: messageListView.width
                            height: messageColumn.height
                            
                            Rectangle {
                                anchors.fill: parent
                                color: index % 2 === 0 ? "#252526" : "#2A2A2B"
                            }
                            
                            ColumnLayout {
                                id: messageColumn
                                width: parent.width
                                spacing: 0
                                
                                // Main message row with columns
                                RowLayout {
                                    Layout.fillWidth: true
                                    Layout.margins: 8
                                    spacing: 8
                                    
                                    Label {
                                        Layout.preferredWidth: 180
                                        text: brokerManager.formatTimestamp(modelData.timestamp)
                                        font.family: "Consolas, 'Courier New', monospace"
                                        color: theme.textSecondary
                                        elide: Text.ElideRight
                                    }
                                    
                                    Label {
                                        Layout.preferredWidth: 150
                                        text: modelData.topicName
                                        elide: Text.ElideRight
                                        font.italic: modelData.topicName === "system_trace"
                                        font.bold: modelData.topicName === "system_trace"
                                        color: modelData.topicName === "system_trace" ? theme.textMuted : theme.textPrimary
                                    }
                                    
                                    // Show either the plain message or a "JSON" indicator if it's JSON
                                    Label {
                                        Layout.fillWidth: true
                                        visible: !modelData.isJsonMessage
                                        text: modelData.messageContent
                                        wrapMode: Text.WordWrap
                                        color: theme.textPrimary
                                    }
                                    
                                    Label {
                                        Layout.fillWidth: true
                                        visible: modelData.isJsonMessage
                                        text: "<JSON Message>"
                                        color: theme.accent
                                        font.italic: true
                                    }
                                }
                                
                                // JSON content when applicable with orange accent
                                Rectangle {
                                    visible: modelData.isJsonMessage
                                    Layout.fillWidth: true
                                    Layout.leftMargin: 348
                                    Layout.rightMargin: 8
                                    Layout.bottomMargin: 8
                                    color: "#2A2A2A"
                                    border {
                                        width: 1
                                        color: Qt.alpha(theme.accent, 0.5)
                                    }
                                    radius: 4
                                    height: jsonText.height + 16
                                    
                                    Rectangle {
                                        anchors {
                                            top: parent.top
                                            left: parent.left
                                            right: parent.right
                                            topMargin: -1
                                        }
                                        height: 2
                                        color: theme.accent
                                        opacity: 0.7
                                    }
                                    
                                    TextArea {
                                        id: jsonText
                                        anchors {
                                            left: parent.left
                                            right: parent.right
                                            top: parent.top
                                            margins: 8
                                        }
                                        text: modelData.prettyJsonString
                                        readOnly: true
                                        wrapMode: TextEdit.Wrap
                                        font.family: "Consolas, 'Courier New', monospace"
                                        font.pointSize: 9
                                        color: theme.textPrimary
                                        background: null
                                    }
                                }
                            }
                        }

                        // Custom scrollbar with lock/refresh toggle
                        ScrollBar.vertical: ScrollBar {
                            id: messageScrollBar
                            policy: ScrollBar.AlwaysOn
                            
                            contentItem: Rectangle {
                                implicitWidth: 10
                                radius: 5
                                color: "#FFFFFF"  // White thumb
                            }
                            
                            background: Rectangle {
                                implicitWidth: 10
                                color: Qt.alpha(theme.accent, 0.15)  // Translucent orange
                                radius: 5
                            }
                        }
                    }

                    // Properly centered and round lock/refresh toggle button
                    Button {
                        id: lockToggleButton
                        width: 42
                        height: 42
                        anchors {
                            right: parent.right
                            bottom: parent.bottom
                            margins: 10
                        }
                        
                        // Simplify down to just what's necessary
                        background: Rectangle {
                            implicitWidth: 42
                            implicitHeight: 42
                            color: theme.buttonBackground
                            border.width: 1.5
                            border.color: brokerManager.isViewLocked ? theme.accent : "#555555"
                            radius: width / 2  // Perfect circle
                        }
                        
                        // Use Text with emoji for both states - locked and unlocked
                        contentItem: Text {
                            anchors.centerIn: parent
                            text: brokerManager.isViewLocked ? "🔒" : "🔓"  // Lock or unlock emoji
                            font.pixelSize: 18
                            font.family: "Segoe UI Emoji, Apple Color Emoji, Noto Color Emoji"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            color: "#FFFFFF"
                        }
                        
                        onClicked: {
                            brokerManager.toggleViewLock()
                        }
                        
                        ToolTip.visible: hovered
                        ToolTip.text: brokerManager.isViewLocked ? "Message view is locked. Click to unlock." : 
                                                               "Auto-refresh is on. Click to lock view."
                    }
                }
            }
        }
    }
}
