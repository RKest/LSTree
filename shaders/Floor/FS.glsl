#version 460 
//Floor
out vec4 FragColor;

in vec3 v_fragPos;
in vec3 v_normal;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform vec3 viewPos;
uniform Light light;

uniform samplerCube depthMap;
uniform float far_plane;

float bias = 0.25;

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);
/*---------------------------------------------------NOT-------------------------------------------------*/

float calcShadows(vec3 fragPos)
{
    vec3 fragToLight = fragPos - light.position;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    
    return 1.0 - shadow;
}

void main() {
    vec3 tex = vec3(0.6, 0.46, 0.32);
    vec3 norm = normalize(v_normal);

    vec3 lightDir = normalize(light.position - v_fragPos);
    vec3 viewDir = normalize(viewPos - v_fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    //ambient
    vec3 ambient = light.ambient * tex;

    //diffuse
    float diff = max(dot(norm, halfwayDir), 0.0);
    vec3 diffuse = light.diffuse * diff * tex;

    //specular

    //Non-Blinn phoning
    // vec3 reflectDir = reflect(-lightDir, norm);
    // float spec = max(dot(viewDir, reflectDir), 0.0);

    //Blinn phoning
    float spec = max(dot(norm, halfwayDir), 0.0);
    vec3 specular = light.specular * spec * tex;

    //attenuation
    float dist = length(normalize(light.position) - normalize(v_fragPos));
    float attenuation = 1.0 / light.constant + (light.linear * dist) + (light.quadratic * (dist * dist));

    float shadow = calcShadows(v_fragPos);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 res = (shadow * (diffuse + specular) + ambient) * tex;
    FragColor = vec4(res, 1.0);
}