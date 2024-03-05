#version 330 core

struct DirectionalLight {
	vec3 Direction;
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
};

struct Material {
	sampler2D Kd;
	sampler2D Ks;
	float Shininess;
};

uniform DirectionalLight uDirLight;
uniform vec3 uViewPos;
uniform Material uMaterial;

uniform sampler2D dudvMap;
uniform float moveFactor;
const float waveStrength = 0.02f;

in vec3 vWorldSpaceFragment;
in vec3 vWorldSpaceNormal;
in vec3 vCol;
in vec2 UV;

out vec4 FragColor;

void main() {
	vec3 ViewDirection = normalize(uViewPos - vWorldSpaceFragment);
	vec3 DirLightVector = normalize(-uDirLight.Direction);
	float DirDiffuse = dot(vWorldSpaceNormal, DirLightVector);
	vec3 DirReflectDirection = reflect(-DirLightVector, vWorldSpaceNormal);
	float DirSpecular = pow(max(dot(ViewDirection, DirReflectDirection), 0.0f), uMaterial.Shininess);

	vec2 distortion1 = (texture(dudvMap, vec2(UV.x + moveFactor, UV.y)).rg * 2.0f - 1.0f) * waveStrength;
	vec2 distortion2 = (texture(dudvMap, vec2(-UV.x + moveFactor, UV.y + moveFactor)).rg * 2.0f - 1.0f) * waveStrength;
	vec2 textureCoords = UV;
	
	textureCoords += distortion1 + distortion2; 
	textureCoords = clamp(textureCoords, 0.001f, 0.999f);

	vec3 DirAmbientColor = uDirLight.Ka * vec3(texture(uMaterial.Kd, textureCoords));
	vec3 DirDiffuseColor = uDirLight.Kd * DirDiffuse * vec3(texture(uMaterial.Kd, textureCoords));
	vec3 DirSpecularColor = uDirLight.Ks * DirSpecular * vec3(texture(uMaterial.Ks, textureCoords));
	vec3 DirColor = DirAmbientColor + DirDiffuseColor + DirSpecularColor;


	vec3 FinalColor = DirColor + vCol;
	FragColor = vec4(FinalColor, 1.0f);
	//FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
}