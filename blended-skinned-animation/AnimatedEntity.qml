import Qt3D.Core 2.10
import Qt3D.Render 2.10
import Qt3D.Input 2.0
import Qt3D.Animation 2.10
import Qt3D.Extras 2.10
import QtQuick 2.9

SkinnedEntity {
    id: riggedFigure1
    property alias running: animator1.running
    property alias startClip: startClipValue.clip
    property alias endClip: endClipValue.clip
    property alias blendFactor: lerp.blendFactor
    property alias playbackRate: animationClock.playbackRate

    function start() {
        timer.running = true
    }

    Timer {
        id: timer
        interval: 1000
        repeat: false
        running: true
        onTriggered: animator1.running = true
    }

    components: [
        BlendedClipAnimator {
            id: animator1
            loops: Animator.Infinite
            channelMapper: ChannelMapper {
                mappings: [ SkeletonMapping { skeleton: riggedFigure1.skeleton } ]
            }
            clock: Clock { id: animationClock }

            blendTree: LerpClipBlend {
                id: lerp
                startClip: ClipBlendValue { id: startClipValue }
                endClip: ClipBlendValue { id: endClipValue }
            }
        }
    ]
}
