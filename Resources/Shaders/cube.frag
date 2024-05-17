#version 330

in vec4 materialColor;
in vec3 normalWorld;

struct Material
{
	float ambiant;
	float diffuse;
	float specular;
	float specularShinyness;
};

struct DirectionalLight
{
	vec3 dir;
	vec3 color;
};

uniform Material material;
uniform DirectionalLight light;

void main()
{
	vec4 ambiant = material.ambiant * materialColor;
	vec4 diffuse = material.diffuse * max( 0, -dot(light.dir, normalWorld.xyz)) * materialColor;
	vec4 specular = vec4(0);
	gl_FragColor = ambiant + diffuse + specular;
}
