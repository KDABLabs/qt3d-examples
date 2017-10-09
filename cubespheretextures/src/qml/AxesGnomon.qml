import Qt3D.Core 2.9
import Qt3D.Render 2.9
import Qt3D.Input 2.0
import Qt3D.Extras 2.9
import QtQml 2.2
import QtQuick 2.8 as QQ2

Entity {

    components: transform

    readonly property Transform transform: Transform {}
    property Layer layer


    PhongMaterial { id: red; diffuse: "red"; ambient: diffuse }
    PhongMaterial { id: green; diffuse: "green"; ambient: diffuse }
    PhongMaterial { id: blue; diffuse: "blue"; ambient: diffuse }

    GeometryRenderer {
        id: lineRenderer
        primitiveType: GeometryRenderer.LineLoop
        geometry: Geometry {
            attributes: [
                Attribute {
                    name: defaultPositionAttributeName
                    attributeType: Attribute.VertexAttribute
                    vertexBaseType: Attribute.Float
                    vertexSize: 3
                    byteOffset: 0
                    count: 2
                    buffer: Buffer {
                        type: Buffer.VertexBuffer
                        data: {
                            var vertexArray = new Float32Array(2 * 3)
                            vertexArray[0] = 0.0
                            vertexArray[1] = 0.0
                            vertexArray[2] = 0.0
                            vertexArray[3] = 1.0
                            vertexArray[4] = 0.0
                            vertexArray[5] = 0.0
                            return vertexArray
                        }
                    }
                }

            ]
        }
    }

    // X
    Entity {
        components: [
            lineRenderer,
            blue,
            layer
        ]
    }

    // Y
    Entity {
        readonly property Transform transform: Transform {
            rotationZ: 90
        }

        components: [
            transform,
            lineRenderer,
            green,
            layer
        ]
    }

    // Z
    Entity {
        readonly property Transform transform: Transform {
            rotationY: -90
        }

        components: [
            transform,
            lineRenderer,
            red,
            layer
        ]
    }
}
