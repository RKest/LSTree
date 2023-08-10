#ifndef L_SYSTEMS_H
#define L_SYSTEMS_H

#include <cassert>
#include <random>
#include <string>
#include <vector>

#include "_config.h"
#include "random.h"

struct Replacer
{
    float odds;
    std::string contents;
};

struct Rule
{
    std::string symbol;
    std::vector<Replacer> replacers;
    static std::string ChooseReplaceString(Rule& rule, CustomRand& customRand);
};

class TreeLS
{
public:
    TreeLS(ui seed, ui _noIterations);

    std::string& Result() { return result; };

private:
    /*
    X - Initial state also just a blank symbol
    F - Forward Mening Up
    + - Rotate by an angle
    - - Rotate by a -angle
    [ - Save a state (branch) -- Push
    ] - Resume the other state -- Pop
    C - Draw a Leaf
    G - adjust the gurth exponent -- UNIPLEMENTED
    */
    std::string alphabet = "XF+-[]CG";
    std::string result = "X";

    CustomRand customRand;

    enum
    {
        X,
        F
    };

    ui noIterations;

    void Iterate();

    Rule rules[2] = {
        {
            .symbol = "X",
            .replacers =
                {
                    {
                        .odds = 0.5,
                        .contents = "FF-[[X]+X]+F[FX]+X",
                    },
                    {
                        .odds = 0.5,
                        .contents = "FF+[[X]-X]-F[FX]-X",
                    },
                },
        },
        {
            .symbol = "F",
            .replacers{
                {
                    .odds = 1.0,
                    .contents = "FF",
                },
            },
        },
    };
};

#endif