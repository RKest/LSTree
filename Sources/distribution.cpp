#include "distribution.h"

Distribution::Distribution(CustomRand &_customRand, f minX, f maxX, f minY, f maxY, f closestProxima, ui noElements) : customRand(_customRand)
{
    coords.reserve(noElements);
    coords.push_back(glm::vec2(customRand.NextFloat(minX, maxX), customRand.NextFloat(minY, maxY)));

    while (coords.size() < noElements)
    {
        f x = customRand.NextFloat(minX, maxX);
        f y = customRand.NextFloat(minY, maxY);

        bool doesElementFit = std::all_of(
            coords.begin(),
            coords.end(),
            [this, x, y, closestProxima](glm::vec2 &v)
            {
                return Diff(v.x, x) > closestProxima && Diff(v.y, y) > closestProxima;
            });

        if (doesElementFit)
        {
            coords.push_back(glm::vec2(x, y));
        }
    }
}

Distribution::~Distribution()
{
}

f Distribution::Diff(f x, f y)
{
    return x > y ? x - y : y - x;
}

std::vector<glm::mat4> &Distribution::ToTranslateMatricies()
{
    matricies.reserve(coords.size());
    for (auto &v : coords)
    {
        matricies.push_back(glm::translate(glm::vec3(v.x, -1, v.y)));
    }

    return matricies;
}

std::vector<MatSeedPair> &Distribution::ToMatSeedPair()
{
    matSeedPairs.reserve(coords.size());
    for (auto &v : coords)
    {
        MatSeedPair pair;
        pair.seed = customRand.NextU32();
        pair.mat = glm::translate(glm::vec3(v.x, -1, v.y));
        matSeedPairs.push_back(pair);
    }

    return matSeedPairs;
}