#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;

const vec3 aNormal = vec3(0.0f, 1.0f, 0.0f);
const vec3 aCol = vec3(0.3f); // 0.1, 0.2, 0.6

uniform float amplitude;
uniform float frequency;
uniform float speed;
uniform float steepness;

const float rippleFrequency = 4.0f;
const float rippleSpeed = 5.0f;
const float PI = 3.14159;
const float e = 2.71828;
const int MAX_CENTERS = 100;
const float decayFactor = 0.5f;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

uniform int maxCenters;
uniform vec3 centers[MAX_CENTERS];
uniform float timers[MAX_CENTERS];
uniform float amplitudes[MAX_CENTERS];
uniform float time;

uniform vec2 direction;
uniform int waves;
uniform int colors;

out vec3 vWorldSpaceFragment;
out vec3 vWorldSpaceNormal;
out vec3 vCol;
out vec2 UV;

vec3 calculateGerstnerWave(vec3 vertexPosition, vec2 direction, float amplitude, float wavelength, float steepness, float speed, float time) {
    vec2 waveDirectionXZ = normalize(direction);
    float waveNumber = 2.0 * PI / wavelength;
    float phase = waveNumber * dot(vertexPosition.xz, waveDirectionXZ) + speed * time;
    float cosPhase = cos(phase);
    float sinPhase = sin(phase);
    float Q = steepness / waveNumber;

    vec3 displacement;
    displacement.x = waveDirectionXZ.x * Q * amplitude * cosPhase;
    displacement.z = waveDirectionXZ.y * Q * amplitude * cosPhase;
    displacement.y = amplitude * sinPhase;

    return displacement;
}

void main()
{ 
	vec3 finalPosition = aPos;
	vCol = aCol;
	vec3 updatedNormal = normalize(mat3(transpose(inverse(uModel))) * aNormal);

	for (int i = 0; i < MAX_CENTERS; i++)
	{
		if (i < maxCenters && timers[i] < 30){
			vec3 center = centers[i];
			float distance = distance(aPos, center) / 10;
			float attenuation = 1.0 / (distance + amplitudes[i]);
			float decay = pow(decayFactor, timers[i]);
			float y = amplitudes[i] * attenuation * decay * sin(-1 * PI * distance * rippleFrequency + time * rippleSpeed);
			finalPosition.y += y;

			float cDecay = pow(decayFactor, timers[i] / 5.0f);
			float colorDecay = clamp(amplitudes[i] * pow(attenuation, 2) * (1 - cDecay) * 10.0f, 0.0f, 1.0f);
			vCol += (vec3(0.0f) - aCol) * mix(0.0f, colorDecay, 2 * cDecay);
		}
	}
	if (waves == 1){
		int numWaves = 2;
		float waveAmplitudes[2];
		float waveWavelengths[3];
		float waveSteepness[2];
		vec2 waveDirections[2];

		waveAmplitudes[0] = 2.0f * amplitude;
		waveWavelengths[0] = 50.0f * 1/frequency;
		waveSteepness[0] = 1.3f * steepness;
		waveDirections[0] = direction;

		waveAmplitudes[1] = amplitude;
		waveWavelengths[1] = 75.0f * 1/frequency;
		waveSteepness[1] = steepness;
		waveDirections[1] = direction + vec2(-0.3f, 0.7f);

		vec3 totalDisplacement = vec3(0.0);
		for (int i = 0; i < numWaves; i++) {
			totalDisplacement += calculateGerstnerWave(finalPosition, waveDirections[i], waveAmplitudes[i], waveWavelengths[i], waveSteepness[i], speed, time);
		}
		finalPosition += totalDisplacement;
		//float displacement = 2 * amplitude * sin(-5 * PI * frequency * dot(aPos.xz, direction) + speed * time);
		//finalPosition.y += displacement;
	}

	if (colors == 0){
		vCol = aCol;
		vCol += max(finalPosition.y, 0) * vec3(0.05f);
	}

    updatedNormal += normalize(mat3(transpose(inverse(uModel))) * vec3(finalPosition.x, finalPosition.y, finalPosition.z));
    updatedNormal = normalize(updatedNormal);

    vWorldSpaceFragment = vec3(uModel * vec4(finalPosition, 1.0));
    vWorldSpaceNormal = updatedNormal;
	UV = aUV;

	gl_Position = uProjection * uView * uModel * vec4(finalPosition, 1);
}
