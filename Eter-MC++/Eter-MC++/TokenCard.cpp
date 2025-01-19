//
// Created by mitza on 1/19/25.
//

#include "TokenCard.h"

TokenCard::TokenCard() {
}

TokenCard::TokenCard(CardTexture *card, unsigned int team) : m_cardTexture(card), m_team(team) {
}

TokenCard::~TokenCard() {
}

void TokenCard::SetCard(CardTexture *card) {
    this->m_cardTexture = card;
}

void TokenCard::SetTeam(unsigned int team) {
    this->m_team = team;
}

CardTexture * TokenCard::GetCard() const {
    return this->m_cardTexture;
}

unsigned int TokenCard::GetTeam() const {
    return this->m_team;
}
