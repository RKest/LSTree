#version 460 
// Wolverine
out vec4 out_colour;

in vec3 v_fragPos;
in vec2 v_texCoord;
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
uniform sampler2D diffuse;
uniform Light light;

void main() {
    vec3 tex = texture(diffuse, v_texCoord).rgb;
    vec3 lightDir = normalize(light.position - v_fragPos);
    vec3 viewDir = normalize(viewPos - v_fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 norm = normalize(v_normal);
    //ambient
    vec3 ambient = light.ambient * tex;

    //diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * tex;

    // specular
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = max(dot(viewDir, reflectDir), 0.0);
    vec3 specular = light.specular * spec * diffuse * tex;

    //attenuation
    float dist = length(normalize(light.position) - normalize(v_fragPos));
    float attenuation = 1.0 / light.constant + (light.linear * dist) + (light.quadratic * (dist * dist));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 res = ambient + diffuse + specular;
    out_colour = vec4(res, 1.0);
}