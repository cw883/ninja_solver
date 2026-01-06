#pragma once

#include "Types.h"
#include "Hand.h"
#include "Board.h"
#include <vector>
#include <span>
#include <string>
#include <string_view>

namespace solver {

// A poker range: a collection of hands with weights
//
// Supports parsing from standard notation:
// - Pairs: "AA", "KK", "QQ"
// - Suited: "AKs", "QJs"
// - Offsuit: "AKo", "QJo"
// - Both suited and offsuit: "AK" (same as "AKs,AKo")
// - Pair ranges: "QQ-TT" (QQ, JJ, TT)
// - Suited ranges: "AKs-ATs" (AKs, AQs, AJs, ATs)
// - Offsuit ranges: "AKo-ATo"
// - Weighted: "AA:0.5" (50% of AA combos)
// - Comma-separated: "AA,KK,QQ,AKs"
class Range {
public:
    Range() = default;

    // Parse from notation string
    explicit Range(std::string_view notation);

    // Number of hands in the range
    size_t size() const { return hands_.size(); }

    // Check if empty
    bool empty() const { return hands_.empty(); }

    // Access hands
    std::span<const Hand> hands() const { return hands_; }
    std::span<Hand> hands() { return hands_; }

    // Access by index
    const Hand& operator[](size_t i) const { return hands_[i]; }
    Hand& operator[](size_t i) { return hands_[i]; }

    // Iterators
    auto begin() { return hands_.begin(); }
    auto end() { return hands_.end(); }
    auto begin() const { return hands_.begin(); }
    auto end() const { return hands_.end(); }

    // Add a hand to the range
    void add(const Hand& hand) { hands_.push_back(hand); }
    void add(CardIndex c1, CardIndex c2, float weight = 1.0f) {
        hands_.emplace_back(c1, c2, weight);
    }

    // Remove hands that overlap with the board
    void remove_blocked(const Board& board);

    // Get a filtered view without blocked hands (returns copy)
    Range without_blocked(const Board& board) const;

    // Compute initial reach probabilities for a board
    // Returns vector of size() with probabilities accounting for card removal
    std::vector<float> get_initial_reach_probs(const Board& board) const;

    // Convert to string notation (for debugging)
    std::string to_string() const;

    // Clear all hands
    void clear() { hands_.clear(); }

    // Reserve space for efficiency
    void reserve(size_t n) { hands_.reserve(n); }

private:
    std::vector<Hand> hands_;

    // Parser helper methods
    static void parse_component(std::string_view component, float weight,
                                std::vector<Hand>& out);
    static void add_pair(uint8_t rank, float weight, std::vector<Hand>& out);
    static void add_suited(uint8_t rank1, uint8_t rank2, float weight,
                           std::vector<Hand>& out);
    static void add_offsuit(uint8_t rank1, uint8_t rank2, float weight,
                            std::vector<Hand>& out);
    static void add_pair_range(uint8_t high_rank, uint8_t low_rank, float weight,
                               std::vector<Hand>& out);
    static void add_suited_range(uint8_t rank1, uint8_t high_rank2,
                                 uint8_t low_rank2, float weight,
                                 std::vector<Hand>& out);
    static void add_offsuit_range(uint8_t rank1, uint8_t high_rank2,
                                  uint8_t low_rank2, float weight,
                                  std::vector<Hand>& out);
};

} // namespace solver
