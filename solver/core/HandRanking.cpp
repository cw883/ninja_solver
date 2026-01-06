#include "HandRanking.h"

namespace solver {

int32_t HandRanking::evaluate(CardIndex hole1, CardIndex hole2, const Board& board) {
    // Convert to poker::Card vector
    std::vector<poker::Card> cards;
    cards.reserve(7);

    cards.push_back(CardAdapter::to_card(hole1));
    cards.push_back(CardAdapter::to_card(hole2));

    for (const auto& card : board.cards()) {
        cards.push_back(CardAdapter::to_card(card));
    }

    auto result = poker::HandEvaluator::evaluate(cards);
    return encode(result);
}

int32_t HandRanking::evaluate(CardIndex hole1, CardIndex hole2,
                               std::span<const CardIndex> board_cards) {
    std::vector<poker::Card> cards;
    cards.reserve(2 + board_cards.size());

    cards.push_back(CardAdapter::to_card(hole1));
    cards.push_back(CardAdapter::to_card(hole2));

    for (const auto& card : board_cards) {
        cards.push_back(CardAdapter::to_card(card));
    }

    auto result = poker::HandEvaluator::evaluate(cards);
    return encode(result);
}

int32_t HandRanking::encode(const poker::HandResult& result) {
    // Pack hand rank and tiebreakers into a single int32_t
    // Format: RRRR TTTT TTTT TTTT TTTT TTTT (24 bits used)
    // R = rank (4 bits, values 1-10)
    // T = tiebreakers (5 x 4 bits = 20 bits, values 0-14 each)

    int32_t encoded = static_cast<int32_t>(result.rank) << 20;

    // Add tiebreakers (up to 5, each occupying 4 bits)
    for (size_t i = 0; i < result.tiebreakers.size() && i < 5; ++i) {
        int shift = 16 - (i * 4);  // 16, 12, 8, 4, 0
        encoded |= (static_cast<int32_t>(result.tiebreakers[i]) & 0xF) << shift;
    }

    return encoded;
}

} // namespace solver
