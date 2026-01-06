#include "solver/core/Types.h"
#include "solver/core/CardAdapter.h"
#include "solver/core/CardSet.h"
#include "solver/core/Board.h"
#include "solver/core/HandRanking.h"
#include "solver/core/Hand.h"
#include "solver/core/Range.h"

#include <iostream>
#include <cassert>
#include <cmath>

using namespace solver;

// Simple test framework
int tests_passed = 0;
int tests_failed = 0;

#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
    std::cout << "Running " << #name << "... "; \
    try { \
        test_##name(); \
        std::cout << "PASSED" << std::endl; \
        tests_passed++; \
    } catch (const std::exception& e) { \
        std::cout << "FAILED: " << e.what() << std::endl; \
        tests_failed++; \
    } \
} while(0)

#define ASSERT(cond) do { \
    if (!(cond)) { \
        throw std::runtime_error("Assertion failed: " #cond); \
    } \
} while(0)

#define ASSERT_EQ(a, b) do { \
    if ((a) != (b)) { \
        throw std::runtime_error("Assertion failed: " #a " == " #b); \
    } \
} while(0)

// =============================================================================
// Types Tests
// =============================================================================

TEST(card_index_creation) {
    CardIndex c1(0);
    ASSERT_EQ(c1.value, 0);

    CardIndex c2 = CardIndex::from_rank_suit(Rank::ACE, Suit::SPADES);
    ASSERT_EQ(c2.rank(), Rank::ACE);
    ASSERT_EQ(c2.suit(), Suit::SPADES);
}

TEST(card_index_rank_suit) {
    // Test all cards
    for (uint8_t rank = 0; rank < 13; ++rank) {
        for (uint8_t suit = 0; suit < 4; ++suit) {
            CardIndex c = CardIndex::from_rank_suit(rank, suit);
            ASSERT_EQ(c.rank(), rank);
            ASSERT_EQ(c.suit(), suit);
        }
    }
}

TEST(player_id) {
    PlayerId p1 = PlayerId::OOP();
    PlayerId p2 = PlayerId::IP();

    ASSERT_EQ(p1.id, 1);
    ASSERT_EQ(p2.id, 2);
    ASSERT_EQ(p1.opponent().id, 2);
    ASSERT_EQ(p2.opponent().id, 1);
}

// =============================================================================
// CardAdapter Tests
// =============================================================================

TEST(card_adapter_roundtrip) {
    // Test conversion poker::Card -> CardIndex -> poker::Card
    poker::Card original(poker::Rank::ACE, poker::Suit::SPADES);
    CardIndex idx = CardAdapter::to_index(original);
    poker::Card converted = CardAdapter::to_card(idx);

    ASSERT_EQ(converted.getRank(), original.getRank());
    ASSERT_EQ(converted.getSuit(), original.getSuit());
}

TEST(card_adapter_string_parsing) {
    CardIndex as = CardAdapter::from_string("As");
    ASSERT_EQ(as.rank(), Rank::ACE);
    ASSERT_EQ(as.suit(), Suit::SPADES);

    CardIndex tc = CardAdapter::from_string("Tc");
    ASSERT_EQ(tc.rank(), Rank::TEN);
    ASSERT_EQ(tc.suit(), Suit::CLUBS);

    CardIndex twod = CardAdapter::from_string("2d");
    ASSERT_EQ(twod.rank(), Rank::TWO);
    ASSERT_EQ(twod.suit(), Suit::DIAMONDS);
}

TEST(card_adapter_to_string) {
    CardIndex as = CardIndex::from_rank_suit(Rank::ACE, Suit::SPADES);
    ASSERT_EQ(CardAdapter::to_string(as), "As");

    CardIndex kh = CardIndex::from_rank_suit(Rank::KING, Suit::HEARTS);
    ASSERT_EQ(CardAdapter::to_string(kh), "Kh");
}

// =============================================================================
// CardSet Tests
// =============================================================================

TEST(card_set_basic) {
    CardSet set;
    ASSERT(set.empty());

    CardIndex as = CardAdapter::from_string("As");
    set.add(as);

    ASSERT(!set.empty());
    ASSERT(set.contains(as));
    ASSERT_EQ(set.count(), 1);
}

TEST(card_set_overlap) {
    CardSet set1;
    set1.add(CardAdapter::from_string("As"));
    set1.add(CardAdapter::from_string("Kh"));

    CardSet set2;
    set2.add(CardAdapter::from_string("Kh"));
    set2.add(CardAdapter::from_string("Qd"));

    ASSERT(set1.overlaps(set2));  // Kh in common

    CardSet set3;
    set3.add(CardAdapter::from_string("2c"));
    ASSERT(!set1.overlaps(set3));  // No overlap
}

TEST(card_set_pair) {
    CardIndex c1 = CardAdapter::from_string("As");
    CardIndex c2 = CardAdapter::from_string("Kh");

    CardSet pair = CardSet::pair(c1, c2);
    ASSERT_EQ(pair.count(), 2);
    ASSERT(pair.contains(c1));
    ASSERT(pair.contains(c2));
}

// =============================================================================
// Board Tests
// =============================================================================

TEST(board_creation) {
    Board board;
    board.add(CardAdapter::from_string("Ah"));
    board.add(CardAdapter::from_string("Kd"));
    board.add(CardAdapter::from_string("Qc"));

    ASSERT_EQ(board.size(), 3);
    ASSERT(board.is_flop());
}

TEST(board_from_string) {
    Board board = Board::from_string("AhKdQc");
    ASSERT_EQ(board.size(), 3);
    ASSERT(board.is_flop());
    ASSERT_EQ(board.to_string(), "AhKdQc");
}

TEST(board_overlap) {
    Board board = Board::from_string("AhKdQc");

    CardIndex ah = CardAdapter::from_string("Ah");
    CardIndex as = CardAdapter::from_string("As");

    ASSERT(board.overlaps(ah));
    ASSERT(!board.overlaps(as));
}

TEST(board_with_card) {
    Board flop = Board::from_string("AhKdQc");
    CardIndex turn_card = CardAdapter::from_string("Js");
    Board turn = flop.with_card(turn_card);

    ASSERT_EQ(flop.size(), 3);
    ASSERT_EQ(turn.size(), 4);
    ASSERT(turn.is_turn());
}

TEST(board_hash) {
    // Same cards should produce same hash
    Board b1 = Board::from_string("AhKdQc");
    Board b2 = Board::from_string("AhKdQc");

    ASSERT_EQ(b1.hash(), b2.hash());
}

// =============================================================================
// Hand Tests
// =============================================================================

TEST(hand_creation) {
    CardIndex as = CardAdapter::from_string("As");
    CardIndex kh = CardAdapter::from_string("Kh");

    Hand hand(as, kh);
    ASSERT(hand.card1 == as || hand.card1 == kh);  // Canonical ordering
    ASSERT(hand.card2 == as || hand.card2 == kh);
}

TEST(hand_from_string) {
    Hand hand = Hand::from_string("AsKh");
    ASSERT_EQ(hand.to_string(), "AsKh");
}

TEST(hand_overlap_board) {
    Hand hand = Hand::from_string("AsKh");
    Board board = Board::from_string("AhKdQc");

    ASSERT(!hand.overlaps(board));  // As and Kh not on board

    Board board2 = Board::from_string("AsKdQc");
    ASSERT(hand.overlaps(board2));  // As is on board
}

TEST(hand_is_pair) {
    Hand pair = Hand::from_string("AsAh");
    Hand nonpair = Hand::from_string("AsKh");

    ASSERT(pair.is_pair());
    ASSERT(!nonpair.is_pair());
}

TEST(hand_is_suited) {
    CardIndex as = CardAdapter::from_string("As");
    CardIndex ks = CardAdapter::from_string("Ks");
    CardIndex kh = CardAdapter::from_string("Kh");

    Hand suited(as, ks);
    Hand offsuit(as, kh);

    ASSERT(suited.is_suited());
    ASSERT(!offsuit.is_suited());
}

// =============================================================================
// Range Tests
// =============================================================================

TEST(range_pair) {
    Range range("AA");
    ASSERT_EQ(range.size(), 6);  // 6 combos for AA
}

TEST(range_suited) {
    Range range("AKs");
    ASSERT_EQ(range.size(), 4);  // 4 suited combos

    // All should be suited
    for (const auto& hand : range) {
        ASSERT(hand.is_suited());
    }
}

TEST(range_offsuit) {
    Range range("AKo");
    ASSERT_EQ(range.size(), 12);  // 12 offsuit combos

    // All should be offsuit
    for (const auto& hand : range) {
        ASSERT(!hand.is_suited());
    }
}

TEST(range_both) {
    Range range("AK");
    ASSERT_EQ(range.size(), 16);  // 4 suited + 12 offsuit
}

TEST(range_pair_range) {
    Range range("QQ-TT");
    ASSERT_EQ(range.size(), 18);  // 6 * 3 = 18 combos (QQ, JJ, TT)
}

TEST(range_suited_range) {
    Range range("AKs-ATs");
    ASSERT_EQ(range.size(), 16);  // 4 * 4 = 16 combos (AKs, AQs, AJs, ATs)
}

TEST(range_comma_separated) {
    Range range("AA,KK,QQ");
    ASSERT_EQ(range.size(), 18);  // 6 + 6 + 6
}

TEST(range_weighted) {
    Range range("AA:0.5");
    ASSERT_EQ(range.size(), 6);

    for (const auto& hand : range) {
        ASSERT(std::abs(hand.weight - 0.5f) < 0.001f);
    }
}

TEST(range_remove_blocked) {
    Range range("AA");
    ASSERT_EQ(range.size(), 6);

    Board board = Board::from_string("AhKdQc");
    range.remove_blocked(board);

    // Should remove combos containing Ah
    ASSERT_EQ(range.size(), 3);  // AA with Ah is blocked, 3 remain
}

TEST(range_initial_reach_probs) {
    Range range("AA");
    Board board = Board::from_string("AhKdQc");

    auto probs = range.get_initial_reach_probs(board);
    ASSERT_EQ(probs.size(), 6);

    // Count non-zero probabilities
    int active = 0;
    for (float p : probs) {
        if (p > 0) active++;
    }
    ASSERT_EQ(active, 3);  // 3 combos not blocked
}

// =============================================================================
// HandRanking Tests
// =============================================================================

TEST(hand_ranking_pair_vs_high_card) {
    Board board = Board::from_string("2h3d4c5s7h");

    // AA vs KQ (AA should make a pair)
    CardIndex as = CardAdapter::from_string("As");
    CardIndex ah = CardAdapter::from_string("Ah");
    CardIndex ks = CardAdapter::from_string("Ks");
    CardIndex qh = CardAdapter::from_string("Qh");

    int32_t rank_aa = HandRanking::evaluate(as, ah, board);
    int32_t rank_kq = HandRanking::evaluate(ks, qh, board);

    ASSERT(rank_aa > rank_kq);  // Pair beats high card
}

TEST(hand_ranking_flush_vs_straight) {
    Board board = Board::from_string("2h3h4h5h9c");

    // Ah7h (flush) vs 6s7s (straight)
    CardIndex ah = CardAdapter::from_string("Ah");
    CardIndex h7 = CardAdapter::from_string("7h");
    CardIndex s6 = CardAdapter::from_string("6s");
    CardIndex s7 = CardAdapter::from_string("7s");

    int32_t rank_flush = HandRanking::evaluate(ah, h7, board);
    int32_t rank_straight = HandRanking::evaluate(s6, s7, board);

    ASSERT(rank_flush > rank_straight);  // Flush beats straight
}

// =============================================================================
// Main
// =============================================================================

int main() {
    std::cout << "=== Solver Core Tests ===" << std::endl;

    // Types tests
    RUN_TEST(card_index_creation);
    RUN_TEST(card_index_rank_suit);
    RUN_TEST(player_id);

    // CardAdapter tests
    RUN_TEST(card_adapter_roundtrip);
    RUN_TEST(card_adapter_string_parsing);
    RUN_TEST(card_adapter_to_string);

    // CardSet tests
    RUN_TEST(card_set_basic);
    RUN_TEST(card_set_overlap);
    RUN_TEST(card_set_pair);

    // Board tests
    RUN_TEST(board_creation);
    RUN_TEST(board_from_string);
    RUN_TEST(board_overlap);
    RUN_TEST(board_with_card);
    RUN_TEST(board_hash);

    // Hand tests
    RUN_TEST(hand_creation);
    RUN_TEST(hand_from_string);
    RUN_TEST(hand_overlap_board);
    RUN_TEST(hand_is_pair);
    RUN_TEST(hand_is_suited);

    // Range tests
    RUN_TEST(range_pair);
    RUN_TEST(range_suited);
    RUN_TEST(range_offsuit);
    RUN_TEST(range_both);
    RUN_TEST(range_pair_range);
    RUN_TEST(range_suited_range);
    RUN_TEST(range_comma_separated);
    RUN_TEST(range_weighted);
    RUN_TEST(range_remove_blocked);
    RUN_TEST(range_initial_reach_probs);

    // HandRanking tests
    RUN_TEST(hand_ranking_pair_vs_high_card);
    RUN_TEST(hand_ranking_flush_vs_straight);

    std::cout << "\n=== Results ===" << std::endl;
    std::cout << "Passed: " << tests_passed << std::endl;
    std::cout << "Failed: " << tests_failed << std::endl;

    return tests_failed > 0 ? 1 : 0;
}
