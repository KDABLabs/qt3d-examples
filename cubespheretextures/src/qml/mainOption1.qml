import QtQuick 2.7
import QtQuick.Scene3D 2.0

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import MyModule 1.0

import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1

Item {
    id: mainRoot
    width: 1200
    height: 800

    Scene3D {
        anchors.fill: parent
        aspects: ["input", "logic"]
        focus: true
        multisample: true

        Entity {
            id: rootEntity

            components: [
                RenderSettings {
                    renderPolicy: RenderSettings.Always
                    activeFrameGraph: FrameGraphOption1 {
                        id: frameGraph
                        camera: mainCamera
                    }
                },
                // Event Source will be set by the Qt3DQuickWindow
                InputSettings { }
            ]

            CameraController {
                id: cameraController
                camera: mainCamera
            }

            Camera {
                id: mainCamera
                projectionType: CameraLens.PerspectiveProjection
                fieldOfView: 60 // Vertical FOV (set so that horizontalFOV is 90 for that value and ratio)
                nearPlane : (mainCamera.position.length() - 6378137) / 10 // farPlane / 1000 //1.0 / (altitudeLevel) * (2500 / (Math.pow(2, altitudeLevel - 1)))
                farPlane : mainCamera.position.length()
                position: Qt.vector3d(0, 0, 10000000) // 10000km from earth center
                upVector: Qt.vector3d(0.0, 1.0, 0.0)
                viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
                aspectRatio: mainRoot.width / mainRoot.height
            }

            CubeSphere {
                id: cubeSphere
                camera: mainCamera
                viewportSize: Qt.size(mainRoot.width, mainRoot.height)
                useImagery: showImagerySwitch.checked
                useElevation: useElevationSwitch.checked
            }

            // Sphere Rendering
            Entity {
                readonly property Material material: CubeSphereMaterialOption1 {
                    wireframeEnabled: wireframeSwitch.checked
                    useLogarithmicDepth: logarithmicSwitch.checked
                    showImagery: showImagerySwitch.checked
                    useElevation: useElevationSwitch.checked
                    perspectiveFarPlaneDistance: mainCamera.farPlane
                    logarithmicDepthConstant: 0.0001
                    eyePosition: mainCamera.position
                    satelliteTexture: cubeSphere.satelliteImagery
                    elevationTexture: cubeSphere.elevation
                    satelliteUniformBuffer: cubeSphere.satelliteUniformBuffer
                    elevationUniformBuffer: cubeSphere.elevationUniformBuffer
                }
                readonly property Transform transform: Transform {
                    scale: 1
                }

                components: [
                    material,
                    transform,
                    cubeSphere.sphereRenderer,
                    frameGraph.finalPassLayer
                ]
            }

            AxesGnomon {
                layer: frameGraph.axisGnomonLayer
                transform.scale: 1000000 * 2
            }
        }
    }

    ///////// QT QUICK UI /////////
    Text {
        text: "Rendering with Option 1"
    }

    Pane {
        anchors.horizontalCenter: parent.horizontalCenter
        RowLayout {
            ColumnLayout {
                Text { text: "Altitude " +  ((mainCamera.position.length() - 6378137) / 1000).toFixed(3) + "km" }
                Text { text: "NearPlane " +  (mainCamera.nearPlane / 1000).toFixed(3) + "km" }
                Text { text: "FarPlane " +  (mainCamera.farPlane / 1000).toFixed(3) + "km" }
            }
            ColumnLayout {
                Text { text: "Tiles to render: " +  cubeSphere.tilesToRender }
                Text { text: "Imagery Tiles count: " +  cubeSphere.imageryTilesCount }
                Text { text: "Elevation Tiles count: " +  cubeSphere.elevationTilesCount }
            }
            Text { text: "Scene updated performed in: " +  cubeSphere.updateElapsed.toFixed(4) + "ms" }
        }
    }

    // Options Panel
    Pane {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        RowLayout {
            GroupBox {
                title: "Options         <font color=\"#0000ff\">X</font><font color=\"#00ff00\">Y</font><font color=\"#ff0000\">Z</font>"
                RowLayout {
                    anchors.fill: parent
                    Switch { id: wireframeSwitch; text: "Wireframe"; checked: true}
                    Switch { id: logarithmicSwitch; text: "LogarithmicDepth"; checked: false}
                    Switch { id: showImagerySwitch; text: "Show Imagery"; checked: false}
                    Switch { id: useElevationSwitch; text: "Use Elevation"; checked: false}
                }
            }
            GroupBox {
                title: "Base number of subdivisions: " + cubeSphere.baseLevel
                Slider { from: 1; to: 4; stepSize: 1; value: cubeSphere.baseLevel;
                    onValueChanged: {
                        cubeSphere.baseLevel = value;
                        if (cubeSphere.maxLevel < cubeSphere.baseLevel)
                            cubeSphere.maxLevel = cubeSphere.baseLevel
                    }
                }
            }
            GroupBox {
                title: "Max number of subdivisions: " + cubeSphere.maxLevel
                Slider { from: 1; to: 15; stepSize: 1; value: cubeSphere.maxLevel ;
                    onValueChanged: {
                        cubeSphere.maxLevel = value;
                        if (cubeSphere.maxLevel < cubeSphere.baseLevel)
                            cubeSphere.baseLevel = cubeSphere.maxLevel
                    }
                }

            }
        }
    }
}
