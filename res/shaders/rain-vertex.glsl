#version 330

layout(location = 0)in vec4 vert;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 fragCoord;

void main()
{
    gl_Position = projection * view * model * vert;
    fragCoord = gl_Position.xy;
}