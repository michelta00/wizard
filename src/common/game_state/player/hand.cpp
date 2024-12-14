#include "hand.h"
#include <ranges>
#include "../../exceptions/WizardException.h"
#include "../../serialization/vector_utils.h"

// constructor
hand::hand() : unique_serializable() { }

// from_diff constructor
hand::hand(const std::string& id) : unique_serializable(id) { }

// deserialization constructor
hand::hand(const std::string& id, const std::vector<card*>& cards) : unique_serializable(id) {
    this->_cards = cards;
}

// destructor
hand::~hand() {
    _cards.clear();
}

// accessors
unsigned int hand::get_nof_cards() const {
    return _cards.size();
}

std::vector<card*> hand::get_cards() const {
    return _cards;
}

// this function searches for a given card in the hand, and returns whether the card was found or not;
// if the card was found, the variable hand_card (given as reference) is updated
bool hand::try_get_card(const std::string &card_id, card *&hand_card) const {
    const auto it = std::ranges::find_if(_cards,
                                         [&card_id](const card* x) { return x->get_id() == card_id;});
    if (it < _cards.end()) {
        hand_card = *it;
        return true;
    }
    return false;
}

// remove card functions
card* hand::remove_card(const int idx) {
    return remove_card(_cards.begin() + idx);
}

card* hand::remove_card(card* card) {
    const auto pos = std::ranges::find(_cards, card);
    return remove_card(pos);
}

card* hand::remove_card(const std::vector<card*>::iterator pos) {
    if (pos >= _cards.begin() && pos < _cards.end()) {
        card* res = *pos;
        _cards.erase(pos);
        return res;
    }
    return nullptr;
}

bool hand::remove_card(std::string card_id, std::string &err) {
    const auto it = std::ranges::find_if(_cards,
                                         [&card_id](const card* x) { return x->get_id() == card_id;});
    if (it < _cards.end()) {
        remove_card(it);
        return true;
    } else {
        err = "Could not play card, as the requested card was not on the player's hand.";
        return false;
    }
}

#ifdef WIZARD_SERVER
// state update functions
bool hand::add_card(card* card, std::string &err) {
    _cards.push_back(card);
    return true;
}
#endif

// serialization interface
void hand::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& allocator) const {
    unique_serializable::write_into_json(json, allocator);
    json.AddMember("cards", vector_utils::serialize_vector(_cards, allocator), allocator);
}

hand* hand::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") &&
        json.HasMember("cards"))
    {
        std::vector<card*> deserialized_cards = std::vector<card*>();
        for (auto &serialized_card : json["cards"].GetArray()) {
            deserialized_cards.push_back(card::from_json(serialized_card.GetObject()));
        }
        return new hand(json["id"].GetString(), deserialized_cards);
    }
    throw WizardException("Could not parse hand from json. 'cards' were missing.");
}
