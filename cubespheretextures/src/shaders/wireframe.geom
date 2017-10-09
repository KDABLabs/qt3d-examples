#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VertexData {
  vec3 color;
  vec2 texCoords;
  vec3 eyePosition;
  vec3 worldPosition;
} vertexIn[3];

in TesselData {
  vec4 patchDistance;
  flat int idxIntoSatelliteTextureInfo;
} tesselIn[3];

out FragData {
  vec3 color;
  vec3 triPosition;
  vec2 texCoords;
  vec4 patchDistance;
  vec3 eyePosition;
  vec3 normal;
  vec3 worldPosition;
  flat int idxIntoSatelliteTextureInfo;
} fragOut;

void main(void)
{
    fragOut.color = vertexIn[0].color;
    fragOut.texCoords = vertexIn[0].texCoords;
    fragOut.patchDistance = tesselIn[0].patchDistance;
    fragOut.triPosition = vec3(1, 0, 0);
    fragOut.eyePosition = vertexIn[0].eyePosition;
    fragOut.worldPosition = vertexIn[0].worldPosition;
    fragOut.normal = -normalize(vertexIn[0].eyePosition);
    fragOut.idxIntoSatelliteTextureInfo = tesselIn[0].idxIntoSatelliteTextureInfo;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    fragOut.color = vertexIn[1].color;
    fragOut.texCoords = vertexIn[1].texCoords;
    fragOut.patchDistance = tesselIn[1].patchDistance;
    fragOut.triPosition = vec3(0, 1, 0);
    fragOut.eyePosition = vertexIn[1].eyePosition;
    fragOut.worldPosition = vertexIn[1].worldPosition;
    fragOut.normal = -normalize(vertexIn[1].eyePosition);
    fragOut.idxIntoSatelliteTextureInfo = tesselIn[1].idxIntoSatelliteTextureInfo;
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    fragOut.color = vertexIn[2].color;
    fragOut.texCoords = vertexIn[2].texCoords;
    fragOut.patchDistance = tesselIn[2].patchDistance;
    fragOut.triPosition = vec3(0, 0, 1);
    fragOut.eyePosition = vertexIn[2].eyePosition;
    fragOut.worldPosition = vertexIn[2].worldPosition;
    fragOut.normal = -normalize(vertexIn[2].eyePosition);
    fragOut.idxIntoSatelliteTextureInfo = tesselIn[2].idxIntoSatelliteTextureInfo;
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}
