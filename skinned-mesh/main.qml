/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
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
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt3D.Core 2.10
import Qt3D.Render 2.10
import Qt3D.Input 2.0
import Qt3D.Animation 2.10
import Qt3D.Extras 2.10
import QtQuick 2.9

DefaultSceneEntity {
    id: scene

    SkinnedPbrEffect {
        id: texturedSkinnedPbrEffect
        useTextures: true
    }

    SkinnedPbrEffect {
        id: skinnedPbrEffect
    }

    AnimationClipLoader {
        id: animationClip
        source: "qrc:/assets/gltf/2.0/Robot/robot.gltf"
    }

    AnimatedEntity {
        id: riggedFigure1
        transform.scale: 0.035
        transform.rotationX: 90

        source: "qrc:/assets/gltf/2.0/Robot/robot.gltf"
        clip: animationClip
        playbackRate: 1.0

        effect: texturedSkinnedPbrEffect
        textureBaseName: "qrc:/assets/gltf/2.0/Robot/robot"
    }

    NodeInstantiator {
        id: instantiator
        property var colors: [
//            Qt.rgba(0, 1, 0, 1),    // green
//            Qt.rgba(1, 0.6, 0, 1),  // orange
//            Qt.rgba(1, 0, 0, 1),    // red
//            Qt.rgba(0, 0, 1, 1),    // blue
            "_green", "_gray", "_red", "_blue"
        ]
        model: ListModel {
            ListElement { color: 0; x: -3; z: -2 }
            ListElement { color: 0; x:  3; z: -2 }

            ListElement { color: 1; x: -6; z: -4 }
            ListElement { color: 1; x:  0; z: -4 }
            ListElement { color: 1; x:  6; z: -4 }

            ListElement { color: 2; x: -9; z: -6 }
            ListElement { color: 2; x: -3; z: -6 }
            ListElement { color: 2; x:  3; z: -6 }
            ListElement { color: 2; x:  9; z: -6 }

            ListElement { color: 3; x: -12; z: -8 }
            ListElement { color: 3; x: -6; z: -8 }
            ListElement { color: 3; x:  0; z: -8 }
            ListElement { color: 3; x:  6; z: -8 }
            ListElement { color: 3; x:  12; z: -8 }
        }

        delegate: AnimatedEntity {
            transform.scale: 0.035;
            transform.rotationX: 90
            transform.translation: Qt.vector3d(model.x, 0, model.z)

            source: "qrc:/assets/gltf/2.0/Robot/robot.gltf"
            clip: animationClip
            playbackRate: 1.0

//            effect: skinnedPbrEffect
//            baseColor: instantiator.colors[model.color]
            effect: texturedSkinnedPbrEffect
            textureBaseName: "qrc:/assets/gltf/2.0/Robot/robot"
            textureBaseColor: instantiator.colors[model.color]
        }
    }
}
