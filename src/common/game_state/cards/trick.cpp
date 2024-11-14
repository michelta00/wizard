//
// Created by Aidan Meara on 2024.11.08.
//

#include "trick.h"
#include "../../serialization/vector_utils.h"
#include "../../exceptions/WizardException.h"


trick::trick(std::string id) : unique_serializable(id) { }

trick::trick(std::string id, std::vector<serializable_value<std::pair<card*, player*>>*> &cards):
        unique_serializable(id),
        _cards(cards)
{ }

trick::trick() : unique_serializable() { }

trick::trick(const int trump)
        : unique_serializable(),
        _trump_color(trump) { }


trick::~trick() {
        for (int i = 0; i < _cards.size(); i++) {
                delete _cards[i]->get_value().first;   // delete the `card*`
                delete _cards[i]->get_value().second;  // delete the `player*`
        }
        _cards.clear();
}

int trick::get_trick_color() {
        return this->_trick_color->get_value();
}


//#ifdef WIZARD_SERVER
// determines winner using trump color
player* trick::wrap_up_trick(std::string& err) {
/*
 * TODO game logic for determining winner
 * Needs all the rules for which player wins
 * Needs to know who the first player was so it can return the winning player -> first in cards vector
 * do we need to pass round state to it?
 * OR include header file for round state?
*/
        player* winner = _cards[0]->get_value().second; // base winner is first player, in case of 4 jokers

        // Determine and return winner
        // wizard check
        for (int i = 0; i < _cards.size(); i++) {
                if (_cards[i]->get_value().first.first == 14)
                {
                        return _cards[i]->get_value().second;
                }
        }
        // all joker check
        if (_trick_color == 0)
        {
                return winner; // would be first joker player
        }
        // trump check
        bool trump_present = false;
        int highest_trump = 0;
        for (int i = 0; i < _cards.size(); i++) {
                if (_cards[i]->get_value().first.second == _trump_color)
                {
                        trump_present = true;
                        if (_cards[i]->get_value().first.first > highest_trump)
                        {
                                highest_trump = _cards[i]->get_value().first.first;
                                winner = _cards[i]->get_value().second;
                        }
                }
        }
        if (trump_present) {
                return winner;
        }
        // highest card of trick color check
        int winner_idx = -1; // use a non joker idx;
        for (int i = 0; i < _cards.size(); i++) {
                if (_cards[i]->get_value().first.second == _trick_color)
                        if (winner_idx == -1 || _cards[i]->get_value().first.first > _cards[winner_idx]->get_value().first.first) {
                        winner_idx = i;
                }
        }
        if (winner_idx != -1) {
                winner = _cards[winner_idx]->get_value().second;
        }

        return winner;
}

void trick::set_up_round(std::string& err) {
        // remove all cards (if any) and
        for (int i = 0; i < _cards.size(); i++) {
                delete _cards[i];
        }
        _cards.clear();
}
bool trick::add_card(const std::string& card_id, player* player, std::string& err) {

        return false;
}


bool ::try_play(const std::string& card_id, player* player, std::string& err) {
        card* played_card = nullptr;
        if (player->get_hand()->try_get_card(card_id, played_card)) {
                if (can_play(played_card)) {
                        card* local_system_card;
                        if (player->remove_card(played_card->get_id(), local_system_card, err)) {
                                _cards.push_back(local_system_card);
                                return true;
                        } else {
                                err = "Could not play card " + played_card->get_id() + " because player does not have this card.";
                        }
                } else {
                        err = "The desired card with value " + std::to_string(played_card->get_value())
                              + " cannot be played on top of a card with value " + std::to_string(get_top_card()->get_value());
                }
        } else {
                err = "The player does not possess the card " + card_id + ", which was requested to be played.";
        }
        return false;
}

void set_trick_color(int color) {
        this->_trick_color->set_value(color);
}

// bool discard_pile::try_play(card* played_card, std::string& err) {
//         if (can_play(played_card)) {
//                 _cards.push_back(played_card);
//                 return true;
//         } else {
//                 err = "The desired card with value " + std::to_string(played_card->get_value())
//                         + " cannot be played on top of a card with value " + std::to_string(get_top_card()->get_value());
//         }
//         return false;
// }

//#endif

trick *trick::from_json(const rapidjson::Value &json) {
        if (json.HasMember("id") && json.HasMember("cards")) {
                std::vector<card*> deserialized_cards = std::vector<card*>();
                for (auto &serialized_card : json["cards"].GetArray()) {
                        deserialized_cards.push_back(card::from_json(serialized_card.GetObject()));
                }
                return new trick(json["id"].GetString(), deserialized_cards);
        } else {
                throw WizardException("Could not parse trick from json. 'id' or 'cards' were missing.");
        }
}

void trick::write_into_json(rapidjson::Value &json,
                                   rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
        unique_serializable::write_into_json(json, allocator);
        json.AddMember("cards", vector_utils::serialize_vector(_cards, allocator), allocator);
}