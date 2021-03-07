// Deck.h
//
// This class contains information for the decks (both event and recycle),
// such as the list of cards, as well as functionality for altering and
// shuffling the deck.


#ifndef __DECK_H__
#define __DECK_H__

#include <cugl/cugl.h>
#include "Card.h"

/** 
 *  Model class representing a deck of cards.
 */
class Deck {
private:
	deque<Card> _deck;

public:
	/**
	 * Create a deck, starting empty.
	 */
	Deck();

	/**
	 * Dispose of a deck, freeing resources.
	 */
	~Deck();

	/** 
	 * Shuffle a deck model, randomizing the order of cards.
	 */
	void shuffle();

	/**
	 * Add a card c to the end of the deck.
	 * @param c		card to add to deck
	 */
	void addCard(Card c);

	/**
	 * Draw the top card of the deck, removing and returning it.
	 */
	Card draw();
    
    void printDeck();
    
    int getSize(){
        return _deck.size();
    }
};

#endif
