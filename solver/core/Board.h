#pragma once

#include "Types.h"
#include "CardSet.h"
#include "CardAdapter.h"
#include <array>
#include <span>
#include <string>
#include <string_view>
#include <stdexcept>
#include <functional>

namespace solver {

// Fixed-size board representation (3-5 cards)
class Board {
public:
    static constexpr int MAX_CARDS = 5;

    constexpr Board() = default;

    // Add a card to the board
    constexpr void add(CardIndex card) {
        if (size_ >= MAX_CARDS) {
            throw std::runtime_error("Board is full");
        }
        cards_[size_++] = card;
        card_set_.add(card);
    }

    // Get the number of cards on the board
    constexpr uint8_t size() const { return size_; }

    // Check if board is a flop (3 cards)
    constexpr bool is_flop() const { return size_ == 3; }

    // Check if board is a turn (4 cards)
    constexpr bool is_turn() const { return size_ == 4; }

    // Check if board is a river (5 cards)
    constexpr bool is_river() const { return size_ == 5; }

    // Access cards by index
    constexpr CardIndex operator[](size_t i) const { return cards_[i]; }

    // Get a span view of the cards
    constexpr std::span<const CardIndex> cards() const {
        return std::span<const CardIndex>(cards_.data(), size_);
    }

    // Get the underlying CardSet for overlap detection
    constexpr const CardSet& card_set() const { return card_set_; }

    // Check if a card overlaps with the board
    constexpr bool overlaps(CardIndex card) const {
        return card_set_.contains(card);
    }

    // Check if a CardSet overlaps with the board
    constexpr bool overlaps(CardSet set) const {
        return card_set_.overlaps(set);
    }

    // Compute hash for caching
    // Uses a simple but collision-resistant scheme
    uint64_t hash() const {
        // Sort-independent hash using XOR of individual card hashes
        // This ensures the same board cards in different order produce the same hash
        uint64_t h = 0;
        for (uint8_t i = 0; i < size_; ++i) {
            // Use a multiplicative hash for each card
            uint64_t card_hash = static_cast<uint64_t>(cards_[i].value) * 0x9E3779B97F4A7C15ULL;
            h ^= card_hash;
        }
        // Mix in the size to differentiate flop/turn/river
        h ^= (static_cast<uint64_t>(size_) << 56);
        return h;
    }

    // Alternative: order-dependent hash (faster, but requires consistent card ordering)
    uint64_t ordered_hash() const {
        uint64_t h = size_;
        for (uint8_t i = 0; i < size_; ++i) {
            h = h * 53 + cards_[i].value;
        }
        return h;
    }

    // Parse from string like "AhKsQd" or "Ah Ks Qd"
    static Board from_string(std::string_view str);

    // Convert to string like "AhKsQd"
    std::string to_string() const;

    // Create a copy with an additional card
    Board with_card(CardIndex card) const {
        Board result = *this;
        result.add(card);
        return result;
    }

    constexpr bool operator==(const Board& other) const {
        if (size_ != other.size_) return false;
        // Compare card sets (order-independent comparison)
        return card_set_ == other.card_set_;
    }

private:
    std::array<CardIndex, MAX_CARDS> cards_{};
    uint8_t size_ = 0;
    CardSet card_set_;
};

} // namespace solver

// Hash support for Board
template<>
struct std::hash<solver::Board> {
    size_t operator()(const solver::Board& b) const noexcept {
        return b.hash();
    }
};
