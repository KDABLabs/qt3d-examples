#version 430

in vec4 vertexPosition;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out VertexData {
  flat int indexIntoUBO;
  vec2 texCoords;
} vOut;

const vec2 texCoordsForVertexId[] = vec2[](
    vec2(0.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 1.0),
    vec2(1.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 0.0)
);

void main()
{
    vOut.texCoords = texCoordsForVertexId[gl_VertexID % 6];
    vOut.indexIntoUBO = int(vertexPosition.w);
    gl_Position = vec4(vertexPosition.xyz, 1.0);
}
