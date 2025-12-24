#include "Card.h"
#include <stdexcept>

namespace poker {

Card::Card() : rank_(Rank::TWO), suit_(Suit::CLUBS) {}

Card::Card(Rank rank, Suit suit) : rank_(rank), suit_(suit) {}

Rank Card::getRank() const {
    return rank_;
}

Suit Card::getSuit() const {
    return suit_;
}

uint8_t Card::getRankValue() const {
    return static_cast<uint8_t>(rank_);
}

std::string Card::toString() const {
    std::string result;

    // Rank character
    switch (rank_) {
        case Rank::TWO:   result += '2'; break;
        case Rank::THREE: result += '3'; break;
        case Rank::FOUR:  result += '4'; break;
        case Rank::FIVE:  result += '5'; break;
        case Rank::SIX:   result += '6'; break;
        case Rank::SEVEN: result += '7'; break;
        case Rank::EIGHT: result += '8'; break;
        case Rank::NINE:  result += '9'; break;
        case Rank::TEN:   result += 'T'; break;
        case Rank::JACK:  result += 'J'; break;
        case Rank::QUEEN: result += 'Q'; break;
        case Rank::KING:  result += 'K'; break;
        case Rank::ACE:   result += 'A'; break;
    }

    // Suit character
    switch (suit_) {
        case Suit::CLUBS:    result += 'c'; break;
        case Suit::DIAMONDS: result += 'd'; break;
        case Suit::HEARTS:   result += 'h'; break;
        case Suit::SPADES:   result += 's'; break;
    }

    return result;
}

bool Card::operator==(const Card& other) const {
    return rank_ == other.rank_ && suit_ == other.suit_;
}

bool Card::operator!=(const Card& other) const {
    return !(*this == other);
}

bool Card::operator<(const Card& other) const {
    if (rank_ != other.rank_) {
        return rank_ < other.rank_;
    }
    return suit_ < other.suit_;
}

Card Card::fromString(const std::string& str) {
    if (str.length() != 2) {
        throw std::invalid_argument("Card string must be 2 characters");
    }
    return Card(parseRank(str[0]), parseSuit(str[1]));
}

Rank Card::parseRank(char c) {
    switch (c) {
        case '2': return Rank::TWO;
        case '3': return Rank::THREE;
        case '4': return Rank::FOUR;
        case '5': return Rank::FIVE;
        case '6': return Rank::SIX;
        case '7': return Rank::SEVEN;
        case '8': return Rank::EIGHT;
        case '9': return Rank::NINE;
        case 'T': case 't': return Rank::TEN;
        case 'J': case 'j': return Rank::JACK;
        case 'Q': case 'q': return Rank::QUEEN;
        case 'K': case 'k': return Rank::KING;
        case 'A': case 'a': return Rank::ACE;
        default:
            throw std::invalid_argument(std::string("Invalid rank character: ") + c);
    }
}

Suit Card::parseSuit(char c) {
    switch (c) {
        case 'c': case 'C': return Suit::CLUBS;
        case 'd': case 'D': return Suit::DIAMONDS;
        case 'h': case 'H': return Suit::HEARTS;
        case 's': case 'S': return Suit::SPADES;
        default:
            throw std::invalid_argument(std::string("Invalid suit character: ") + c);
    }
}

} // namespace poker
