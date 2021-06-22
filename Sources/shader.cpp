#include "shader.h"
#include "glm/gtx/string_cast.hpp"
#include <fstream>
#include <iostream>

//zxnm12OP

static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string &errorMessage);
static std::string LoadShader(const std::string &fileName);
static GLuint CreateShader(const std::string &text, GLenum shaderType);

Shader::Shader(const std::string &filePath, const std::string *paramNames, unsigned int noParams, const std::string *uniformNames, unsigned int noUniforms, bool hasGeometryShader)
{
    program = glCreateProgram();
    shaders.push_back(CreateShader(LoadShader(filePath + "/VS.glsl"), GL_VERTEX_SHADER));
    shaders.push_back(CreateShader(LoadShader(filePath + "/FS.glsl"), GL_FRAGMENT_SHADER));

    if (hasGeometryShader)
        shaders.push_back(CreateShader(LoadShader(filePath + "/GS.glsl"), GL_GEOMETRY_SHADER));

    for (unsigned int i = 0; i < shaders.size(); i++)
        glAttachShader(program, shaders[i]);

    for (unsigned int i = 0; i < noParams; i++)
        glBindAttribLocation(program, i, paramNames[i].c_str());

    glLinkProgram(program);
    CheckShaderError(program, GL_LINK_STATUS, true, "ERROR: Program linking failed");
    glValidateProgram(program);
    CheckShaderError(program, GL_VALIDATE_STATUS, true, "ERROR: Program is invalid");

    for (unsigned int i = 0; i < noUniforms; i++)
        uniforms[i] = glGetUniformLocation(program, uniformNames[i].c_str());
}

Shader::~Shader()
{
    for (unsigned int i = 0; i < shaders.size(); i++)
    {
        glDetachShader(program, shaders[i]);
        glDeleteShader(shaders[i]);
    }
    glDeleteProgram(program);
}

void Shader::Bind()
{
    glUseProgram(program);
}

void Shader::Update(const Transform &transform, const Camera &camera)
{
    glm::mat4 model = transform.Model();
    glm::mat4 projection = camera.ViewProjection();

    SetMat4(TRANSFORM_U, model);
    SetMat4(PROJECTION_U, projection);
}

void Shader::Update(const Transform &transform)
{
    glm::mat4 model = transform.Model();

    SetMat4(TRANSFORM_U, model);
    SetMat4(PROJECTION_U, blankTransform);
}

static std::string LoadShader(const std::string &fileName)
{
    std::ifstream file;
    file.open((fileName).c_str());

    std::string output;
    std::string line;

    if (file.is_open())
    {
        while (file.good())
        {
            getline(file, line);
            output.append(line + '\n');
        }
    }
    else
    {
        std::cerr << "Unable to open shader: " << fileName << std::endl;
    }

    return output;
}

static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string &errorMessage)
{
    GLint success = 0;
    GLchar error[1024] = {0};

    if (isProgram)
        glGetProgramiv(shader, flag, &success);
    else
        glGetShaderiv(shader, flag, &success);

    if (success == GL_FALSE)
    {
        if (isProgram)
            glGetProgramInfoLog(shader, sizeof(error), NULL, error);
        else
            glGetShaderInfoLog(shader, sizeof(error), NULL, error);

        std::cerr << errorMessage << ": '" << error << "'" << std::endl;
    }
}

static GLuint CreateShader(const std::string &text, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);

    if (shader == 0)
        std::cerr << "ERROR: Shader creation failed" << std::endl;

    const GLchar *shaderSourceStrings[1];
    GLint shaderSourceStringLengths[1];

    shaderSourceStrings[0] = text.c_str();
    shaderSourceStringLengths[0] = text.length();

    glShaderSource(shader, 1, shaderSourceStrings, shaderSourceStringLengths);
    glCompileShader(shader);

    CheckShaderError(shader, GL_COMPILE_STATUS, false, "ERROR: Shader failed to compile");

    return shader;
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &arg)
{
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &arg[0][0]);
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &arg)
{
    glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &arg[0]);
}

void Shader::SetFloat(const std::string &name, float arg)
{
    glUniform1f(glGetUniformLocation(program, name.c_str()), arg);
}

void Shader::SetInt(const std::string &name, unsigned int arg)
{
    glUniform1i(glGetUniformLocation(program, name.c_str()), arg);
}
void Shader::SetMat4(Shader *shaders, unsigned int noShaders, const std::string &name, const glm::mat4 &arg)
{
    for (unsigned int i = 0; i < noShaders; i++)
        shaders[i].SetMat4(name, arg);
}

void Shader::SetVec3(Shader *shaders, unsigned int noShaders, const std::string &name, const glm::vec3 &arg)
{
    for (unsigned int i = 0; i < noShaders; i++)
        shaders[i].SetVec3(name, arg);
}

void Shader::SetFloat(Shader *shaders, unsigned int noShaders, const std::string &name, float arg)
{
    for (unsigned int i = 0; i < noShaders; i++)
        shaders[i].SetFloat(name, arg);
}
