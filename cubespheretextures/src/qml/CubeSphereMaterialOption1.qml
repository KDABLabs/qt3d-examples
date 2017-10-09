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
    property alias satelliteUniformBuffer: satelliteUBOParam.value
    property alias elevationUniformBuffer: elevationUBOParam.value
    property alias satelliteTexture: satelliteTextureParam.value
    property alias elevationTexture: elevationTextureParam.value

    parameters: [
        Parameter { id: satelliteUBOParam; name: "ImageryMappingBlock" },
        Parameter { id: elevationUBOParam; name: "ElevationMappingBlock" },
        Parameter { id: satelliteTextureParam; name: "satelliteImagery" },
        Parameter { id: elevationTextureParam; name: "elevation" },
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
                            tessellationEvaluationShaderCode: loadSource("qrc:/shaders/subdivide-option1.te")
                            geometryShaderCode: loadSource("qrc:/shaders/wireframe.geom")
                            fragmentShaderCode: loadSource("qrc:/shaders/earth-option1.frag")
                        }
                    }
                ]
            }
        ]
    }
}
