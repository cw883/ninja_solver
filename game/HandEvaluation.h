#pragma once

#include "Card.h"
#include <vector>
#include <array>
#include <map>

namespace poker {

enum class HandRank : uint8_t {
    HIGH_CARD = 1,
    PAIR = 2,
    TWO_PAIR = 3,
    THREE_OF_A_KIND = 4,
    STRAIGHT = 5,
    FLUSH = 6,
    FULL_HOUSE = 7,
    FOUR_OF_A_KIND = 8,
    STRAIGHT_FLUSH = 9,
    ROYAL_FLUSH = 10
};

struct HandResult {
    HandRank rank;
    std::vector<uint8_t> tiebreakers;  // Values for comparing same-ranked hands

    bool operator<(const HandResult& other) const;
    bool operator>(const HandResult& other) const;
    bool operator==(const HandResult& other) const;
};

// Compare result: 1 = hand1 wins, -1 = hand2 wins, 0 = tie
enum class CompareResult {
    HAND1_WINS = 1,
    TIE = 0,
    HAND2_WINS = -1
};

class HandEvaluator {
public:
    // Evaluate a 5-7 card hand and return the best 5-card ranking
    static HandResult evaluate(const std::vector<Card>& cards);

    // Evaluate with hole cards + community cards
    static HandResult evaluate(const std::vector<Card>& holeCards,
                               const std::vector<Card>& community);

    // Compare two hands given community cards
    static CompareResult compare(const std::vector<Card>& holeCards1,
                                 const std::vector<Card>& holeCards2,
                                 const std::vector<Card>& community);

    // Get string representation of hand rank
    static std::string handRankToString(HandRank rank);

private:
    // Check for specific hands - return tiebreakers if found, empty if not
    static std::vector<uint8_t> checkRoyalFlush(const std::vector<Card>& cards);
    static std::vector<uint8_t> checkStraightFlush(const std::vector<Card>& cards);
    static std::vector<uint8_t> checkFourOfAKind(const std::vector<Card>& cards);
    static std::vector<uint8_t> checkFullHouse(const std::vector<Card>& cards);
    static std::vector<uint8_t> checkFlush(const std::vector<Card>& cards);
    static std::vector<uint8_t> checkStraight(const std::vector<Card>& cards);
    static std::vector<uint8_t> checkThreeOfAKind(const std::vector<Card>& cards);
    static std::vector<uint8_t> checkTwoPair(const std::vector<Card>& cards);
    static std::vector<uint8_t> checkPair(const std::vector<Card>& cards);
    static std::vector<uint8_t> getHighCard(const std::vector<Card>& cards);

    // Helper functions
    static std::map<uint8_t, int> getRankFrequency(const std::vector<Card>& cards);
    static std::map<Suit, std::vector<Card>> getSuitGroups(const std::vector<Card>& cards);
    static std::vector<uint8_t> getKickers(const std::vector<Card>& cards,
                                           const std::vector<uint8_t>& exclude,
                                           size_t count);
    static std::vector<Card> sortByRankDescending(std::vector<Card> cards);
};

} // namespace poker
