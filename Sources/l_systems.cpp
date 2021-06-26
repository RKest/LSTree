#include "l_systems.h"
#include <iostream>
#include <time.h>

TreeLS::TreeLS(ui seed, ui _noIterations) : noIterations(_noIterations)
{
    customRand = CustomRand(seed);
    Iterate();
}

void TreeLS::Iterate()
{
    for (ui i = 0; i < noIterations; i++)
    {
        std::string tmp = "";
        ui len = result.length();

        if (i + 1 != noIterations)
        {
            for (ui j = 0; j < len; j++)
            {
                switch (result[j])
                {
                case * "X":
                {
                    tmp += Rule::ChooseReplaceString(rules[X], customRand);
                    break;
                }
                case * "F":
                {
                    tmp += Rule::ChooseReplaceString(rules[F], customRand);
                    break;
                }
                default:
                {
                    tmp += result[j];
                }
                }
            }
        }
        else
        {
            for (ui j = 0; j < len; j++)
            {
                switch (result[j])
                {
                case * "X":
                {
                    tmp += "C";
                    break;
                }
                case * "F":
                {
                    tmp += "FF";
                    break;
                }
                default:
                {
                    tmp += result[j];
                }
                }
            }
        }
        result = tmp;
    }
}

std::string Rule::ChooseReplaceString(Rule &rule, CustomRand &customRand)
{
    ft accumulator = 0.00001f;
    ft r = customRand.NextFloat(0.0, 1.0);

    for (ui i = 0; i < rule.replacers.size(); ++i)
    {
        accumulator += rule.replacers[i].odds;
        if (accumulator >= r)
            return rule.replacers[i].contents;
    }

    throw "Failed to apply a rule Make sure the odds in each replacer amount to 1";
}
