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

uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform float lightIntensity;

#include "deferred.glsl"
#include "ReflectiveShadowMapping/reflectiveShadowMapping.glsl"

vec3 CalcDirectDiffuseLight (vec3 in_position, vec3 in_normal)
{
	// Diffuse contribution
	float dCont = max (dot (in_normal, -lightDirection), 0.0);

	// Attenuation is 1.0 for Directional Lights
	vec3 diffuseColor = lightColor * dCont;

	return diffuseColor;
}

vec3 CalcDirectSpecularLight (vec3 in_position, vec3 in_normal, float in_shininess)
{
	vec3 surface2view = normalize (-in_position);
	vec3 reflection = reflect (lightDirection, in_normal);

	// Specular contribution
	float sCont = pow (max (dot (surface2view, reflection), 0.0), in_shininess);

	vec3 specularColor = lightColor * sCont;

	return specularColor;
}

vec3 CalcDirectionalLight (vec3 in_position, vec3 in_normal, vec3 in_diffuse,
	vec3 in_specular, float in_shininess)
{
	vec3 directDiffuseColor = CalcDirectDiffuseLight (in_position, in_normal);

	float shadow = CalcShadowContribution (in_position);

	directDiffuseColor = shadow * directDiffuseColor;

	vec3 directSpecularColor = CalcDirectSpecularLight (in_position, in_normal, in_shininess);

	directSpecularColor = shadow * directSpecularColor;

	return (directDiffuseColor * in_diffuse
			+ directSpecularColor * in_specular) * lightIntensity;
}

void main()
{
	vec2 texCoord = CalcTexCoord();
	vec3 in_position = textureLod (gPositionMap, texCoord, 0).xyz;

	out_color = vec3 (0.0);

	if (in_position.z > -cameraZLimits.y) {
		vec3 in_diffuse = textureLod (gDiffuseMap, texCoord, 0).xyz;
		vec3 in_normal = textureLod (gNormalMap, texCoord, 0).xyz;
		vec3 in_specular = textureLod (gSpecularMap, texCoord, 0).xyz;
		float in_shininess = textureLod (gSpecularMap, texCoord, 0).w;

		in_normal = normalize(in_normal);

		out_color = CalcDirectionalLight(in_position, in_normal, in_diffuse, in_specular, in_shininess);
	}
} 