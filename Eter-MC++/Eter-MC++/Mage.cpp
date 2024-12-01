#include "Mage.h"

Mage::Mage()
{
    std::random_device rd; // Sursa de entropie
    std::mt19937 gen(rd()); // Generator bazat pe Mersenne Twister
    std::uniform_int_distribution<> distr(0, 7); // Distributie uniforma 
    
    int typeIndex = distr(gen);
    m_mageType = static_cast<MageType>(typeIndex);
}

/// doesn't work properly, still have to make some changes
void firePower_1(GameBoard& board, Coordinates pos, Color playerTurn)
{
    auto& playedPos = board.GetPlayedPositions();
    if (playedPos.find(pos) != playedPos.end())
    {
        bool valid = false;
        if (playedPos[pos].back().GetColor() != playerTurn && playedPos[pos].size() > 1)
        {
            for (auto& card : playedPos[pos])
            {
                if (card.GetColor() == playerTurn)
                {
                    valid = true;
                }
            }
        }
        else
        {
            std::cout << "Not a valid position!\n";
            return;
        }
    }
    else
    {
        std::cout << "Position not played yet!\n";
        return;
    }

}

void earthPower_2(GameBoard& board, Coordinates pos)
{
    auto& holes = board.GetHoles();
    holes.emplace(pos);
    std::cout << "Added hole on pos: x=" << pos.GetX() << ", y=" << pos.GetY() << '\n';
}

void Mage::playPower(GameBoard& board)
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
        // TODO: make pos get the chosen position from mouse input
        //Coordinates pos = board.
        //earthPower_2(board, pos);
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
