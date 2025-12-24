#include "../game/HandEvaluation.h"
#include "../game/Card.h"
#include <iostream>
#include <cassert>
#include <string>

using namespace poker;

// Helper to create cards from string like "As Kh Qd Jc Ts"
std::vector<Card> makeHand(const std::string &str)
{
    std::vector<Card> cards;
    size_t i = 0;
    while (i < str.length())
    {
        if (str[i] == ' ')
        {
            ++i;
            continue;
        }
        cards.push_back(Card::fromString(str.substr(i, 2)));
        i += 2;
    }
    return cards;
}

void testHighCard()
{
    auto hand = makeHand("As Kd Qh Jc 9s");
    auto result = HandEvaluator::evaluate(hand);
    assert(result.rank == HandRank::HIGH_CARD);
    assert(result.tiebreakers[0] == 14); // Ace high
    std::cout << "✓ High card\n";
}

void testPair()
{
    auto hand = makeHand("As Ah Kd Qc Js");
    auto result = HandEvaluator::evaluate(hand);
    assert(result.rank == HandRank::PAIR);
    assert(result.tiebreakers[0] == 14); // Pair of aces
    std::cout << "✓ Pair\n";
}

void testTwoPair()
{
    auto hand = makeHand("As Ah Kd Kc Js");
    auto result = HandEvaluator::evaluate(hand);
    assert(result.rank == HandRank::TWO_PAIR);
    assert(result.tiebreakers[0] == 14); // Aces
    assert(result.tiebreakers[1] == 13); // Kings
    std::cout << "✓ Two pair\n";
}

void testThreeOfAKind()
{
    auto hand = makeHand("As Ah Ad Kc Js");
    auto result = HandEvaluator::evaluate(hand);
    assert(result.rank == HandRank::THREE_OF_A_KIND);
    assert(result.tiebreakers[0] == 14); // Trip aces
    std::cout << "✓ Three of a kind\n";
}

void testStraight()
{
    auto hand = makeHand("As Kd Qh Jc Ts");
    auto result = HandEvaluator::evaluate(hand);
    assert(result.rank == HandRank::STRAIGHT);
    assert(result.tiebreakers[0] == 14); // Ace-high straight
    std::cout << "✓ Straight (ace high)\n";
}

void testStraightWheel()
{
    auto hand = makeHand("As 2d 3h 4c 5s");
    auto result = HandEvaluator::evaluate(hand);
    assert(result.rank == HandRank::STRAIGHT);
    assert(result.tiebreakers[0] == 5); // 5-high straight (wheel)
    std::cout << "✓ Straight (wheel/ace low)\n";
}

void testFlush()
{
    auto hand = makeHand("As Ks Qs Js 9s");
    auto result = HandEvaluator::evaluate(hand);
    assert(result.rank == HandRank::FLUSH);
    assert(result.tiebreakers[0] == 14); // Ace-high flush
    std::cout << "✓ Flush\n";
}

void testFullHouse()
{
    auto hand = makeHand("As Ah Ad Kc Ks");
    auto result = HandEvaluator::evaluate(hand);
    assert(result.rank == HandRank::FULL_HOUSE);
    assert(result.tiebreakers[0] == 14); // Aces full
    assert(result.tiebreakers[1] == 13); // of Kings
    std::cout << "✓ Full house\n";
}

void testFourOfAKind()
{
    auto hand = makeHand("As Ah Ad Ac Ks");
    auto result = HandEvaluator::evaluate(hand);
    assert(result.rank == HandRank::FOUR_OF_A_KIND);
    assert(result.tiebreakers[0] == 14); // Quad aces
    std::cout << "✓ Four of a kind\n";
}

void testStraightFlush()
{
    auto hand = makeHand("9s Ts Js Qs Ks");
    auto result = HandEvaluator::evaluate(hand);
    assert(result.rank == HandRank::STRAIGHT_FLUSH);
    assert(result.tiebreakers[0] == 13); // King-high straight flush
    std::cout << "✓ Straight flush\n";
}

void testRoyalFlush()
{
    auto hand = makeHand("As Ks Qs Js Ts");
    auto result = HandEvaluator::evaluate(hand);
    assert(result.rank == HandRank::ROYAL_FLUSH);
    std::cout << "✓ Royal flush\n";
}

void testSevenCardHand()
{
    // 7 cards - should find the best 5-card hand
    auto hand = makeHand("As Ah Ad Kc Ks 2d 3h");
    auto result = HandEvaluator::evaluate(hand);
    assert(result.rank == HandRank::FULL_HOUSE);
    std::cout << "✓ Seven card hand (finds best 5)\n";
}

void testCompareHands()
{
    auto hole1 = makeHand("As Ah"); // Will make pair of aces
    auto hole2 = makeHand("Ks Kh"); // Will make pair of kings
    auto community = makeHand("2d 5c 8h Jd Qc");

    auto result = HandEvaluator::compare(hole1, hole2, community);
    assert(result == CompareResult::HAND1_WINS);
    std::cout << "✓ Compare hands (AA vs KK)\n";
}

void testCompareTie()
{
    auto hole1 = makeHand("2s 3s");
    auto hole2 = makeHand("2d 3d");
    auto community = makeHand("As Ah Ad Kc Kh"); // Board plays (AAA KK)

    auto result = HandEvaluator::compare(hole1, hole2, community);
    assert(result == CompareResult::TIE);
    std::cout << "✓ Compare hands (tie - board plays)\n";
}

void testFlushBeatsStaight()
{
    auto hand1 = makeHand("As Ks Qs Js 9s"); // Flush
    auto hand2 = makeHand("As Kd Qh Jc Ts"); // Straight

    auto result1 = HandEvaluator::evaluate(hand1);
    auto result2 = HandEvaluator::evaluate(hand2);
    assert(result1 > result2);
    std::cout << "✓ Flush beats straight\n";
}

void testKickerMatters()
{
    auto hole1 = makeHand("As Kh"); // Pair of aces, K kicker
    auto hole2 = makeHand("Ad Qh"); // Pair of aces, Q kicker
    auto community = makeHand("Ah 2d 5c 8h Jd");

    auto result = HandEvaluator::compare(hole1, hole2, community);
    assert(result == CompareResult::HAND1_WINS);
    std::cout << "✓ Kicker matters\n";
}

int main()
{
    std::cout << "Running HandEvaluator tests...\n\n";

    testHighCard();
    testPair();
    testTwoPair();
    testThreeOfAKind();
    testStraight();
    testStraightWheel();
    testFlush();
    testFullHouse();
    testFourOfAKind();
    testStraightFlush();
    testRoyalFlush();
    testSevenCardHand();
    testCompareHands();
    testCompareTie();
    testFlushBeatsStaight();
    testKickerMatters();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
