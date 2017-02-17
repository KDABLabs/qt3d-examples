/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
**
** This file is part of the Qt3D examples module.
**
** BSD License Usage
** You may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/

import QtQuick 2.0
import QtQuick.Scene3D 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

Item {
    id: root

    Scene3D {
        anchors.fill: parent
        focus: true
        aspects: ["input", "logic"]

        PbrSphere {
            baseColor: Qt.rgba(baseColorRed.value / 255,
                               baseColorGreen.value / 255,
                               baseColorBlue.value / 255,
                               1.0)
            metalness: metalnessSlider.value
            roughness: roughnessSlider.value
            exposure: exposureSlider.value
            gamma: gammaSlider.value
        }
    }

    ColumnLayout {
        anchors.left: parent.horizontalCenter
        anchors.leftMargin: parent.width * 0.25
        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.top: parent.top
        spacing: 5

        Text { text: "Base color RGB" }
        RowLayout {
            Text { text: "R" }
            Slider {
                id: baseColorRed
                Layout.fillWidth: true
                minimumValue: 0
                maximumValue: 255
                value: 128
            }
        }
        RowLayout {
            Text { text: "G" }
            Slider {
                id: baseColorGreen
                Layout.fillWidth: true
                minimumValue: 0
                maximumValue: 255
                value: 195
            }
        }
        RowLayout {
            Text { text: "B" }
            Slider {
                id: baseColorBlue
                Layout.fillWidth: true
                minimumValue: 0
                maximumValue: 255
                value: 66
            }
        }
        Text { text: "Metallic" }
        Slider {
            id: metalnessSlider
            Layout.fillWidth: true
            minimumValue: 0
            maximumValue: 1
        }
        Text { text: "Roughness" }
        Slider {
            id: roughnessSlider
            Layout.fillWidth: true
            minimumValue: 0
            maximumValue: 1
        }
        Text { text: "Exposure" }
        Slider {
            id: exposureSlider
            Layout.fillWidth: true
            minimumValue: -2
            maximumValue: 2
        }
        Text { text: "Gamma" }
        Slider {
            id: gammaSlider
            Layout.fillWidth: true
            minimumValue: 1.2
            maximumValue: 2.8
            value: 1.8
        }
    }
}
