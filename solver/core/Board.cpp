#include "Board.h"

namespace solver {

Board Board::from_string(std::string_view str) {
    Board board;

    size_t i = 0;
    while (i < str.size() && board.size() < MAX_CARDS) {
        // Skip whitespace and commas
        while (i < str.size() && (str[i] == ' ' || str[i] == ',')) {
            ++i;
        }
        if (i >= str.size()) break;

        // Parse card (2 characters)
        if (i + 1 >= str.size()) {
            throw std::invalid_argument("Incomplete card at end of board string");
        }

        uint8_t rank = CardAdapter::parse_rank(str[i]);
        uint8_t suit = CardAdapter::parse_suit(str[i + 1]);

        if (rank == 255 || suit == 255) {
            throw std::invalid_argument("Invalid card in board string: " +
                                        std::string(str.substr(i, 2)));
        }

        board.add(CardIndex::from_rank_suit(rank, suit));
        i += 2;
    }

    if (board.size() < 3) {
        throw std::invalid_argument("Board must have at least 3 cards");
    }

    return board;
}

std::string Board::to_string() const {
    std::string result;
    result.reserve(size_ * 2);
    for (uint8_t i = 0; i < size_; ++i) {
        result += CardAdapter::rank_char(cards_[i].rank());
        result += CardAdapter::suit_char(cards_[i].suit());
    }
    return result;
}

} // namespace solver
