#pragma once

#include <string>
#include <cstdint>

namespace poker {

enum class Suit : uint8_t {
    CLUBS = 0,
    DIAMONDS = 1,
    HEARTS = 2,
    SPADES = 3
};

// Ranks: 2-14 where 14 = Ace
enum class Rank : uint8_t {
    TWO = 2,
    THREE = 3,
    FOUR = 4,
    FIVE = 5,
    SIX = 6,
    SEVEN = 7,
    EIGHT = 8,
    NINE = 9,
    TEN = 10,
    JACK = 11,
    QUEEN = 12,
    KING = 13,
    ACE = 14
};

class Card {
public:
    Card();
    Card(Rank rank, Suit suit);

    Rank getRank() const;
    Suit getSuit() const;

    // Returns value 2-14 for rank comparisons
    uint8_t getRankValue() const;

    // Returns string like "As", "Kh", "2c"
    std::string toString() const;

    bool operator==(const Card& other) const;
    bool operator!=(const Card& other) const;
    bool operator<(const Card& other) const;

    // Parse from string like "As", "Kh", "2c"
    static Card fromString(const std::string& str);
    static Rank parseRank(char c);
    static Suit parseSuit(char c);

private:
    Rank rank_;
    Suit suit_;
};

} // namespace poker
