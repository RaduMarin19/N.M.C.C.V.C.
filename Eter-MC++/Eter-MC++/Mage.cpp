#include "Mage.h"

Mage::Mage()
{
    std::random_device rd; // Sursa de entropie
    std::mt19937 gen(rd()); // Generator bazat pe Mersenne Twister
    std::uniform_int_distribution<> distr(0, 7); // Distributie uniforma 

    int typeIndex = distr(gen);
    m_mageType = static_cast<MageType>(typeIndex);
}

void Mage::playPower()
{
    switch (m_mageType)
    {
    case MageType::fireMaster_1:
        break;
    case MageType::fireMaster_2:
        break;
    case MageType::earthMaster_1:
        break;
    case MageType::earthMaster_2:
        break;
    case MageType::airMaster_1:
        break;
    case MageType::airMaster_2:
        break;
    case MageType::waterMaster_1:
        break;
    case MageType::waterMaster_2:
        break;
    default:
        break;
    }
}
