#include "random.h"
#include <iostream>

CustomRand::CustomRand(uint32_t seed)
{
    g_lehmer64_state = seed;
}

ft CustomRand::NextFloat(ft min, ft max)
{
    return ((ft)Next() / (ft)(0xFFFFFFFFFFFFFFFF)) * (max - min) + min;
}

ui CustomRand::NextU32(ui min, ui max)
{
    return (Next() % (max - min)) + min;
}

ui CustomRand::NextUi()
{
    return (ui)Next();
}
uint64_t CustomRand::Next()
{
    g_lehmer64_state *= 0xda942042e4dd58b5;
    return g_lehmer64_state >> 64;
}
