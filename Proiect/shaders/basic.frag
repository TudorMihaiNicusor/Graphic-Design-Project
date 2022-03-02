#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fPosEye;
in vec3 fPos;
out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

//lights positions
uniform vec3 p_lights[8];

//components for lights
vec3 ambient;
vec3 diffuse;
vec3 specular;
float ambientStrength = 0.2f;
float specularStrength = 0.5;
float shininess = 32.0f;
float attenuation;
float constant = 1.0f;
float linear = 0.0045f;

float quadraticCasa = 0.075f;
float quadraticTorta = 0.03f;
float quadraticLuna = 0.0005f;

vec3 composed_ambient;
vec3 composed_diffuse;
vec3 composed_specular;

uniform vec3 cameraPosition;

//components for shasow
in vec4 fragPosLightSpace;
uniform sampler2D shadowMap;

//component for fog
uniform vec3 fogDensity;

void calcPointLights(int i){
		
	//transform normal
	vec3 normalEye = normalize(fNormal);	

	vec4 lightPosEye1 = view * model * vec4(p_lights[i], 1.0f);
	vec3 lightPosEye = lightPosEye1.xyz;

	//compute light direction
	vec3 lightDirN = normalize(p_lights[i] - fPos);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosition - fPos);
	vec3 reflection = normalize(reflect(lightDirN, normalEye));
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
		
	float distance = length(p_lights[i] - fPos);

	if (i == 7){
		attenuation = 1.0 / (constant + linear * distance + 
  				     quadraticLuna * (distance * distance));
		composed_ambient += attenuation * ambientStrength * vec3(1.0f, 1.0f, 1.0f);
		composed_diffuse += attenuation * max(dot(normalEye, lightDirN), 0.0f) * vec3(1.0f, 1.0f, 1.0f);
		composed_specular += attenuation * specularStrength * specCoeff * vec3(1.0f, 1.0f, 1.0f);
	}
	if (i >= 2 && i < 7){
		attenuation = 1.0 / (constant + linear * distance + 
  				     quadraticTorta * (distance * distance));
					 
		composed_ambient += attenuation * ambientStrength * vec3(1.0f, 0.549f, 1.0f);
		composed_diffuse += attenuation * max(dot(normalEye, lightDirN), 0.0f) * vec3(1.0f, 0.549f, 1.0f);
		composed_specular += attenuation * specularStrength * specCoeff * vec3(1.0f, 0.549f, 1.0f);
	}
	else{
		attenuation = 1.0 / (constant + linear * distance + 
  				     quadraticCasa * (distance * distance));
		composed_ambient += attenuation * ambientStrength * vec3(1.0f, 0.0f, 0.0f);
		composed_diffuse += attenuation * max(dot(normalEye, lightDirN), 0.0f) * vec3(1.0f, 0.0f, 0.0f);
		composed_specular += attenuation * specularStrength * specCoeff * vec3(1.0f, 0.0f, 0.0f);
	}
}

void computeDirLight(){

    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;
}

float computeShadow()
{
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    normalizedCoords = normalizedCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
    float currentDepth = normalizedCoords.z;
    float bias = max(0.05f * (1.0f - dot(fNormal, lightDir)), 0.005f);
    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

    if (normalizedCoords.z > 1.0f)
        return 0.0f;

    return shadow;
}

float computeFog()
{
 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity.x, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

void main() {
    
    computeDirLight();
    
    //compute pointlight
    for(int i=0; i<8; i++){
    	calcPointLights(i);
    }

    //compute spotlight 
    

    //compute final vertex color
    ambient += composed_ambient;
    ambient *= texture(diffuseTexture, fTexCoords).rgb;
    diffuse += composed_diffuse;
    diffuse *= texture(diffuseTexture, fTexCoords).rgb;
    specular += composed_specular;
    specular *= texture(diffuseTexture, fTexCoords).rgb;
    
    float shadow = computeShadow();
    vec3 color = min((ambient + (1.0f - shadow)*diffuse)+ (1.0f - shadow)*specular, 1.0f);
    
    float fogFactor = computeFog();
    vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
    
    fColor = mix(fogColor, vec4(color, 1.0f), fogFactor);
}
