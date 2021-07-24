#version 460

//Must be consistant with the ShadowMap shader and _config.h
const int TREE_NO = 5;
const int MAX_JOINT_AMOUNT = 13;
const int SWIWEL_AXEE = 2;

in vec3 position;
in ivec3 jointIndices;

uniform mat4 transform;
uniform mat4 projection;

uniform float windAffectionAngle;
uniform vec3 windDirectionVector;

uniform float windStrenght;
uniform mat4 jointBaseTransforms[TREE_NO][MAX_JOINT_AMOUNT];
uniform vec3 jointVectors[TREE_NO][MAX_JOINT_AMOUNT];
uniform int currTreeIndex;

//Courtasy of Neil Mendoza at this url http://www.neilmendoza.com/glsl-rotation-about-an-arbitrary-axis/
mat4 rotationMatrix(float angle, vec3 axis) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

//axisInx must be from the range 0 to 2 inclusive
vec3 perpendicularVec(vec3 vec, int axisInx){
    bool firstPassFlag = true;
    int firstPassInx;

    vec3 returnVec;
    returnVec[axisInx] = vec[axisInx];
    for(int i = 0; i < 3; ++i)
    {
        if(axisInx == i)
            continue;

        if(firstPassFlag)
        {
            firstPassFlag = false;
            firstPassInx = i;
            returnVec[i] = 1;
            continue;
        }

        returnVec[i] = - ((vec[firstPassInx] * returnVec[firstPassInx]) / vec[i]);
    }

    normalize(returnVec);
    return returnVec;
}

mat4 windTransform(){
    mat4 windTransform = {{1.0,0.0,0.0,0.0}, {0.0,1.0,0.0,0.0}, {0.0,0.0,1.0,0.0}, {0.0,0.0,0.0,1.0}};
    for (int i = 0; i < 3; ++i)
    {
        if(windStrenght < 0.001)
            break;

        int affectedIndex = jointIndices[i];
        
        if(affectedIndex <= 0 || affectedIndex >= MAX_JOINT_AMOUNT)
            continue;

        vec3 affectedVec = normalize(jointVectors[currTreeIndex][affectedIndex]);
        mat4 affectedMat = jointBaseTransforms[currTreeIndex][affectedIndex];

        float windAcutenessFactor = dot(affectedVec, windDirectionVector);
        float scaledAngle = windAffectionAngle * windStrenght * windAcutenessFactor;

        vec3 rotationVector = perpendicularVec(affectedVec, SWIWEL_AXEE);

        mat4 jointTransform = affectedMat * rotationMatrix(scaledAngle, rotationVector) * inverse(affectedMat);

        windTransform *= jointTransform;
    }

    return windTransform;
}

void main()
{
    mat4 windTransform = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
    // ---------------------------- UNCOMMENT FOR THE WIND TO AFFECT THE SHADOW ----------------------------------//
    // windTransform *= windTransform();
    gl_Position = projection * windTransform * transform * vec4(position, 1.0);
}