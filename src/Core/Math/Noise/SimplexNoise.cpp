#include "SimplexNoise.h"

namespace Core::Math
{
	SimplexNoise::SimplexNoise(uint32_t seed)
	{
		noise.SetSeed(static_cast<int>(seed));
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		noise.SetFractalType(FastNoiseLite::FractalType_FBm);
	}

	float SimplexNoise::GetNoise(float x, float z)
	{
		return noise.GetNoise(x, z);
	}
}