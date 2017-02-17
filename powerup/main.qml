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

import Qt3D.Core 2.0
import Qt3D.Render 2.9
import Qt3D.Input 2.0
import Qt3D.Extras 2.9
import QtQuick 2.0

Entity {
    id: scene

    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                camera: mainCamera
                clearColor: Qt.rgba(0.1, 0.1, 0.1, 1.0)
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    Camera {
        id: mainCamera
        position: Qt.vector3d(-10, 0, 0)
        viewCenter: Qt.vector3d(0, 0, 0)

        NumberAnimation on exposure {
            from: -2; to: 2
            duration: 5000
            easing.type: Easing.SineCurve
            loops: Animation.Infinite
        }
    }

    OrbitCameraController {
        camera: mainCamera
        linearSpeed: 10
        lookSpeed: 240
    }

    SkyboxEntity {
        baseName: "qrc:/assets/envmaps/wobbly-bridge/wobbly_bridge_4k_cube_radiance"
        extension: ".dds"
        gammaCorrect: true
    }

    Entity {
        components: [
            Mesh {
                source: "qrc:/assets/powerup/powerup.obj"
            },
            TexturedMetalRoughMaterial {
                baseColor: TextureLoader {
                    format: Texture.SRGB8_Alpha8
                    source: "qrc:/assets/powerup/basecolor.png"
                }
                metalness: TextureLoader { source: "qrc:/assets/powerup/metalness.png" }
                roughness: TextureLoader { source: "qrc:/assets/powerup/roughness.png" }
                normal: TextureLoader { source: "qrc:/assets/powerup/normal.png" }
                ambientOcclusion: TextureLoader { source: "qrc:/assets/powerup/ambientocclusion.png" }
                environmentIrradiance: TextureLoader {
                    source: "qrc:/assets/envmaps/wobbly-bridge/wobbly_bridge_4k_cube_irradiance.dds"

                    minificationFilter: Texture.LinearMipMapLinear
                    magnificationFilter: Texture.Linear
                    wrapMode {
                        x: WrapMode.ClampToEdge
                        y: WrapMode.ClampToEdge
                    }
                    generateMipMaps: false
                }
                environmentSpecular: TextureLoader {
                    source: "qrc:/assets/envmaps/wobbly-bridge/wobbly_bridge_4k_cube_specular.dds"

                    minificationFilter: Texture.LinearMipMapLinear
                    magnificationFilter: Texture.Linear
                    wrapMode {
                        x: WrapMode.ClampToEdge
                        y: WrapMode.ClampToEdge
                    }
                    generateMipMaps: false
                }
            }
        ]
    }
}
