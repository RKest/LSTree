#include "glm/gtx/transform.hpp"
#include <iostream>
#include "glm/gtx/string_cast.hpp"
#include "shader.h"

#include "_config.h"

class ShadowMap
{
public:
	ShadowMap();
	~ShadowMap();

	void Apply(Shader &depthMapShader, Transform &transform, const glm::vec3 &lightPos);
	void CleanUp(ui screenWidth, ui screenHeight);
	inline GLuint Cubemap()
	{
		return depthCubemap;
	}

protected:
private:
	glm::mat4 shadowProjection;

	GLuint depthMapFBO;
	GLuint depthCubemap;
};
