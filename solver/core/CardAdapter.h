#pragma once

#include "Types.h"
#include "../../game/Card.h"
#include <string>
#include <string_view>

namespace solver {

// Converts between poker::Card and solver::CardIndex
//
// poker::Card uses Rank 2-14 (Ace=14), Suit 0-3
// solver::CardIndex uses value 0-51: rank * 4 + suit where rank is 0-12 (2=0, Ace=12)
class CardAdapter {
public:
    // Convert poker::Card to CardIndex
    static constexpr CardIndex to_index(const poker::Card& card) {
        uint8_t rank = card.getRankValue() - 2;  // Convert 2-14 to 0-12
        uint8_t suit = static_cast<uint8_t>(card.getSuit());
        return CardIndex::from_rank_suit(rank, suit);
    }

    // Convert CardIndex to poker::Card
    static poker::Card to_card(CardIndex index);

    // Parse string like "As", "Kh", "2c" directly to CardIndex
    static CardIndex from_string(std::string_view str);

    // Convert CardIndex to string like "As", "Kh", "2c"
    static std::string to_string(CardIndex index);

    // Get rank character for display (2-9, T, J, Q, K, A)
    static constexpr char rank_char(uint8_t rank) {
        constexpr char chars[] = "23456789TJQKA";
        return chars[rank];
    }

    // Get suit character for display (c, d, h, s)
    static constexpr char suit_char(uint8_t suit) {
        constexpr char chars[] = "cdhs";
        return chars[suit];
    }

    // Parse rank character to 0-12
    static constexpr uint8_t parse_rank(char c) {
        switch (c) {
            case '2': return 0;
            case '3': return 1;
            case '4': return 2;
            case '5': return 3;
            case '6': return 4;
            case '7': return 5;
            case '8': return 6;
            case '9': return 7;
            case 'T': case 't': return 8;
            case 'J': case 'j': return 9;
            case 'Q': case 'q': return 10;
            case 'K': case 'k': return 11;
            case 'A': case 'a': return 12;
            default: return 255;  // Invalid
        }
    }

    // Parse suit character to 0-3
    static constexpr uint8_t parse_suit(char c) {
        switch (c) {
            case 'c': case 'C': return Suit::CLUBS;
            case 'd': case 'D': return Suit::DIAMONDS;
            case 'h': case 'H': return Suit::HEARTS;
            case 's': case 'S': return Suit::SPADES;
            default: return 255;  // Invalid
        }
    }
};

} // namespace solver
