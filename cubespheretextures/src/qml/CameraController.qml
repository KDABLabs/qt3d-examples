
import Qt3D.Core 2.9
import Qt3D.Render 2.9
import Qt3D.Input 2.1
import Qt3D.Logic 2.0
import QtQml 2.2
import QtQuick 2.5

Entity {
    id: root
    property Camera camera
    property real linearSpeed: 10.0 / Math.max(1, currentZoom)
    property real lookSpeed: 180.0 / Math.max(1, currentZoom)
    property real zoomLimit: 2.0

    // TO DO: Show camera latitude / longitude of view vector
    readonly property real latitude: 0.0
    readonly property real longitude: 0.0

    QtObject {
        id: d
        readonly property vector3d firstPersonUp: Qt.vector3d(0, 1, 0)
        readonly property bool leftMouseButtonPressed: leftMouseButtonAction.active
        property real orbitX: clampInputs(leftMouseButtonPressed ? mouseXAxis.value : 0, 0) * lookSpeed;
        property real orbitY: clampInputs(leftMouseButtonPressed ? mouseYAxis.value : 0, 0) * lookSpeed;
    }

    function clampInputs(input1, input2) {
        var axisValue = input1 + input2;
        return (axisValue < -1) ? -1 : (axisValue > 1) ? 1 : axisValue;
    }

    function zoomDistance(firstPoint, secondPoint) {
        var u = secondPoint.minus(firstPoint); u = u.times(u);
        return u.x + u.y + u.z;
    }

    readonly property real maxAltitude: 10000000 // 10 000 km
    readonly property real minAltitude: 1000// km
    readonly property real altitudeRange: maxAltitude - minAltitude
    readonly property vector3d ellipsoidRadius: Qt.vector3d(6378137.0, 6356752.3, 6378137.0)

    readonly property int maxZoom: 18
    property int currentZoom: 1

    function zoomSequence(step, altitude) {
        if (altitude === undefined)
            altitude = altitudeRange

        if (step > 0)
            return zoomSequence(step - 1, altitude / 2)
        return altitude;
    }

    KeyboardDevice {
        id: keyboardSourceDevice
    }

    MouseDevice {
        id: mouseSourceDevice
        sensitivity: 0.1
    }

    components: [

        LogicalDevice {
            enabled: root.enabled
            actions: [
                Action {
                    id: leftMouseButtonAction
                    ActionInput {
                        sourceDevice: mouseSourceDevice
                        buttons: [MouseEvent.LeftButton]
                    }
                }
            ] // actions

            axes: [
                // Mouse
                Axis {
                    id: mouseXAxis
                    AnalogAxisInput {
                        sourceDevice: mouseSourceDevice
                        axis: MouseDevice.X
                    }
                },
                Axis {
                    id: mouseYAxis
                    AnalogAxisInput {
                        sourceDevice: mouseSourceDevice
                        axis: MouseDevice.Y
                    }
                },
                Axis {
                    id: mouseWheel
                    AnalogAxisInput {
                        sourceDevice: mouseSourceDevice
                        axis: MouseDevice.WheelY
                    }

                    onValueChanged: {
                        currentZoom = Math.min(Math.max(currentZoom + Math.sign(value), 0), maxZoom)
                        altitudeForZoom = zoomSequence(currentZoom)
                        console.log("AltitudeForZoom(" + currentZoom + ") = " + altitudeForZoom)
                    }
                }
            ] // axes
        },
        FrameAction {
            onTriggered: {
                // The time difference since the last frame is passed in as the
                // argument dt. It is a floating point value in units of seconds.

                // Mouse input
                if (d.leftMouseButtonPressed) {
                    // Orbit
                    root.camera.panAboutViewCenter(d.orbitX * dt, d.firstPersonUp);
                    root.camera.tiltAboutViewCenter(d.orbitY * dt);
                    root.camera.viewCenter = Qt.vector3d(0.0, 0.0, 0.0)
                }
            }
        }
    ] // components

    property real altitudeForZoom: 0.0
    Behavior on altitudeForZoom { NumberAnimation { duration: 450; easing.type: Easing.InOutQuad } }

    readonly property vector3d cameraDirVector: root.camera ? root.camera.position.minus(root.camera.viewCenter) : Qt.vector3d(1, 1, 1)
    readonly property real cameraDistanceToCenter: cameraDirVector.length()
    readonly property vector3d cameraDirVectorUnit: cameraDirVector.normalized()

    onAltitudeForZoomChanged: {
        root.camera.position = root.camera.viewCenter.plus(cameraDirVectorUnit.times(altitudeForZoom)).plus(cameraDirVectorUnit.times(ellipsoidRadius));
    }
}
