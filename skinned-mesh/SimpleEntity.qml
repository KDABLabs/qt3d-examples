import Qt3D.Core 2.10
import Qt3D.Render 2.10
import Qt3D.Input 2.0
import Qt3D.Animation 2.10
import Qt3D.Extras 2.10
import QtQuick 2.9

Entity {
    id: simpleEntity

    property alias source: mesh.source
    property alias transform: transform
    property alias baseColor: material.baseColor

    components: [
        Transform {
            id: transform
        },
        Mesh {
            id: mesh
        },
        MetalRoughMaterial {
            id: material
        }
    ]
}
