#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 vertexColor;
out vec3 pass_color;
uniform mat4 projection;

void main()
{
   gl_Position = projection * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   pass_color = vertexColor;
}
