#include "HandEvaluation.h"
#include <algorithm>
#include <stdexcept>

namespace poker {

bool HandResult::operator<(const HandResult& other) const {
    if (rank != other.rank) {
        return rank < other.rank;
    }
    // Compare tiebreakers
    for (size_t i = 0; i < std::min(tiebreakers.size(), other.tiebreakers.size()); ++i) {
        if (tiebreakers[i] != other.tiebreakers[i]) {
            return tiebreakers[i] < other.tiebreakers[i];
        }
    }
    return false;
}

bool HandResult::operator>(const HandResult& other) const {
    return other < *this;
}

bool HandResult::operator==(const HandResult& other) const {
    return !(*this < other) && !(other < *this);
}

HandResult HandEvaluator::evaluate(const std::vector<Card>& cards) {
    if (cards.size() < 5) {
        throw std::invalid_argument("Need at least 5 cards to evaluate");
    }

    HandResult result;
    std::vector<uint8_t> tiebreakers;

    // Check from strongest to weakest
    if (!(tiebreakers = checkRoyalFlush(cards)).empty()) {
        result.rank = HandRank::ROYAL_FLUSH;
        result.tiebreakers = tiebreakers;
    } else if (!(tiebreakers = checkStraightFlush(cards)).empty()) {
        result.rank = HandRank::STRAIGHT_FLUSH;
        result.tiebreakers = tiebreakers;
    } else if (!(tiebreakers = checkFourOfAKind(cards)).empty()) {
        result.rank = HandRank::FOUR_OF_A_KIND;
        result.tiebreakers = tiebreakers;
    } else if (!(tiebreakers = checkFullHouse(cards)).empty()) {
        result.rank = HandRank::FULL_HOUSE;
        result.tiebreakers = tiebreakers;
    } else if (!(tiebreakers = checkFlush(cards)).empty()) {
        result.rank = HandRank::FLUSH;
        result.tiebreakers = tiebreakers;
    } else if (!(tiebreakers = checkStraight(cards)).empty()) {
        result.rank = HandRank::STRAIGHT;
        result.tiebreakers = tiebreakers;
    } else if (!(tiebreakers = checkThreeOfAKind(cards)).empty()) {
        result.rank = HandRank::THREE_OF_A_KIND;
        result.tiebreakers = tiebreakers;
    } else if (!(tiebreakers = checkTwoPair(cards)).empty()) {
        result.rank = HandRank::TWO_PAIR;
        result.tiebreakers = tiebreakers;
    } else if (!(tiebreakers = checkPair(cards)).empty()) {
        result.rank = HandRank::PAIR;
        result.tiebreakers = tiebreakers;
    } else {
        result.rank = HandRank::HIGH_CARD;
        result.tiebreakers = getHighCard(cards);
    }

    return result;
}

HandResult HandEvaluator::evaluate(const std::vector<Card>& holeCards,
                                    const std::vector<Card>& community) {
    std::vector<Card> allCards = holeCards;
    allCards.insert(allCards.end(), community.begin(), community.end());
    return evaluate(allCards);
}

CompareResult HandEvaluator::compare(const std::vector<Card>& holeCards1,
                                      const std::vector<Card>& holeCards2,
                                      const std::vector<Card>& community) {
    HandResult hand1 = evaluate(holeCards1, community);
    HandResult hand2 = evaluate(holeCards2, community);

    if (hand1 > hand2) return CompareResult::HAND1_WINS;
    if (hand1 < hand2) return CompareResult::HAND2_WINS;
    return CompareResult::TIE;
}

std::string HandEvaluator::handRankToString(HandRank rank) {
    switch (rank) {
        case HandRank::HIGH_CARD: return "High Card";
        case HandRank::PAIR: return "Pair";
        case HandRank::TWO_PAIR: return "Two Pair";
        case HandRank::THREE_OF_A_KIND: return "Three of a Kind";
        case HandRank::STRAIGHT: return "Straight";
        case HandRank::FLUSH: return "Flush";
        case HandRank::FULL_HOUSE: return "Full House";
        case HandRank::FOUR_OF_A_KIND: return "Four of a Kind";
        case HandRank::STRAIGHT_FLUSH: return "Straight Flush";
        case HandRank::ROYAL_FLUSH: return "Royal Flush";
        default: return "Unknown";
    }
}

std::map<uint8_t, int> HandEvaluator::getRankFrequency(const std::vector<Card>& cards) {
    std::map<uint8_t, int> freq;
    for (const auto& card : cards) {
        freq[card.getRankValue()]++;
    }
    return freq;
}

std::map<Suit, std::vector<Card>> HandEvaluator::getSuitGroups(const std::vector<Card>& cards) {
    std::map<Suit, std::vector<Card>> groups;
    for (const auto& card : cards) {
        groups[card.getSuit()].push_back(card);
    }
    return groups;
}

std::vector<Card> HandEvaluator::sortByRankDescending(std::vector<Card> cards) {
    std::sort(cards.begin(), cards.end(), [](const Card& a, const Card& b) {
        return a.getRankValue() > b.getRankValue();
    });
    return cards;
}

std::vector<uint8_t> HandEvaluator::getKickers(const std::vector<Card>& cards,
                                                const std::vector<uint8_t>& exclude,
                                                size_t count) {
    std::vector<uint8_t> kickers;
    auto sorted = sortByRankDescending(cards);

    for (const auto& card : sorted) {
        uint8_t rank = card.getRankValue();
        bool excluded = std::find(exclude.begin(), exclude.end(), rank) != exclude.end();
        if (!excluded) {
            kickers.push_back(rank);
            if (kickers.size() >= count) break;
        }
    }
    return kickers;
}

std::vector<uint8_t> HandEvaluator::checkRoyalFlush(const std::vector<Card>& cards) {
    auto suitGroups = getSuitGroups(cards);

    for (const auto& [suit, suitCards] : suitGroups) {
        if (suitCards.size() >= 5) {
            std::vector<bool> hasRank(15, false);
            for (const auto& card : suitCards) {
                hasRank[card.getRankValue()] = true;
            }
            // Check for A, K, Q, J, T of same suit
            if (hasRank[14] && hasRank[13] && hasRank[12] && hasRank[11] && hasRank[10]) {
                return {14}; // Ace high
            }
        }
    }
    return {};
}

std::vector<uint8_t> HandEvaluator::checkStraightFlush(const std::vector<Card>& cards) {
    auto suitGroups = getSuitGroups(cards);

    for (const auto& [suit, suitCards] : suitGroups) {
        if (suitCards.size() >= 5) {
            auto straightResult = checkStraight(suitCards);
            if (!straightResult.empty()) {
                return straightResult;
            }
        }
    }
    return {};
}

std::vector<uint8_t> HandEvaluator::checkFourOfAKind(const std::vector<Card>& cards) {
    auto freq = getRankFrequency(cards);

    for (const auto& [rank, count] : freq) {
        if (count == 4) {
            auto kickers = getKickers(cards, {rank}, 1);
            return {rank, kickers.empty() ? uint8_t(0) : kickers[0]};
        }
    }
    return {};
}

std::vector<uint8_t> HandEvaluator::checkFullHouse(const std::vector<Card>& cards) {
    auto freq = getRankFrequency(cards);

    uint8_t threeRank = 0;
    uint8_t pairRank = 0;

    // Find the highest three of a kind
    for (auto it = freq.rbegin(); it != freq.rend(); ++it) {
        if (it->second >= 3 && threeRank == 0) {
            threeRank = it->first;
        }
    }

    if (threeRank == 0) return {};

    // Find the highest pair (can also be three of a kind)
    for (auto it = freq.rbegin(); it != freq.rend(); ++it) {
        if (it->second >= 2 && it->first != threeRank && pairRank == 0) {
            pairRank = it->first;
        }
    }

    if (pairRank == 0) return {};

    return {threeRank, pairRank};
}

std::vector<uint8_t> HandEvaluator::checkFlush(const std::vector<Card>& cards) {
    auto suitGroups = getSuitGroups(cards);

    for (const auto& [suit, suitCards] : suitGroups) {
        if (suitCards.size() >= 5) {
            auto sorted = sortByRankDescending(suitCards);
            std::vector<uint8_t> result;
            for (size_t i = 0; i < 5 && i < sorted.size(); ++i) {
                result.push_back(sorted[i].getRankValue());
            }
            return result;
        }
    }
    return {};
}

std::vector<uint8_t> HandEvaluator::checkStraight(const std::vector<Card>& cards) {
    std::vector<bool> hasRank(15, false);
    for (const auto& card : cards) {
        hasRank[card.getRankValue()] = true;
    }

    // Check for straights from high to low
    // Ace can be high (14) or low (1)
    for (int high = 14; high >= 5; --high) {
        bool isStraight = true;
        for (int i = 0; i < 5; ++i) {
            int rank = high - i;
            // Handle ace-low straight (A-2-3-4-5)
            if (rank == 1) rank = 14;
            if (!hasRank[rank]) {
                isStraight = false;
                break;
            }
        }
        if (isStraight) {
            return {static_cast<uint8_t>(high)};
        }
    }

    // Check for ace-low straight (wheel): A-2-3-4-5
    if (hasRank[14] && hasRank[2] && hasRank[3] && hasRank[4] && hasRank[5]) {
        return {5}; // 5-high straight
    }

    return {};
}

std::vector<uint8_t> HandEvaluator::checkThreeOfAKind(const std::vector<Card>& cards) {
    auto freq = getRankFrequency(cards);

    for (auto it = freq.rbegin(); it != freq.rend(); ++it) {
        if (it->second == 3) {
            auto kickers = getKickers(cards, {it->first}, 2);
            std::vector<uint8_t> result = {it->first};
            result.insert(result.end(), kickers.begin(), kickers.end());
            return result;
        }
    }
    return {};
}

std::vector<uint8_t> HandEvaluator::checkTwoPair(const std::vector<Card>& cards) {
    auto freq = getRankFrequency(cards);

    std::vector<uint8_t> pairs;
    for (auto it = freq.rbegin(); it != freq.rend(); ++it) {
        if (it->second >= 2) {
            pairs.push_back(it->first);
        }
    }

    if (pairs.size() >= 2) {
        std::sort(pairs.rbegin(), pairs.rend());
        uint8_t highPair = pairs[0];
        uint8_t lowPair = pairs[1];
        auto kickers = getKickers(cards, {highPair, lowPair}, 1);
        return {highPair, lowPair, kickers.empty() ? uint8_t(0) : kickers[0]};
    }
    return {};
}

std::vector<uint8_t> HandEvaluator::checkPair(const std::vector<Card>& cards) {
    auto freq = getRankFrequency(cards);

    for (auto it = freq.rbegin(); it != freq.rend(); ++it) {
        if (it->second == 2) {
            auto kickers = getKickers(cards, {it->first}, 3);
            std::vector<uint8_t> result = {it->first};
            result.insert(result.end(), kickers.begin(), kickers.end());
            return result;
        }
    }
    return {};
}

std::vector<uint8_t> HandEvaluator::getHighCard(const std::vector<Card>& cards) {
    auto sorted = sortByRankDescending(cards);
    std::vector<uint8_t> result;
    for (size_t i = 0; i < 5 && i < sorted.size(); ++i) {
        result.push_back(sorted[i].getRankValue());
    }
    return result;
}

} // namespace poker
