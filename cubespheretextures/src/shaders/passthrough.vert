#version 430 core

// Just a simple passthrough shader to the the Tessellation control shader

in vec4 vertexPositionRTE;

void main()
{
    // xyz and position and w is an index into UBO
    // remember to properly extract it that way in the tessellation shaders
    gl_Position = vertexPositionRTE;
}
