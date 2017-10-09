#version 430

layout(triangles) in;
// 18 = 3 vertices * 6 layers of a cubemap
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 posXMat;
uniform mat4 negXMat;
uniform mat4 posYMat;
uniform mat4 negYMat;
uniform mat4 posZMat;
uniform mat4 negZMat;

uniform mat4 mvp;
uniform mat4 projMat;

in VertexData {
    flat int indexIntoUBO;
    vec2 texCoords;
} vIn[3];

out FragData {
    flat int indexIntoUBO;
    vec2 texCoords;
} fOut;

void emitTriangle(const in mat4 viewMat)
{
    gl_Position = (projMat * viewMat * gl_in[0].gl_Position);
    fOut.texCoords = vIn[0].texCoords;
    fOut.indexIntoUBO = vIn[0].indexIntoUBO;
    EmitVertex();

    gl_Position = (projMat * viewMat * gl_in[1].gl_Position);
    fOut.texCoords = vIn[1].texCoords;
    fOut.indexIntoUBO = vIn[1].indexIntoUBO;
    EmitVertex();

    gl_Position = (projMat * viewMat * gl_in[2].gl_Position);
    fOut.texCoords = vIn[2].texCoords;
    fOut.indexIntoUBO = vIn[2].indexIntoUBO;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    // We emit the triangle once per cubemap face
    gl_Layer = 0;
    emitTriangle(posXMat);

    gl_Layer = 1;
    emitTriangle(negXMat);

    gl_Layer = 2;
    emitTriangle(posYMat);

    gl_Layer = 3;
    emitTriangle(negYMat);

    gl_Layer = 4;
    emitTriangle(posZMat);

    gl_Layer = 5;
    emitTriangle(negZMat);

}
