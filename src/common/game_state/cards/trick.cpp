//
// Created by Aidan Meara on 2024.11.08.
//

#include "trick.h"
#include "../../serialization/vector_utils.h"
#include "../../exceptions/WizardException.h"

// constructors and destructor
trick::trick() : unique_serializable()
{
        this->_trick_color = new serializable_value<int>(0);
        this->_trump_color = new serializable_value<int>(0);
}

trick::trick(const std::string& id) : unique_serializable(id)
{
        this->_trick_color = new serializable_value<int>(0);
        this->_trump_color = new serializable_value<int>(0);
}

trick::trick(const std::string& id, const std::vector<std::pair<card*, player*>> &cards,
             serializable_value<int>* trick_color, serializable_value<int>* trump_color)
    : unique_serializable(id), _cards(cards)
{
        this->_trick_color = trick_color;
        this->_trump_color = trump_color;
}

trick::trick(const int trump) : unique_serializable()
{
        this->_trick_color = new serializable_value<int>(0);
        this->_trump_color = new serializable_value<int>(0);
        *_trump_color = trump;
}

trick::~trick()
{
        delete _trick_color;
        delete _trump_color;
        _cards.clear();
}


// accessors
int trick::get_trick_color() const
{
        return this->_trick_color->get_value();
}

int trick::get_trump_color() const
{
        return this->_trump_color->get_value();
}

std::vector<std::pair<card*, player*>> trick::get_cards_and_players() const
{
        return this->_cards;
}


#ifdef WIZARD_SERVER
// state update functions
void trick::set_up_round(int trump, std::string& err)
{
        // remove all cards (if any)
        _cards.clear();
        *_trump_color = trump;
        *_trick_color = 0;
}

player* trick::wrap_up_trick(std::string& err) const
{
        player* winner = _cards[0].second; // base winner is first player, in case of 4 jokers

        // Determine and return winner
        // wizard check
        for (auto & _card : _cards) {
                if (_card.first->get_value() == 14)
                {
                        return _card.second;
                }
        }
        // all joker check
        if (_trick_color->get_value() == 0)
        {
                return winner; // would be first joker player
        }
        // trump check
        bool trump_present = false;
        int highest_trump = 0;
        for (auto & _card : _cards) {
                if (_card.first->get_color() == _trump_color->get_value())
                {
                        trump_present = true;
                        if (_card.first->get_value() > highest_trump)
                        {
                                highest_trump = _card.first->get_value();
                                winner = _card.second;
                        }
                }
        }
        if (trump_present) {
                return winner;
        }
        // highest card of trick color check
        int winner_idx = -1; // use a non joker idx;
        for (int i = 0; i < _cards.size(); i++) {
                //check if played card color matches trick color
                if (_cards[i].first->get_color() == _trick_color->get_value())
                        if (winner_idx == -1 ||
                                _cards[i].first->get_value()
                                > _cards[winner_idx].first->get_value()) {
                        winner_idx = i;
                }
        }
        if (winner_idx != -1) {
                winner = _cards[winner_idx].second;
        }

        return winner;
}

bool trick::add_card(card* played_card, player* current_player, std::string& err) {
        if (played_card) {
                _cards.emplace_back(played_card, current_player);
                // set trick color
                if(_trick_color->get_value() == 0){
                        _trick_color->set_value(played_card->get_color());
                        if(played_card->get_value() == 14) { //if wizard sets trick color to -1
                                _trick_color->set_value(-1);
                        }
                }
                return true;
        }
        err = "The desired card cannot be played";
        return false;
}


// server setter
void trick::set_trick_color(int color) const
{
        *_trick_color = color;
}
#endif

// serialization interface
trick* trick::from_json(const rapidjson::Value &json) {
        if (json.HasMember("id")
                && json.HasMember("cards")
                && json.HasMember("trump_color")
                && json.HasMember("trick_color")) {

                auto deserialized_cards = std::vector<std::pair<card*, player*>>();
                for (auto &serialized_card : json["cards"].GetArray()) {
                        // Deserialize the card
                        card* deserialized_card = card::from_json(serialized_card["card"]);

                        // Deserialize the player
                        player* deserialized_player = player::from_json(serialized_card["player"]);

                        // Add the pair to the vector
                        deserialized_cards.emplace_back(deserialized_card, deserialized_player);
                }
                auto trump_color = serializable_value<int>::from_json(json["trump_color"].GetObject());
                auto trick_color = serializable_value<int>::from_json(json["trick_color"].GetObject());

                return new trick(json["id"].GetString(), deserialized_cards, trick_color, trump_color);
        } else {
                throw WizardException("Could not parse trick from json. 'id' or 'cards' were missing.");
        }
}

void trick::write_into_json(rapidjson::Value &json,
                                   rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
        unique_serializable::write_into_json(json, allocator);

        json.AddMember("cards", vector_utils::serialize_cards_vector(_cards, allocator), allocator);

        rapidjson::Value trump_color(rapidjson::kObjectType);
        _trump_color->write_into_json(trump_color, allocator);
        json.AddMember("trump_color", trump_color, allocator);

        rapidjson::Value trick_color(rapidjson::kObjectType);
        _trick_color->write_into_json(trick_color, allocator);
        json.AddMember("trick_color", trick_color, allocator);
}
