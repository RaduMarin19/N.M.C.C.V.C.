#pragma once

enum class MageType
{
	fireMaster_1,     // removes opponent card that covers one of my cards
	fireMaster_2,     // removes a row/column of 3 or more cards. The row/column must contain at least one of my cards. Impacts stacks as well
	earthMaster_1,    // covers an opponent card with you card of lower value
	earthMaster_2,    // makes hole on chosen grid space. No Mage can play any cards there, no Mage Powers or Power Explosions may affect that space.
	airMaster_1,      // On his turn, the player may move his card (or a stack of cards with his card on top) to a different, empty space.
	airMaster_2,      // is used as an additional ETER card. It is placed on empty space on the play field
	waterMaster_1,    // On his turn, a Mage may move an opponent’s card (or a stack of cards with an opponent’s card) to a different, empty space.
	waterMaster_2	  // Take any row or column of 3 or more cards at the edge of the playing field and move it to a different edge.This redefines the shape of the playing field.
};