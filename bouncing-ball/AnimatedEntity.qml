import Qt3D.Core 2.10
import Qt3D.Render 2.10
import Qt3D.Extras 2.10
import Qt3D.Animation 2.10
import QtQuick 2.9

Entity {
    id: animatedEntity

    property alias source: mesh.source
    property alias transform: transform
    property alias baseColor: material.baseColor
    property string clipSource

    components: [
        Transform {
            id: transform
        },
        Mesh {
            id: mesh
        },
        MetalRoughMaterial {
            id: material
            metalness: 0
            roughness: 0.9
        },
        ClipAnimator {
            id: animator
            running: true
            loops: Animator.Infinite
            clip: AnimationClipLoader { source: animatedEntity.clipSource }
            channelMapper: ChannelMapper {
                mappings: [
                    ChannelMapping { channelName: "Location"; target: transform; property: "translation" },
                    ChannelMapping { channelName: "Rotation"; target: transform; property: "rotation" },
                    ChannelMapping { channelName: "Scale"; target: transform; property: "scale3D" }
                ]
            }
        }
    ]
}
