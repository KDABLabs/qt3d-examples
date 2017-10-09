import Qt3D.Core 2.0
import Qt3D.Render 2.0

RenderTarget {
    readonly property int targetWidth: 1024
    readonly property int targetHeight: 1024


    readonly property TextureCubeMap satelliteCubeMap: TextureCubeMap {
        width: targetWidth; height: targetHeight
        format: Texture.RGBA8_UNorm
        generateMipMaps: false
        magnificationFilter: Texture.Linear
        minificationFilter: Texture.Linear
        wrapMode {
            x: WrapMode.ClampToEdge
            y: WrapMode.ClampToEdge
        }
    }

    readonly property TextureCubeMap elevationCubeMap: TextureCubeMap {
        width: targetWidth; height: targetHeight
        format: Texture.RGBA8_UNorm
        generateMipMaps: false
        magnificationFilter: Texture.Linear
        minificationFilter: Texture.Linear
        wrapMode {
            x: WrapMode.ClampToEdge
            y: WrapMode.ClampToEdge
        }
    }

    readonly property TextureCubeMap depthCubeMap: TextureCubeMap {
        width: targetWidth; height: targetHeight
        format: Texture.D32F
        generateMipMaps: false
        magnificationFilter: Texture.Linear
        minificationFilter: Texture.Linear
        wrapMode {
            x: WrapMode.ClampToEdge
            y: WrapMode.ClampToEdge
        }
    }

    attachments: [
        RenderTargetOutput {
            attachmentPoint: RenderTargetOutput.Color0
            texture: satelliteCubeMap
            face: Texture.AllFaces
        },
        RenderTargetOutput {
            attachmentPoint: RenderTargetOutput.Color1
            texture: elevationCubeMap
            face: Texture.AllFaces
        },
        RenderTargetOutput {
            attachmentPoint: RenderTargetOutput.Depth
            texture: depthCubeMap
            face: Texture.AllFaces
        }
    ]
}
