#pragma once

#include "Types.h"
#include <cstdint>
#include <bit>

namespace solver {

// Bitset-based card set for O(1) overlap detection
// Uses a 64-bit integer where bits 0-51 represent cards
class CardSet {
public:
    constexpr CardSet() = default;

    // Add a card to the set
    constexpr void add(CardIndex card) {
        bits_ |= (1ULL << card.value);
    }

    // Remove a card from the set
    constexpr void remove(CardIndex card) {
        bits_ &= ~(1ULL << card.value);
    }

    // Check if a card is in the set
    constexpr bool contains(CardIndex card) const {
        return (bits_ & (1ULL << card.value)) != 0;
    }

    // Check if this set overlaps with another set (any cards in common)
    constexpr bool overlaps(CardSet other) const {
        return (bits_ & other.bits_) != 0;
    }

    // Check if this set overlaps with a single card
    constexpr bool overlaps(CardIndex card) const {
        return contains(card);
    }

    // Return the union of two sets
    constexpr CardSet operator|(CardSet other) const {
        CardSet result;
        result.bits_ = bits_ | other.bits_;
        return result;
    }

    // Return the intersection of two sets
    constexpr CardSet operator&(CardSet other) const {
        CardSet result;
        result.bits_ = bits_ & other.bits_;
        return result;
    }

    // Count the number of cards in the set
    constexpr int count() const {
        return std::popcount(bits_);
    }

    // Check if the set is empty
    constexpr bool empty() const {
        return bits_ == 0;
    }

    // Clear all cards from the set
    constexpr void clear() {
        bits_ = 0;
    }

    // Get the underlying bits (for advanced operations)
    constexpr uint64_t bits() const {
        return bits_;
    }

    // Create from raw bits
    static constexpr CardSet from_bits(uint64_t bits) {
        CardSet result;
        result.bits_ = bits;
        return result;
    }

    // Create a set containing a single card
    static constexpr CardSet single(CardIndex card) {
        CardSet result;
        result.add(card);
        return result;
    }

    // Create a set containing two cards (a hand)
    static constexpr CardSet pair(CardIndex card1, CardIndex card2) {
        CardSet result;
        result.add(card1);
        result.add(card2);
        return result;
    }

    constexpr bool operator==(const CardSet&) const = default;

private:
    uint64_t bits_ = 0;
};

} // namespace solver
