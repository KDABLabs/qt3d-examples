#version 430 core

layout( triangles ) in;
// 6 vertex to make up 3 lines * 6 layers
layout( line_strip, max_vertices = 36 ) out;

uniform mat4 posXMat;
uniform mat4 negXMat;
uniform mat4 posYMat;
uniform mat4 negYMat;
uniform mat4 posZMat;
uniform mat4 negZMat;

uniform mat4 projMat;

void drawWireframe(const in mat4 viewMat)
{
    gl_Position = (projMat * viewMat * gl_in[0].gl_Position);
    EmitVertex();
    gl_Position = (projMat * viewMat * gl_in[1].gl_Position);
    EmitVertex();
    EndPrimitive();

    gl_Position = (projMat * viewMat * gl_in[1].gl_Position);
    EmitVertex();
    gl_Position = (projMat * viewMat * gl_in[2].gl_Position);
    EmitVertex();
    EndPrimitive();

    gl_Position = (projMat * viewMat * gl_in[2].gl_Position);
    EmitVertex();
    gl_Position = (projMat * viewMat * gl_in[0].gl_Position);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    gl_Layer = 0;
    drawWireframe(posXMat);

    gl_Layer = 1;
    drawWireframe(negXMat);

    gl_Layer = 2;
    drawWireframe(posYMat);

    gl_Layer = 3;
    drawWireframe(negYMat);

    gl_Layer = 4;
    drawWireframe(posZMat);

    gl_Layer = 5;
    drawWireframe(negZMat);
}
