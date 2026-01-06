#pragma once

#include <cstdint>
#include <functional>
#include <compare>

namespace solver {

// Strong type for card index (0-51)
// Encoding: rank * 4 + suit where rank is 0-12 (2-A) and suit is 0-3
struct CardIndex {
    uint8_t value;

    constexpr CardIndex() : value(0) {}
    constexpr explicit CardIndex(uint8_t v) : value(v) {}

    constexpr auto operator<=>(const CardIndex&) const = default;

    // Extract rank (0-12 where 0=2, 12=Ace)
    constexpr uint8_t rank() const { return value / 4; }

    // Extract suit (0-3)
    constexpr uint8_t suit() const { return value % 4; }

    // Create from rank and suit
    static constexpr CardIndex from_rank_suit(uint8_t rank, uint8_t suit) {
        return CardIndex(static_cast<uint8_t>(rank * 4 + suit));
    }
};

// Strong type for player ID (1 or 2)
struct PlayerId {
    uint8_t id;

    constexpr PlayerId() : id(1) {}
    constexpr explicit PlayerId(uint8_t i) : id(i) {}

    constexpr auto operator<=>(const PlayerId&) const = default;

    static constexpr PlayerId OOP() { return PlayerId(1); }  // Out of position
    static constexpr PlayerId IP() { return PlayerId(2); }   // In position

    constexpr PlayerId opponent() const {
        return PlayerId(static_cast<uint8_t>(3 - id));
    }

    constexpr bool is_valid() const { return id == 1 || id == 2; }
};

// Strong type for index into hand range
struct HandIndex {
    uint16_t value;

    constexpr HandIndex() : value(0) {}
    constexpr explicit HandIndex(uint16_t v) : value(v) {}

    constexpr auto operator<=>(const HandIndex&) const = default;

    // Allow implicit conversion to size_t for array indexing
    constexpr operator size_t() const { return value; }
};

// Strong type for action index
struct ActionIndex {
    uint8_t value;

    constexpr ActionIndex() : value(0) {}
    constexpr explicit ActionIndex(uint8_t v) : value(v) {}

    constexpr auto operator<=>(const ActionIndex&) const = default;

    // Allow implicit conversion to size_t for array indexing
    constexpr operator size_t() const { return value; }
};

// Number of cards in a standard deck
inline constexpr int NUM_CARDS = 52;
inline constexpr int NUM_RANKS = 13;
inline constexpr int NUM_SUITS = 4;

// Rank constants (for readability)
namespace Rank {
    inline constexpr uint8_t TWO = 0;
    inline constexpr uint8_t THREE = 1;
    inline constexpr uint8_t FOUR = 2;
    inline constexpr uint8_t FIVE = 3;
    inline constexpr uint8_t SIX = 4;
    inline constexpr uint8_t SEVEN = 5;
    inline constexpr uint8_t EIGHT = 6;
    inline constexpr uint8_t NINE = 7;
    inline constexpr uint8_t TEN = 8;
    inline constexpr uint8_t JACK = 9;
    inline constexpr uint8_t QUEEN = 10;
    inline constexpr uint8_t KING = 11;
    inline constexpr uint8_t ACE = 12;
}

// Suit constants
namespace Suit {
    inline constexpr uint8_t CLUBS = 0;
    inline constexpr uint8_t DIAMONDS = 1;
    inline constexpr uint8_t HEARTS = 2;
    inline constexpr uint8_t SPADES = 3;
}

} // namespace solver

// Hash support for strong types
template<>
struct std::hash<solver::CardIndex> {
    size_t operator()(const solver::CardIndex& c) const noexcept {
        return std::hash<uint8_t>{}(c.value);
    }
};

template<>
struct std::hash<solver::PlayerId> {
    size_t operator()(const solver::PlayerId& p) const noexcept {
        return std::hash<uint8_t>{}(p.id);
    }
};
