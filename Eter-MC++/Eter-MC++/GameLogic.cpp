#include "GameLogic.h"
#include "Graphics.h"

void HandleBoardState(GameBoard& board, Graphics& painter, GameState& currentState, bool& drawThisFrame) {
    switch (currentState) {
    case TRAINING_MODE:
        board.setTable(3);
        // Specific logic for TRAINING_MODE (if any)
        break;

    case ELEMENTAL_BATTLE:
        board.setTable(4);
        // Specific logic for ELEMENTAL_BATTLE (if any)
        break;

    case MAGE_DUEL:
        board.setTable(4);
        // Specific logic for MAGE_DUEL (if any)
        break;

    case TOURNAMENT:
        board.setTable(4);
        // Specific logic for TOURNAMENT (if any)
        break;

    case QUICK_MODE:
		board.setTable(3);
        // Specific logic for QUICK_MODE (if any)
        break;

    default:
        break;
    }

    // Common logic for all modes
    if (board.canUseExplosion() && board.didExplode() == false) {
        static bool checkExplosion = false;
        board.generateRandomExplosion();

        {
            SDL_Rect explosionRect { SCREEN_WIDTH / 2 - textureWidth, SCREEN_HEIGHT - 200, 128, 128 };
            painter.drawTexturedRect(explosionRect, board.GetExplosionBoardTexture()->getTexture());
            auto explosionMask = board.GetExplosionMask();
            for(int i = 0; i < 3; i++) {
                for(int j = 0; j < 3; j++) {
                    SDL_Rect spriteRect {explosionRect.x + 12 + (i * 34), explosionRect.y + 6 + (j * 32), 32, 32};
                    if(explosionMask[i][j] == ExplosionType::HOLE) {
                        painter.drawTexturedRect(spriteRect, board.GetExplosionSprite(2)->getTexture());
                    }
                    else if(explosionMask[i][j] == ExplosionType::DELETE) {
                        painter.drawTexturedRect(spriteRect, board.GetExplosionSprite(0)->getTexture());
                    }
                    else if(explosionMask[i][j] == ExplosionType::RETURN) {
                        painter.drawTexturedRect(spriteRect, board.GetExplosionSprite(1)->getTexture());
                    }
                }

            }
        }

        if (!checkExplosion) {
            painter.drawButton(checkExplosion, { SCREEN_WIDTH - 1000, SCREEN_HEIGHT - 100 }, 120, 50, "Check explosion!", 14);
        }
        if (checkExplosion) {
            drawThisFrame = false;
            bool exploded = false;
            painter.drawButton(exploded, { SCREEN_WIDTH - 750, SCREEN_HEIGHT - 100 }, 100, 50, "Explode!", 14);
            bool rotate = false;
            painter.drawButton(rotate, { SCREEN_WIDTH - 550, SCREEN_HEIGHT - 100 }, 100, 50, "Rotate!", 14);
            if (exploded) {
                if (board.validateExplosion())
                    board.explode();
                else
                    std::cout << "Explosion invalidates map!\n";
            }
            if (rotate) {
                board.rotateExplosionMask();
                std::cout << "----------------\n";
                board.printExplosionMask();
            }
        }
    }

    if (board.getPlayerBlue()->HasPlayedIllusion() == false && board.isBluePlayer()) {
        painter.drawButton(board.getPlayerBlue()->isPlayingIllusion(), { SCREEN_WIDTH - 320, SCREEN_HEIGHT - 100 }, 120, 50, "Play illusion!", 14);
    }

    if (board.getPlayerRed()->HasPlayedIllusion() == false && !board.isBluePlayer()) {
        painter.drawButton(board.getPlayerRed()->isPlayingIllusion(), { SCREEN_WIDTH - 320, SCREEN_HEIGHT - 100 }, 120, 50, "Play illusion!", 14);

    }
    //Draw the board, with the possible positions and played cards;
    for (const auto& possiblePosition : board.GetPossiblePositions()) {
        SDL_Rect renderRect;
        renderRect.x = board.getCenterX() - (possiblePosition.GetX() * textureWidth);
        renderRect.y = board.getCenterY() - (possiblePosition.GetY() * textureHeight);
        renderRect.w = textureWidth;
        renderRect.h = textureHeight;

        if (painter.isMouseInRect(renderRect)) {
            SDL_SetRenderDrawColor(painter.GetRenderer(), 250, 250, 50, 255);

            if (board.isBluePlayer() && board.getPlayerBlue()->isGrabbingCard() && !painter.isPressingLeftClick()) {
                PlayingCard pushCard = *board.getPlayerBlue()->GetGrabbedCard();
                pushCard.SetBoardPosition(possiblePosition);
                pushCard.SetCoordinates({ renderRect.x, renderRect.y });
                if (board.getPlayerBlue()->HasPlayedIllusion() == false && board.getPlayerBlue()->isPlayingIllusion())
                    pushCard.SetIllussion(true);

                CardStatus status = board.pushNewCard(pushCard);

                if (status == ON_BOARD) {
                    board.getPlayerBlue()->removeCard(*board.getPlayerBlue()->GetGrabbedCard());
                    board.setIsBluePlayer(false);
                    if (pushCard.isIllusion())
                    {
                        for (auto& card : board.GetPlayedPositions())
                            if (card.second.top() == pushCard)
                                card.second.top().SetIllussion(board.getPlayerBlue()->isPlayingIllusion());
                        board.getPlayerBlue()->SetHasPlayedIllusion();
                    }
                    board.checkStatus(currentState);
                }
                else if (status == IN_HAND) {
                    board.returnCardToDeck(*board.getPlayerBlue()->GetGrabbedCard());
                }
                else if (status == REMOVED) {
                    board.getPlayerBlue()->removeCard(*board.getPlayerBlue()->GetGrabbedCard());
                }

            }
            else if (board.getPlayerRed()->isGrabbingCard() && !painter.isPressingLeftClick()) {
                PlayingCard pushCard = *board.getPlayerRed()->GetGrabbedCard();
                pushCard.SetBoardPosition(possiblePosition);
                pushCard.SetCoordinates({ renderRect.x, renderRect.y });
                if (board.getPlayerRed()->HasPlayedIllusion() == false && board.getPlayerRed()->isPlayingIllusion())
                    pushCard.SetIllussion(true);

                CardStatus status = board.pushNewCard(pushCard);

                if (status == ON_BOARD) {
                    board.getPlayerRed()->removeCard(*board.getPlayerRed()->GetGrabbedCard());
                    if (pushCard.isIllusion()) {
                        for (auto& card : board.GetPlayedPositions())
                            if (card.second.top() == pushCard)
                                card.second.top().SetIllussion(board.getPlayerRed()->isPlayingIllusion());
                        board.getPlayerRed()->SetHasPlayedIllusion();
                    }
                    board.setIsBluePlayer(true);
                    board.checkStatus(currentState);
                }
                else if (status == IN_HAND) {
                    board.returnCardToDeck(*board.getPlayerRed()->GetGrabbedCard());
                }
                else if (status == REMOVED) {
                    board.getPlayerRed()->removeCard(*board.getPlayerRed()->GetGrabbedCard());
                }
            }
        }
        else {
            SDL_SetRenderDrawColor(painter.GetRenderer(), 250, 250, 255, 255);
        }
        SDL_RenderDrawRect(painter.GetRenderer(), &renderRect);

    }

    for (const auto& cards = board.GetPlayedCards(); const auto & card : cards) {
        if (card.isIllusion())
        {
            if (card.GetColor() == BLUE)
                painter.drawCard(card, board.getBlueIllusionTexture()->getTexture());
            else if (card.GetColor() == RED)
            {
                painter.drawCard(card, board.getRedIllusionTexture()->getTexture());
            }
        }
        else
        {
            painter.drawCard(card, card.GetTexture()->getTexture());
        }
    }

    //Iterate each players' cards and draw them onto the screen
    //This is where all the in game logic will go
    for (auto& card : board.getPlayerBlue()->GetCards()) {
        SDL_Rect cardRect;
        cardRect.x = card.GetCoordinates().GetX();
        cardRect.y = card.GetCoordinates().GetY();
        cardRect.w = textureWidth;
        cardRect.h = textureHeight;
        if (board.isBluePlayer()) {
            painter.drawCard(card, card.GetTexture()->getTexture());
            if (painter.isMouseInRect(cardRect)) {
                SDL_SetRenderDrawColor(painter.GetRenderer(), 250, 250, 50, 255);
                SDL_RenderDrawRect(painter.GetRenderer(), &cardRect);
                if (painter.isPressingLeftClick() && !board.getPlayerBlue()->isGrabbingCard()) {
                    board.getPlayerBlue()->SetIsGrabbingCard(true);
                    board.getPlayerBlue()->SetGrabbedCard(&card);
                }
                if (board.getPlayerBlue()->isGrabbingCard()) {
                    if (board.getPlayerBlue()->GetGrabbedCard()->GetId() == card.GetId()) {
                        //std::cout << "Player blue is grabbing a card\n";
                        Coordinates mousePos = painter.getMousePos();
                        mousePos.SetX(mousePos.GetX() - (textureWidth / 2));
                        mousePos.SetY(mousePos.GetY() - (textureHeight / 2));
                        card.SetCoordinates(mousePos);
                    }
                }
            }
        }
        else {
            painter.drawCard(card, board.getBlueIllusionTexture()->getTexture());
        }
    }
    for (auto& card : board.getPlayerRed()->GetCards()) {
        SDL_Rect cardRect;
        cardRect.x = card.GetCoordinates().GetX();
        cardRect.y = card.GetCoordinates().GetY();
        cardRect.w = textureWidth;
        cardRect.h = textureHeight;
        if (!board.isBluePlayer()) {
            painter.drawCard(card, card.GetTexture()->getTexture());
            if (painter.isMouseInRect(cardRect)) {
                SDL_SetRenderDrawColor(painter.GetRenderer(), 250, 250, 50, 255);
                SDL_RenderDrawRect(painter.GetRenderer(), &cardRect);

                if (painter.isPressingLeftClick() && !board.getPlayerRed()->isGrabbingCard()) {
                    board.getPlayerRed()->SetIsGrabbingCard(true);
                    board.getPlayerRed()->SetGrabbedCard(&card);
                }
                if (board.getPlayerRed()->isGrabbingCard()) {
                    if (board.getPlayerRed()->GetGrabbedCard()->GetId() == card.GetId()) {
                        //std::cout << "Player red is grabbing a card\n";
                        Coordinates mousePos = painter.getMousePos();
                        mousePos.SetX(mousePos.GetX() - (textureWidth / 2));
                        mousePos.SetY(mousePos.GetY() - (textureHeight / 2));
                        card.SetCoordinates(mousePos);
                    }
                }
            }
        }
        else {
            painter.drawCard(card, board.getRedIllusionTexture()->getTexture());
        }
    }
    if (!painter.isPressingLeftClick() && (board.getPlayerRed()->isGrabbingCard() || board.getPlayerBlue()->isGrabbingCard())) {
        //std::cout << "Player stopped grabbing a card\n";
        board.getPlayerRed()->SetIsGrabbingCard(false);
        board.getPlayerBlue()->SetIsGrabbingCard(false);
    }
}

