#version 330 core

out vec4 FragColor;
in vec3 pass_color;

void main()
{
   FragColor = vec4(pass_color.zyx, 1.0);
}
