#include "CardAdapter.h"
#include <stdexcept>

namespace solver {

poker::Card CardAdapter::to_card(CardIndex index) {
    uint8_t rank = index.rank() + 2;  // Convert 0-12 to 2-14
    uint8_t suit = index.suit();
    return poker::Card(static_cast<poker::Rank>(rank), static_cast<poker::Suit>(suit));
}

CardIndex CardAdapter::from_string(std::string_view str) {
    if (str.size() < 2) {
        throw std::invalid_argument("Card string too short");
    }
    uint8_t rank = parse_rank(str[0]);
    uint8_t suit = parse_suit(str[1]);
    if (rank == 255 || suit == 255) {
        throw std::invalid_argument("Invalid card string: " + std::string(str));
    }
    return CardIndex::from_rank_suit(rank, suit);
}

std::string CardAdapter::to_string(CardIndex index) {
    std::string result;
    result += rank_char(index.rank());
    result += suit_char(index.suit());
    return result;
}

} // namespace solver
