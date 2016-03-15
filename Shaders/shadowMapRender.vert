#version 410

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;

uniform mat4 mMat;
uniform mat4 projMat;

smooth out vec2 vCoord;

void main()
{
    //kerrotaan normaalisti inpos  jos ei toimi
	gl_Position = vec4(inPosition* 2.0 - 1.0 , 1.0);
    vCoord = inCoord;
}