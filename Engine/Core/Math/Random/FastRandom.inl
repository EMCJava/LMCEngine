//
// Created by loyus on 7/16/2023.
//

constexpr int
FastRandom::GetIntSeed(const FastRandom::NoiseType &Seed)
{
	return std::get<0>(Seed.SubSeed) ^ std::get<1>(Seed.SubSeed) ^ std::get<2>(Seed.SubSeed) ^ std::get<3>(Seed.SubSeed);
}

constexpr uint64_t
FastRandom::ROTL(const uint64_t x, int k)
{
	return (x << k) | (x >> (64 - k));
}

constexpr FastRandom::FastRandom(const FastRandom::NoiseType &Seed)
    : m_Seed(Seed)
{
}

constexpr FastRandom::FastRandom(const FastRandom &other)
    : m_Seed{.Seed = other.m_Seed.Seed}
{}

constexpr FastRandom &
FastRandom::operator=(const FastRandom &other)
{
	m_Seed.Seed = other.m_Seed.Seed;
	return *this;
}

constexpr const FastRandom::NoiseType &
FastRandom::GetSeed() const
{
	return m_Seed;
}

constexpr std::pair<uint64_t, uint64_t>
FastRandom::CopySeed() const
{
	return m_Seed.Seed;
}

constexpr void
FastRandom::SetSeed(const std::array<uint64_t, 2> &Seed)
{
	m_Seed.Seed = Seed;
}

constexpr void
FastRandom::SetSeed(const std::array<int32_t, 4> &Seed)
{
	m_Seed.SubSeed = Seed;
}

constexpr uint64_t
FastRandom::SeekUint64()
{
	const uint64_t s0 = m_Seed.SeedArray[0];
	uint64_t s1 = m_Seed.SeedArray[1];
	return FastRandom::ROTL(s0 + s1, 17) + s0;
}

constexpr uint64_t
FastRandom::NextUint64()
{
	const uint64_t s0 = m_Seed.SeedArray[0];
	uint64_t s1 = m_Seed.SeedArray[1];
	const uint64_t result = FastRandom::ROTL(s0 + s1, 17) + s0;

	s1 ^= s0;
	m_Seed.SeedArray[0] = FastRandom::ROTL(s0, 49) ^ s1 ^ (s1 << 21);// a, b
	m_Seed.SeedArray[1] = FastRandom::ROTL(s1, 28);                  // c

	return result;
}

constexpr FastRandom &
FastRandom::Jump() noexcept
{
	constexpr std::uint64_t JUMP[] = {0x2bd7a6a6e99c2ddc, 0x0992ccaf6a6fca05};

	std::uint64_t s0 = 0;
	std::uint64_t s1 = 0;

	for (std::uint64_t jump: JUMP)
	{
		for (int b = 0; b < 64; ++b)
		{
			if (jump & UINT64_C(1) << b)
			{
				s0 ^= m_Seed.SeedArray[0];
				s1 ^= m_Seed.SeedArray[1];
			}

			NextUint64();
		}
	}

	m_Seed.SeedArray[0] = s0;
	m_Seed.SeedArray[1] = s1;

	return *this;
}

constexpr FastRandom
FastRandom::FromUint64(uint64_t Seed)
{
	uint64_t z1 = (Seed += 0x9e3779b97f4a7c15);
	z1 = (z1 ^ (z1 >> 30)) * 0xbf58476d1ce4e5b9;
	z1 = (z1 ^ (z1 >> 27)) * 0x94d049bb133111eb;

	uint64_t z2 = (Seed + 0x9e3779b97f4a7c15);
	z2 = (z2 ^ (z2 >> 30)) * 0xbf58476d1ce4e5b9;
	z2 = (z2 ^ (z2 >> 27)) * 0x94d049bb133111eb;
	return NoiseType{.Seed = std::array{z1 ^ (z1 >> 31), z2 ^ (z2 >> 31)}};
}

inline constexpr bool FastRandom::operator==(const FastRandom &oth) const
{
	return m_Seed.SeedArray[0] == oth.m_Seed.SeedArray[0] && m_Seed.SeedArray[1] == oth.m_Seed.SeedArray[1];
}

inline std::ostream &
operator<<(std::ostream &os, const FastRandom &n)
{
	return os << '[' << n.m_Seed.Seed[0] << ", " << n.m_Seed.Seed[1] << ']' << std::flush;
}
