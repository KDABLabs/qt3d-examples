import QtQuick 2.7
import QtQuick.Scene3D 2.0

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import MyModule 1.0

import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1

Entity {
  id: debugCube
  readonly property Material material: Material {
    effect: Effect {
      parameters: Parameter { name: "cubeMap"; value: cubeMapTarget.satelliteCubeMap }
      techniques: [
        Technique {
          graphicsApiFilter {
            api: GraphicsApiFilter.OpenGL
            profile: GraphicsApiFilter.CoreProfile
            majorVersion: 4
            minorVersion: 3
          }
          
          renderPasses: [
            RenderPass {
              shaderProgram: ShaderProgram {
                vertexShaderCode: "
                                                #version 430
                                                in vec4 vertexPosition;

                                                uniform mat4 mvp;

                                                out vec3 color;
                                                out vec3 worldPosition;

                                                const vec3 cols[] = vec3[](
                                                    vec3(1.0, 0.0, 0.0),
                                                    vec3(1.0, 1.0, 0.0),
                                                    vec3(1.0, 1.0, 1.0),
                                                    vec3(1.0, 0.0, 1.0),
                                                    vec3(0.0, 0.0, 1.0),
                                                    vec3(0.0, 1.0, 1.0)
                                                );

                                                void main() {
                                                color = cols[gl_VertexID / 6];
                                                worldPosition = vertexPosition.xyz;
                                                gl_Position = mvp * vec4(vertexPosition.xyz, 1.0);
                                                }"
                
                fragmentShaderCode: "
                                                #version 430

                                                uniform samplerCube cubeMap;

                                                in vec3 color;
                                                in vec3 worldPosition;

                                                out vec4 fragColor;

                                                void main() {
                                                    fragColor = texture(cubeMap, normalize(-worldPosition));
                                                }
                                                "
              }
            }
          ]
        }
      ]
    }
  }
  
  components: [frameGraph.debugLayer, cubeSphere.cubeRenderer, material]
}
