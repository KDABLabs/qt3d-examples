import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    id: root
    property Layer layer
    property variant geometryRenderer
    property alias satelliteUniformBuffer: satelliteUBOParam.value
    property alias elevationUniformBuffer: elevationUBOParam.value
    property Texture2DArray satelliteImagery
    property Texture2DArray elevation

    function lookAt(pos, center, up)
    {
        var m = Qt.matrix4x4();
        m.lookAt(pos, center, up);
        return m;
    }

    CameraLens {
        id: projectionLens
        nearPlane: 0.01
        farPlane: 1000.0
        fieldOfView: 90.0
        aspectRatio: 1.0
    }

    // We move within the cube map to add details to the area we are really
    // looking at
    readonly property vector3d mainCameraDirVector: mainCamera.position.minus(mainCamera.viewCenter)
    readonly property vector3d mainCameraDirVectorUnit: mainCameraDirVector.normalized()
    readonly property real distanceToCenter: mainCameraDirVector.length() / 6378137.0

    readonly property vector3d relativePositon: Qt.vector3d(0, 0, 0)//.minus(mainCameraDirVectorUnit.times(distanceToCenter))

    onRelativePositonChanged: console.log(relativePositon)


    Material {
        id: material
        parameters: [
            Parameter { name: "posXMat"; value: lookAt(relativePositon,
                                                       Qt.vector3d(1.0, 0.0, 0.0).plus(relativePositon),
                                                       Qt.vector3d(0.0, -1.0, 0.0)) },
            Parameter { name: "negXMat"; value: lookAt(relativePositon,
                                                       Qt.vector3d(-1.0, 0.0, 0.0).plus(relativePositon),
                                                       Qt.vector3d(0.0, -1.0, 0.0)) },
            Parameter { name: "posYMat"; value: lookAt(relativePositon,
                                                       Qt.vector3d(0.0, 1.0, 0.0).plus(relativePositon),
                                                       Qt.vector3d(0.0, 0.0, 1.0)) },
            Parameter { name: "negYMat"; value: lookAt(relativePositon,
                                                       Qt.vector3d(0.0, 1.0, 0.0).plus(relativePositon),
                                                       Qt.vector3d(0.0, 0.0, 1.0)) },
            Parameter { name: "posZMat"; value: lookAt(relativePositon,
                                                       Qt.vector3d(0.0, 0.0, 1.0).plus(relativePositon),
                                                       Qt.vector3d(0.0, 1.0, 0.0)) },
            Parameter { name: "negZMat"; value: lookAt(relativePositon,
                                                       Qt.vector3d(0.0, 0.0, -1.0).plus(relativePositon),
                                                       Qt.vector3d(0.0, -1.0, 0.0)) },
            Parameter { name: "projMat"; value: projectionLens.projectionMatrix },
            Parameter { id: satelliteUBOParam; name: "ImageryMappingBlock" },
            Parameter { id: elevationUBOParam; name: "ElevationMappingBlock" },
            Parameter { name: "satelliteImagery"; value: satelliteImagery },
            Parameter { name: "elevation"; value: elevation }
        ]

        effect: Effect {
            techniques: [
                // GL3 Technique
                Technique {
                    graphicsApiFilter {
                        api: GraphicsApiFilter.OpenGL
                        profile: GraphicsApiFilter.CoreProfile
                        majorVersion: 4
                        minorVersion: 3
                    }

                    renderPasses: [
                        // Draw earth cube with textures into cubemap
                        RenderPass {
                            shaderProgram:  ShaderProgram {
                                vertexShaderCode: loadSource("qrc:/shaders/cubemap.vert")
                                geometryShaderCode: loadSource("qrc:/shaders/cubemap.geom")
                                fragmentShaderCode: loadSource("qrc:/shaders/cubemap.frag")
                            }
                        },
                        // Draw wireframe
                        RenderPass {
                            shaderProgram: ShaderProgram {
                                vertexShaderCode: loadSource("qrc:/shaders/cubemap.vert")
                                geometryShaderCode: loadSource("qrc:/shaders/cubemapwireframe.geom")
                                fragmentShaderCode: "
                                    #version 430 core

                                    out vec4 fragColor;

                                    void main()
                                    {
                                        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
                                    }
                                    "
                            }
                        }
                    ]
                }
            ]
        }
    }

    components: [
        layer,
        geometryRenderer,
        material
    ]
}
