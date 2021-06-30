#version 460
//Floor

in vec3 position;
in vec3 normal;
in vec3 colour;

out vec3 v_fragPos;
out vec3 v_normal;
out vec3 v_colour;

uniform mat4 transform;
uniform mat4 projection;

void main() {
    v_fragPos = vec3(transform * vec4(position, 1.0));
    v_normal = mat3(transpose(inverse(transform))) * normal;
    v_colour = colour;
    gl_Position = projection * vec4(v_fragPos, 1.0);
}