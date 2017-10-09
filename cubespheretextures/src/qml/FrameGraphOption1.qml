import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0

RenderSurfaceSelector {
    property alias camera: cameraSelector.camera
    readonly property Layer finalPassLayer: Layer {}
    readonly property Layer axisGnomonLayer: Layer {}

    Viewport {
        id: mainViewport

        ClearBuffers {
            buffers: ClearBuffers.ColorDepthBuffer
            clearColor: "white"
            NoDraw {}
        }

        CameraSelector {
            id: cameraSelector

            // Render the Sphere
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
    }
}
