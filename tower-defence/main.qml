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
        position: Qt.vector3d(30, 30, 30)
        viewCenter: Qt.vector3d(0, 0, 0)
    }

    FirstPersonCameraController {
        camera: mainCamera
        linearSpeed: 10
        lookSpeed: 240
    }

//    SkyboxEntity {
//        baseName: "qrc:/assets/envmaps/wobbly-bridge/wobbly_bridge_4k" + _envmapFormat + "_cube_radiance"
//        extension: ".dds"
//        gammaCorrect: true
//    }

    Entity {
        components: [
            EnvironmentLight {
                id: envLight
                irradiance: TextureLoader {
                    source: "qrc:/assets/envmaps/wobbly-bridge/wobbly_bridge_4k" + _envmapFormat + "_cube_irradiance.dds"

                    minificationFilter: Texture.LinearMipMapLinear
                    magnificationFilter: Texture.Linear
                    wrapMode {
                        x: WrapMode.ClampToEdge
                        y: WrapMode.ClampToEdge
                    }
                    generateMipMaps: false
                }
                specular: TextureLoader {
                    source: "qrc:/assets/envmaps/wobbly-bridge/wobbly_bridge_4k" + _envmapFormat + "_cube_specular.dds"

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

    Entity {
        components: [
            Mesh {
                source: "qrc:/assets/tower-defence/barrel.obj"
            },
            Transform {
                translation: Qt.vector3d(-4, 0, 4)
            },
            TexturedMetalRoughMaterial {
                baseColor: TextureLoader {
                    format: Texture.SRGB8_Alpha8
                    source: "qrc:/assets/tower-defence/barrel-textures/Barrel_Base_Color.png"
                }
                metalness: TextureLoader { source: "qrc:/assets/tower-defence/barrel-textures/Barrel_Metallic.png" }
                roughness: TextureLoader { source: "qrc:/assets/tower-defence/barrel-textures/Barrel_Roughness.png" }
                normal: TextureLoader { source: "qrc:/assets/tower-defence/barrel-textures/Barrel_Normal_OpenGL.png" }
                ambientOcclusion: TextureLoader { source: "qrc:/assets/tower-defence/barrel-textures/Barrel_Mixed_AO.png" }
            }
        ]
    }

    // Texture to be used for all Modular objects
    TexturedMetalRoughMaterial {
        id: modularTexture
        baseColor: TextureLoader {
            format: Texture.SRGB8_Alpha8
            source: "qrc:/assets/tower-defence/modular/Material_Base_Color.png"
        }
        metalness: TextureLoader { source: "qrc:/assets/tower-defence/modular/Material_Metallic.png" }
        roughness: TextureLoader { source: "qrc:/assets/tower-defence/modular/Material_Roughness.png" }
        normal: TextureLoader { source: "qrc:/assets/tower-defence/modular/Material_Normal_OpenGL.png" }
        ambientOcclusion: TextureLoader { source: "qrc:/assets/tower-defence/modular/Material_Mixed_AO.png" }
    }

    Entity {
        Mesh {
            id: bridgeMesh
            source: "qrc:/assets/tower-defence/modular_bridge.obj"
        }
        Transform {
            id: bridgeTransform
            translation: Qt.vector3d(-8, 0, -9)
        }
        components: [ bridgeMesh, modularTexture, bridgeTransform ]
    }
    Entity {
        Mesh {
            id: bridgeExtMesh
            source: "qrc:/assets/tower-defence/modular_bridgeextension.obj"
        }
        Transform {
            id: bridgeExtTransform
            translation: Qt.vector3d(-6, 0, -9)
        }
        components: [ bridgeExtMesh, modularTexture, bridgeExtTransform ]
    }
    Entity {
        Mesh {
            id: floor1Mesh
            source: "qrc:/assets/tower-defence/modular_floor1.obj"
        }
        Transform {
            id: floor1Transform
            translation: Qt.vector3d(-4, 0, 0)
        }
        components: [ floor1Mesh, modularTexture, floor1Transform ]
    }
    Entity {
        Mesh {
            id: floor2Mesh
            source: "qrc:/assets/tower-defence/modular_floor2.obj"
        }
        Transform {
            id: floor2Transform
            translation: Qt.vector3d(-4, 0, -3)
        }
        components: [ floor2Mesh, modularTexture, floor2Transform ]
    }
    Entity {
        Mesh {
            id: floor3Mesh
            source: "qrc:/assets/tower-defence/modular_floor3.obj"
        }
        Transform {
            id: floor3Transform
            translation: Qt.vector3d(-4, 0, -6)
        }
        components: [ floor3Mesh, modularTexture, floor3Transform ]
    }
    Entity {
        Mesh {
            id: floor4Mesh
            source: "qrc:/assets/tower-defence/modular_floor4.obj"
        }
        Transform {
            id: floor4Transform
            translation: Qt.vector3d(-8, 0, 0)
        }
        components: [ floor4Mesh, modularTexture, floor4Transform ]
    }
    Entity {
        Mesh {
            id: floor5Mesh
            source: "qrc:/assets/tower-defence/modular_floor5.obj"
        }
        Transform {
            id: floor5Transform
            translation: Qt.vector3d(-8, 0, -3)
        }
        components: [ floor5Mesh, modularTexture, floor5Transform ]
    }
    Entity {
        Mesh {
            id: floor6Mesh
            source: "qrc:/assets/tower-defence/modular_floor6.obj"
        }
        Transform {
            id: floor6Transform
            translation: Qt.vector3d(-8, 0, -6)
        }
        components: [ floor6Mesh, modularTexture, floor6Transform ]
    }
    Entity {
        Mesh {
            id: fullwallMesh
            source: "qrc:/assets/tower-defence/modular_fullwall.obj"
        }
        Transform {
            id: fullwallTransform
            translation: Qt.vector3d(-8, 0, 5)
        }
        components: [ fullwallMesh, modularTexture, fullwallTransform ]
    }
    Entity {
        Mesh {
            id: halfwallMesh
            source: "qrc:/assets/tower-defence/modular_halfwall.obj"
        }
        Transform {
            id: halfwallTransform
            translation: Qt.vector3d(-8, 0, 8)
        }
        components: [ halfwallMesh, modularTexture, halfwallTransform ]
    }
    Entity {
        Mesh {
            id: rampMesh
            source: "qrc:/assets/tower-defence/modular_ramp.obj"
        }
        Transform {
            id: rampTransform
            translation: Qt.vector3d(-10, 0, 18)
        }
        components: [ rampMesh, modularTexture, rampTransform ]
    }
    Entity {
        Mesh {
            id: turretMesh
            source: "qrc:/assets/tower-defence/modular_turret.obj"
        }
        Transform {
            id: turretTransform
            translation: Qt.vector3d(-8, 0, -14)
        }
        components: [ turretMesh, modularTexture, turretTransform ]
    }
    Entity {
        Mesh {
            id: wallExtMesh
            source: "qrc:/assets/tower-defence/modular_wallexterior.obj"
        }
        Transform {
            id: wallExtTransform
            translation: Qt.vector3d(-6, 0, 11)
        }
        components: [ wallExtMesh, modularTexture, wallExtTransform ]
    }
    Entity {
        Mesh {
            id: wallIntMesh
            source: "qrc:/assets/tower-defence/modular_wallinterior.obj"
        }
        Transform {
            id: wallIntTransform
            translation: Qt.vector3d(-8, 0, 11)
        }
        components: [ wallIntMesh, modularTexture, wallIntTransform ]
    }
    Entity {
        Mesh {
            id: doorMesh
            source: "qrc:/assets/tower-defence/sci-fi-door.obj"
        }
        Transform {
            id: doorTransform
            translation: Qt.vector3d(-12, 0, -2)
            rotationY: 90.0
        }
        components: [ doorMesh, modularTexture, doorTransform ]
    }
}
