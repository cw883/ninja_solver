#include "Deck.h"

namespace poker {

std::array<Card, Deck::SIZE> Deck::getAllCards() {
    std::array<Card, SIZE> cards;
    size_t idx = 0;

    for (int s = 0; s < 4; ++s) {
        for (int r = 2; r <= 14; ++r) {
            cards[idx++] = Card(static_cast<Rank>(r), static_cast<Suit>(s));
        }
    }
    return cards;
}

std::vector<Card> Deck::getAllCardsVector() {
    std::vector<Card> cards;
    cards.reserve(SIZE);

    for (int s = 0; s < 4; ++s) {
        for (int r = 2; r <= 14; ++r) {
            cards.emplace_back(static_cast<Rank>(r), static_cast<Suit>(s));
        }
    }
    return cards;
}

} // namespace poker
