import Qt3D.Core 2.10
import Qt3D.Render 2.10
import Qt3D.Input 2.0
import Qt3D.Animation 2.10
import Qt3D.Extras 2.10
import QtQuick 2.9

SkinnedEntity {
    id: riggedFigure1
    property alias animationSource: clipLoader.source

    components: [
        BlendedClipAnimator {
            id: animator1
            loops: Animator.Infinite
            running: true
            blendTree: ClipBlendValue {
                clip: AnimationClipLoader { id: clipLoader }
            }
            channelMapper: ChannelMapper {
                mappings: [
                    SkeletonMapping { skeleton: riggedFigure1.skeleton }
                ]
            }

            onRunningChanged: console.log("running = " + running)
        }
    ]
}
