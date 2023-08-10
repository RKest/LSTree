#include "distribution.h"

#include "glm/geometric.hpp"

Distribution::Distribution(CustomRand& _customRand, ft minX, ft maxX, ft minY, ft maxY,
                           ft closestProxima, ui noElements) :
    customRand(_customRand)
{
    coords.reserve(noElements);
    coords.emplace_back(customRand.NextFloat(minX, maxX), customRand.NextFloat(minY, maxY));

    while (coords.size() < noElements)
    {
        ft x = customRand.NextFloat(minX, maxX);
        ft y = customRand.NextFloat(minY, maxY);
        glm::vec2 position{x, y};
        auto length_to_pos = [position](const glm::vec2& from)
        {
            return glm::length(glm::abs(position - from));
        };

        bool doesElementFit = std::all_of(coords.begin(), coords.end(),
                                          [length_to_pos, closestProxima](glm::vec2& v)
                                          { return length_to_pos(v) > closestProxima; });

        if (doesElementFit)
        {
            coords.push_back(position);
        }
    }
}

std::vector<glm::mat4>& Distribution::ToTranslateMatricies()
{
    matricies.reserve(coords.size());
    for (auto& v : coords)
    {
        matricies.push_back(glm::translate(glm::vec3(v.x, -1, v.y)));
    }

    return matricies;
}

std::vector<MatSeedPair>& Distribution::ToMatSeedPair()
{
    matSeedPairs.reserve(coords.size());
    for (auto& v : coords)
    {
        MatSeedPair pair;
        pair.seed = customRand.NextUi();
        pair.mat = glm::translate(glm::vec3(v.x, -1, v.y));
        matSeedPairs.push_back(pair);
    }

    return matSeedPairs;
}