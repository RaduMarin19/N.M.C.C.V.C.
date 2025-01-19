//
// Created by mitza on 1/19/25.
//

#ifndef TOKENCARD_H
#define TOKENCARD_H
#include "CardTexture.h"


class TokenCard {
public:
    TokenCard();
    TokenCard(CardTexture *card, unsigned int team);
    ~TokenCard();

    void SetCard(CardTexture *card);
    void SetTeam(unsigned int team);
    CardTexture *GetCard() const;
    unsigned int GetTeam() const;
private:
    CardTexture* m_cardTexture;
    unsigned int m_team;
};



#endif //TOKENCARD_H
