import Qt3D.Core 2.10
import Qt3D.Render 2.10
import Qt3D.Input 2.0
import Qt3D.Extras 2.10

Entity {
    id: root

    property Effect effect: texturedSkinnedPbrEffect
    property url source: ""
    property alias createJointsEnabled: skeleton.createJointsEnabled
    property alias transform: transform

    property alias textureBaseName: material.textureBaseName
    property alias textureBaseColor: material.textureBaseColor
    property alias baseColor: material.baseColor

    property alias rootJoint: skeleton.rootJoint
    property alias skeleton: skeleton

    components: [
        Transform {
            id: transform
        },
        Mesh {
            source: root.source
        },
        Armature {
            skeleton: SkeletonLoader {
                id: skeleton
                source: root.source
                onStatusChanged: console.log("skeleton loader status: " + status)
                onJointCountChanged: console.log("skeleton has " + jointCount + " joints")
            }
        },
        Material {
            id: material

            property string textureBaseName: ""
            property bool hasTextures: textureBaseName !== ""

            property Texture baseColorMap: TextureLoader {
                source: material.hasTextures
                        ? textureBaseName + material.textureBaseColor + "_basecolor.png"
                        : ""
                format: Texture.SRGB8_Alpha8
                mirrored: false
            }
            property Texture metalnessMap: TextureLoader {
                mirrored: false
                source: material.hasTextures ? textureBaseName + "_metallic.png" : ""
            }
            property Texture roughnessMap: TextureLoader {
                mirrored: false
                source: material.hasTextures ? textureBaseName + "_roughness.png" : ""
            }
            property Texture ambientOcclusionMap: TextureLoader {
                mirrored: false
                source: material.hasTextures ? textureBaseName + "_occlusion.png" : ""
            }

            property var baseColor: hasTextures ? Qt.rgba(1.0, 0.0, 0.0, 1.0) : baseColorMap
            property var metalness: hasTextures ? 0.1 : metalnessMap
            property var roughness: hasTextures ? 0.2 : roughnessMap
            property var ambientOcclusion: hasTextures ? "white" : ambientOcclusionMap
            property string textureBaseColor: ""

            effect: root.effect

            parameters: [
                Parameter { name: "baseColorMap"; value: material.baseColorMap },
                Parameter { name: "metalnessMap"; value: material.metalnessMap },
                Parameter { name: "roughnessMap"; value: material.roughnessMap },
                Parameter { name: "ambientOcclusionMap"; value: material.ambientOcclusionMap },
                Parameter { name: "baseColor"; value: material.baseColor },
                Parameter { name: "metalness"; value: material.metalness },
                Parameter { name: "roughness"; value: material.roughness },
                Parameter { name: "ambientOcclusion"; value: material.ambientOcclusion }
            ]
        }
    ]
}
