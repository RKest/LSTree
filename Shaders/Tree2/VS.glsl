#version 460
//Floor

const int TREE_NO = 10;
const int MAX_JOINT_AMOUNT = 13;

in vec3 position;
in vec3 normal;
in vec3 colour;
in ivec3 jointIndices;

out vec3 v_fragPos;
out vec3 v_normal;
out vec3 v_colour;

uniform mat4 transform;
uniform mat4 projection;

uniform float windStrenght;
uniform vec3 windDirection;
uniform vec3 jointVectors[TREE_NO][MAX_JOINT_AMOUNT];
uniform mat4 jointTransforms[TREE_NO][MAX_JOINT_AMOUNT];
uniform int currTreeIndex;

void main() {
    mat4 windTransform = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
    // for (int i = 0; i < 3; ++i)
    // {
    //     windTransform *= jointTransforms[currTreeIndex][jointIndices[i]] *  
    //                      windStrenght * 
    //                      (1 - dot(windDirection, jointVectors[currTreeIndex][jointIndices[i]]));
    // } 

    v_fragPos = vec3(transform * windTransform * vec4(position, 1.0));


    v_normal = mat3(transpose(inverse(transform))) * normal;
    v_colour = colour;
    gl_Position = projection * vec4(v_fragPos, 1.0);
}