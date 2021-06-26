#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>

#include "_config.h"
#include "random.h"

struct MatSeedPair
{
	ui seed;
	glm::mat4 mat;
};

class Distribution
{
public:
	Distribution(CustomRand &_customRand, ft minX, ft maxX, ft minY, ft maxY, ft closestProxima, ui noElements);
	inline std::vector<glm::vec2> &ToVec2()
	{
		return coords;
	}

	std::vector<glm::mat4> &ToTranslateMatricies();
	std::vector<MatSeedPair> &ToMatSeedPair();

	~Distribution();

protected:
private:
	CustomRand customRand;
	std::vector<glm::vec2> coords;
	std::vector<glm::mat4> matricies;
	std::vector<MatSeedPair> matSeedPairs;
	ft Diff(ft x, ft y);
};
