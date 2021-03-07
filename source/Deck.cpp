#include "Deck.h"
#include "Card.h"

using namespace cugl;

/**
 * Initialize an empty deck.
 */
Deck::Deck() {
	_deck = deque<Card>();
}

/**
 * Dispose of a deck by removing its fields.
 */
Deck::~Deck() {
	_deck.~deque();
}

/**
 * Shuffle a deck model, randomizing the order of cards.
 */
void Deck::shuffle() {
	random_shuffle(_deck.begin(), _deck.end());
}

/**
 * Add a card c to the end of the deck.
 * @param c		card to add to deck
 */
void Deck::addCard(Card c) {
	_deck.push_back(c);
}

/**
 * Draw the top card of the deck, removing and returning it.
 */
Card Deck::draw() {
	Card c = _deck.front();
	_deck.pop_front();
	return c;
}