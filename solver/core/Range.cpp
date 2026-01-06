#include "Range.h"
#include "CardAdapter.h"
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cctype>

namespace solver {

Range::Range(std::string_view notation) {
    // Split by commas and parse each component
    size_t start = 0;
    while (start < notation.size()) {
        // Skip whitespace
        while (start < notation.size() && std::isspace(notation[start])) {
            ++start;
        }
        if (start >= notation.size()) break;

        // Find end of component (comma or end)
        size_t end = notation.find(',', start);
        if (end == std::string_view::npos) {
            end = notation.size();
        }

        // Trim trailing whitespace
        size_t comp_end = end;
        while (comp_end > start && std::isspace(notation[comp_end - 1])) {
            --comp_end;
        }

        if (comp_end > start) {
            std::string_view component = notation.substr(start, comp_end - start);

            // Check for weight suffix (e.g., "AA:0.5")
            float weight = 1.0f;
            size_t colon_pos = component.find(':');
            if (colon_pos != std::string_view::npos) {
                std::string weight_str(component.substr(colon_pos + 1));
                weight = std::stof(weight_str);
                component = component.substr(0, colon_pos);
            }

            parse_component(component, weight, hands_);
        }

        start = end + 1;
    }
}

void Range::parse_component(std::string_view component, float weight,
                            std::vector<Hand>& out) {
    if (component.empty()) return;

    // Check for range notation (contains '-')
    size_t dash_pos = component.find('-');
    if (dash_pos != std::string_view::npos && dash_pos > 0) {
        // Range notation: "QQ-TT", "AKs-ATs", etc.
        std::string_view start_hand = component.substr(0, dash_pos);
        std::string_view end_hand = component.substr(dash_pos + 1);

        if (start_hand.size() < 2 || end_hand.size() < 2) {
            throw std::invalid_argument("Invalid range: " + std::string(component));
        }

        uint8_t start_rank1 = CardAdapter::parse_rank(start_hand[0]);
        uint8_t start_rank2 = CardAdapter::parse_rank(start_hand[1]);
        uint8_t end_rank1 = CardAdapter::parse_rank(end_hand[0]);
        uint8_t end_rank2 = CardAdapter::parse_rank(end_hand[1]);

        if (start_rank1 == 255 || start_rank2 == 255 ||
            end_rank1 == 255 || end_rank2 == 255) {
            throw std::invalid_argument("Invalid range ranks: " + std::string(component));
        }

        // Pair range: "QQ-TT"
        if (start_rank1 == start_rank2 && end_rank1 == end_rank2) {
            add_pair_range(start_rank1, end_rank1, weight, out);
            return;
        }

        // Non-pair range: "AKs-ATs" or "AKo-ATo"
        if (start_rank1 == end_rank1) {
            bool suited = (start_hand.size() > 2 && (start_hand[2] == 's' || start_hand[2] == 'S'));
            bool offsuit = (start_hand.size() > 2 && (start_hand[2] == 'o' || start_hand[2] == 'O'));

            if (suited) {
                add_suited_range(start_rank1, start_rank2, end_rank2, weight, out);
            } else if (offsuit) {
                add_offsuit_range(start_rank1, start_rank2, end_rank2, weight, out);
            } else {
                // Both suited and offsuit
                add_suited_range(start_rank1, start_rank2, end_rank2, weight, out);
                add_offsuit_range(start_rank1, start_rank2, end_rank2, weight, out);
            }
            return;
        }

        throw std::invalid_argument("Invalid range format: " + std::string(component));
    }

    // Single hand notation
    if (component.size() < 2) {
        throw std::invalid_argument("Hand notation too short: " + std::string(component));
    }

    uint8_t rank1 = CardAdapter::parse_rank(component[0]);
    uint8_t rank2 = CardAdapter::parse_rank(component[1]);

    if (rank1 == 255 || rank2 == 255) {
        throw std::invalid_argument("Invalid ranks: " + std::string(component));
    }

    // Check for 's' (suited) or 'o' (offsuit) suffix
    bool suited = false;
    bool offsuit = false;
    if (component.size() > 2) {
        char suffix = component[2];
        if (suffix == 's' || suffix == 'S') suited = true;
        else if (suffix == 'o' || suffix == 'O') offsuit = true;
    }

    // Pair: "AA", "KK", etc.
    if (rank1 == rank2) {
        add_pair(rank1, weight, out);
        return;
    }

    // Ensure rank1 > rank2 (higher card first)
    if (rank1 < rank2) {
        std::swap(rank1, rank2);
    }

    // Suited only
    if (suited) {
        add_suited(rank1, rank2, weight, out);
        return;
    }

    // Offsuit only
    if (offsuit) {
        add_offsuit(rank1, rank2, weight, out);
        return;
    }

    // Both suited and offsuit (default when no suffix)
    add_suited(rank1, rank2, weight, out);
    add_offsuit(rank1, rank2, weight, out);
}

void Range::add_pair(uint8_t rank, float weight, std::vector<Hand>& out) {
    // 6 combos for a pair: 4 choose 2
    for (uint8_t s1 = 0; s1 < 4; ++s1) {
        for (uint8_t s2 = s1 + 1; s2 < 4; ++s2) {
            CardIndex c1 = CardIndex::from_rank_suit(rank, s1);
            CardIndex c2 = CardIndex::from_rank_suit(rank, s2);
            out.emplace_back(c1, c2, weight);
        }
    }
}

void Range::add_suited(uint8_t rank1, uint8_t rank2, float weight,
                       std::vector<Hand>& out) {
    // 4 suited combos
    for (uint8_t suit = 0; suit < 4; ++suit) {
        CardIndex c1 = CardIndex::from_rank_suit(rank1, suit);
        CardIndex c2 = CardIndex::from_rank_suit(rank2, suit);
        out.emplace_back(c1, c2, weight);
    }
}

void Range::add_offsuit(uint8_t rank1, uint8_t rank2, float weight,
                        std::vector<Hand>& out) {
    // 12 offsuit combos
    for (uint8_t s1 = 0; s1 < 4; ++s1) {
        for (uint8_t s2 = 0; s2 < 4; ++s2) {
            if (s1 != s2) {
                CardIndex c1 = CardIndex::from_rank_suit(rank1, s1);
                CardIndex c2 = CardIndex::from_rank_suit(rank2, s2);
                out.emplace_back(c1, c2, weight);
            }
        }
    }
}

void Range::add_pair_range(uint8_t high_rank, uint8_t low_rank, float weight,
                           std::vector<Hand>& out) {
    // Ensure high > low
    if (high_rank < low_rank) {
        std::swap(high_rank, low_rank);
    }
    for (uint8_t rank = low_rank; rank <= high_rank; ++rank) {
        add_pair(rank, weight, out);
    }
}

void Range::add_suited_range(uint8_t rank1, uint8_t high_rank2,
                             uint8_t low_rank2, float weight,
                             std::vector<Hand>& out) {
    if (high_rank2 < low_rank2) {
        std::swap(high_rank2, low_rank2);
    }
    for (uint8_t rank2 = low_rank2; rank2 <= high_rank2; ++rank2) {
        if (rank2 != rank1) {  // Can't be suited with same rank
            add_suited(rank1, rank2, weight, out);
        }
    }
}

void Range::add_offsuit_range(uint8_t rank1, uint8_t high_rank2,
                              uint8_t low_rank2, float weight,
                              std::vector<Hand>& out) {
    if (high_rank2 < low_rank2) {
        std::swap(high_rank2, low_rank2);
    }
    for (uint8_t rank2 = low_rank2; rank2 <= high_rank2; ++rank2) {
        if (rank2 != rank1) {  // Can't be offsuit with same rank
            add_offsuit(rank1, rank2, weight, out);
        }
    }
}

void Range::remove_blocked(const Board& board) {
    hands_.erase(
        std::remove_if(hands_.begin(), hands_.end(),
                       [&board](const Hand& h) { return h.overlaps(board); }),
        hands_.end());
}

Range Range::without_blocked(const Board& board) const {
    Range result;
    result.reserve(hands_.size());
    for (const auto& hand : hands_) {
        if (!hand.overlaps(board)) {
            result.add(hand);
        }
    }
    return result;
}

std::vector<float> Range::get_initial_reach_probs(const Board& board) const {
    std::vector<float> probs(hands_.size());
    for (size_t i = 0; i < hands_.size(); ++i) {
        if (hands_[i].overlaps(board)) {
            probs[i] = 0.0f;
        } else {
            probs[i] = hands_[i].weight;
        }
    }
    return probs;
}

std::string Range::to_string() const {
    std::ostringstream oss;
    bool first = true;
    for (const auto& hand : hands_) {
        if (!first) oss << ",";
        first = false;
        oss << hand.to_string();
        if (hand.weight != 1.0f) {
            oss << ":" << hand.weight;
        }
    }
    return oss.str();
}

} // namespace solver
