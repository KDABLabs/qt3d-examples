import Qt3D.Core 2.0
import Qt3D.Render 2.0

Material {

    property bool wireframeEnabled: false
    property bool useLogarithmicDepth: false
    property real perspectiveFarPlaneDistance: 0.0
    property real logarithmicDepthConstant: 0.0
    property vector3d eyePosition
    property bool showImagery: false
    property bool useElevation: false
    property alias satelliteCubeMap: satelliteCubeMapParam.value
    property alias elevationCubeMap: elevationCubeMapParam.value

    parameters: [
        Parameter { id: satelliteCubeMapParam; name: "satelliteCubemap"; },
        Parameter { id: elevationCubeMapParam; name: "elevationCubemap"; },
        Parameter { name: "wireframeEnabled"; value: wireframeEnabled },
        Parameter { name: "perspectiveFarPlaneDistance"; value: perspectiveFarPlaneDistance },
        Parameter { name: "logarithmicDepthConstant"; value: logarithmicDepthConstant },
        Parameter { name: "useLogarithmicDepth"; value: useLogarithmicDepth },
        Parameter { name: "useElevation"; value: useElevation },
        Parameter { name: "showImagery"; value: showImagery },
        Parameter { name: "eyePos"; value: eyePosition }
    ]

    effect: Effect {
        techniques: [
            Technique {
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 4
                    minorVersion: 3
                }

                renderPasses: [
                    RenderPass {
                        shaderProgram: ShaderProgram {
                            vertexShaderCode: loadSource("qrc:/shaders/passthrough.vert")
                            tessellationControlShaderCode: loadSource("qrc:/shaders/subdivide.tc")
                            tessellationEvaluationShaderCode: loadSource("qrc:/shaders/subdivide-option2.te")
                            geometryShaderCode: loadSource("qrc:/shaders/wireframe.geom")
                            fragmentShaderCode: loadSource("qrc:/shaders/earth-option2.frag")
                        }
                    }
                ]
            }
        ]
    }
}
