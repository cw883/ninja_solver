#pragma once

#include "Types.h"
#include "Board.h"
#include "CardAdapter.h"
#include "../../game/HandEvaluation.h"
#include <cstdint>
#include <span>

namespace solver {

// Wrapper around poker::HandEvaluator that provides a single int32_t ranking
// for fast comparison and sorting. Higher values = better hands.
//
// Encoding: (rank << 20) | (tb1 << 16) | (tb2 << 12) | (tb3 << 8) | (tb4 << 4) | tb5
// where rank is 1-10 (HIGH_CARD to ROYAL_FLUSH) and tb1-tb5 are tiebreaker values
class HandRanking {
public:
    // Evaluate a 7-card hand (2 hole cards + 5 board cards)
    // Returns an integer where higher = better hand
    static int32_t evaluate(CardIndex hole1, CardIndex hole2, const Board& board);

    // Evaluate hole cards against a 5-card board span
    static int32_t evaluate(CardIndex hole1, CardIndex hole2,
                            std::span<const CardIndex> board_cards);

    // Compare two hands: returns positive if hand1 wins, negative if hand2 wins, 0 if tie
    static int compare(int32_t rank1, int32_t rank2) {
        if (rank1 > rank2) return 1;
        if (rank1 < rank2) return -1;
        return 0;
    }

    // Decode the hand rank type from an encoded ranking
    static poker::HandRank decode_hand_type(int32_t ranking) {
        return static_cast<poker::HandRank>((ranking >> 20) & 0xF);
    }

private:
    // Encode a HandResult into a single int32_t
    static int32_t encode(const poker::HandResult& result);
};

} // namespace solver
