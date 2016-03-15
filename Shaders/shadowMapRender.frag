#version 410

out vec4 outputColor;

uniform sampler2D shadowMap;

smooth in vec2 vCoord;

void main()
{
	float depth = texture2D(shadowMap, vCoord).r;

	outputColor = vec4(depth/2, depth/2, depth/2, 1.0);
}
