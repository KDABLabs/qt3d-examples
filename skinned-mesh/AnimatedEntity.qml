import Qt3D.Core 2.10
import Qt3D.Render 2.10
import Qt3D.Input 2.0
import Qt3D.Animation 2.10
import Qt3D.Extras 2.10
import QtQuick 2.9

SkinnedEntity {
    id: riggedFigure1
    property alias clip: animator1.clip
    property alias playbackRate: animationClock.playbackRate

    Timer {
        interval: 500
        repeat: false
        running: true
        onTriggered: animator1.running = true
    }

    components: [
        ClipAnimator {
            id: animator1
            loops: Animator.Infinite
            channelMapper: ChannelMapper {
                mappings: [ SkeletonMapping { skeleton: riggedFigure1.skeleton } ]
            }
            clock: Clock { id: animationClock }
            onRunningChanged: console.log("running = " + running)
        }
    ]
}
