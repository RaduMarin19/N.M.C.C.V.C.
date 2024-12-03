#include "Random.h"

Random& Random::GetInstance()
{
    static Random instance;
    return instance;
}

int Random::Get(int min, int max)
{
    return GetInstance().IGet(min,max);
}

int Random::IGet(int min, int max)
{
    std::uniform_int_distribution<> dist(min, max);
    return dist(*m_engine);
}

Random::Random() {
    std::random_device randomDevice;
    m_engine = std::make_unique<std::mt19937>(randomDevice());
}
