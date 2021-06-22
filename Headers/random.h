#ifndef CUSTON_RAND_H
#define CUSTON_RAND_H
#include "_config.h"
#include <cstdint>

class CustomRand
{
public:
	CustomRand(uint32_t seed = 69);

	f NextFloat(f min, f max);
	ui NextU32(ui min, ui max);
	ui NextU32();

protected:
private:
	__uint128_t g_lehmer64_state;
	uint64_t Next();
	uint64_t procGen;
};

#endif
