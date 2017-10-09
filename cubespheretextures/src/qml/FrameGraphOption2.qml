import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0

RenderSurfaceSelector {
    property alias camera: cameraSelector.camera
    property alias debugCamera: debugCameraSelector.camera
    property alias cubeMapRenderTarget: cubeMapRenderTargetSelector.target
    property bool debugEnabled: false
    readonly property Layer firstPassLayer: Layer {}
    readonly property Layer finalPassLayer: Layer {}
    readonly property Layer debugLayer: Layer {}
    readonly property Layer axisGnomonLayer: Layer {}

    Viewport {
        id: mainViewport

        // Render Cube into CubeMap to build cubemap textures for the sphere
        LayerFilter {
            layers: firstPassLayer
            RenderTargetSelector {
                id: cubeMapRenderTargetSelector
                ClearBuffers {
                    buffers: ClearBuffers.ColorDepthBuffer
                    clearColor: "transparent"
                    RenderStateSet {
                        renderStates: [
                            // Cull front Faces as we render from within the cube
                            CullFace { mode: CullFace.NoCulling },
                            DepthTest { depthFunction: DepthTest.LessOrEqual }
                        ]
                    }
                }
            }
        }

        ClearBuffers {
            buffers: ClearBuffers.ColorDepthBuffer
            clearColor: "white"
            NoDraw {}
        }

        // Render the Sphere
        CameraSelector {
            id: cameraSelector
            LayerFilter {
                layers: finalPassLayer

                RenderStateSet {
                    renderStates: [
                        CullFace { mode: CullFace.Back },
                        DepthTest { depthFunction: DepthTest.Less }
                    ]
                }
            }

            // Small viewport for AxesGnomon
            Viewport {
                normalizedRect: Qt.rect(0.0, 0.7, 0.2, 0.2)
                RenderStateSet {
                    renderStates: CullFace { mode: CullFace.NoCulling }
                    LayerFilter {
                        layers: axisGnomonLayer
                    }
                }
            }
        }


        // Debug viewport
        Viewport {
            normalizedRect: Qt.rect(0.0, 0.0, 0.5, 0.5)
            ClearBuffers {
                buffers: ClearBuffers.DepthBuffer
                NoDraw {}
            }
            LayerFilter {
                layers: debugLayer
                CameraSelector {
                    id: debugCameraSelector
                    RenderStateSet {
                        renderStates: CullFace { mode: CullFace.NoCulling }
                        LayerFilter {
                            layers: axisGnomonLayer
                        }
                        NoDraw {
                            enabled: !debugEnabled
                        }
                    }
                }
            }
        }

    }
}
