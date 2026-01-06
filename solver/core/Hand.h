#pragma once

#include "Types.h"
#include "CardSet.h"
#include "Board.h"
#include "CardAdapter.h"
#include <compare>
#include <string>

namespace solver {

// Simple hand representation: two hole cards + weight
struct Hand {
    CardIndex card1;
    CardIndex card2;
    float weight = 1.0f;  // Probability weight in range (0.0 to 1.0)

    constexpr Hand() = default;

    constexpr Hand(CardIndex c1, CardIndex c2, float w = 1.0f)
        : card1(c1), card2(c2), weight(w) {
        // Ensure canonical ordering: higher card first
        if (card1.value < card2.value) {
            std::swap(card1, card2);
        }
    }

    // Get the CardSet for this hand (for overlap detection)
    constexpr CardSet to_card_set() const {
        return CardSet::pair(card1, card2);
    }

    // Check if this hand overlaps with a single card
    constexpr bool overlaps(CardIndex card) const {
        return card1 == card || card2 == card;
    }

    // Check if this hand overlaps with a board
    constexpr bool overlaps(const Board& board) const {
        return board.overlaps(card1) || board.overlaps(card2);
    }

    // Check if this hand overlaps with a CardSet
    constexpr bool overlaps(CardSet set) const {
        return set.contains(card1) || set.contains(card2);
    }

    // Check if this hand overlaps with another hand
    constexpr bool overlaps(const Hand& other) const {
        return card1 == other.card1 || card1 == other.card2 ||
               card2 == other.card1 || card2 == other.card2;
    }

    // Check if this is a pocket pair
    constexpr bool is_pair() const {
        return card1.rank() == card2.rank();
    }

    // Check if this is suited
    constexpr bool is_suited() const {
        return card1.suit() == card2.suit();
    }

    // Convert to string like "AhKs"
    std::string to_string() const {
        return CardAdapter::to_string(card1) + CardAdapter::to_string(card2);
    }

    // Parse from string like "AhKs"
    static Hand from_string(std::string_view str) {
        if (str.size() < 4) {
            throw std::invalid_argument("Hand string too short");
        }
        CardIndex c1 = CardAdapter::from_string(str.substr(0, 2));
        CardIndex c2 = CardAdapter::from_string(str.substr(2, 2));
        return Hand(c1, c2);
    }

    // Equality comparison (ignores weight)
    constexpr bool operator==(const Hand& other) const {
        return card1 == other.card1 && card2 == other.card2;
    }

    // Ordering for sorting (by card values, ignores weight)
    constexpr auto operator<=>(const Hand& other) const {
        if (auto cmp = card1 <=> other.card1; cmp != 0) return cmp;
        return card2 <=> other.card2;
    }
};

} // namespace solver
