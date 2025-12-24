#pragma once

#include "Card.h"
#include <vector>
#include <array>

namespace poker {

class Deck {
public:
    static constexpr size_t SIZE = 52;

    // Get all 52 cards
    static std::array<Card, SIZE> getAllCards();

    // Get cards as a vector (if needed for flexibility)
    static std::vector<Card> getAllCardsVector();
};

} // namespace poker
