#version 330

layout(location = 0) out vec3 out_color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 viewProjectionMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat3 normalMatrix;
uniform mat3 normalWorldMatrix;
uniform mat4 inverseViewMatrix;
uniform mat3 inverseNormalWorldMatrix;

uniform vec3 cameraPosition;
uniform vec2 cameraZLimits;

uniform sampler2D temporalFilterMap;
uniform sampler2D indirectDiffuseMap;

uniform vec2 rsmResolution;

#include "deferred.glsl"
#include "TemporalFiltering/temporalFiltering.glsl"

vec2 CalcTexCoordRSM ()
{
	return gl_FragCoord.xy / rsmResolution;
}

void main()
{
	vec2 texCoord = CalcTexCoordRSM();
	vec3 in_position = textureLod (gPositionMap, texCoord, 0).xyz;

	out_color = CalcTemporalFiltering (temporalFilterMap, indirectDiffuseMap, rsmResolution,
		in_position, texCoord, false);
}
