#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GL/glew.h>

class Texture 
{
public:
    Texture(const std::string& fileName);

    void Bind(unsigned int unit);

    virtual ~Texture();

protected:

private:
    GLuint texture;
};

#endif // TEXTURE_H