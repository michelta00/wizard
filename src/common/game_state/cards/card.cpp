#include "card.h"
#include "trick.h"
#include "../player/hand.h"
#include "../../exceptions/WizardException.h"

// constructors
card::card(std::string id) : unique_serializable(id) { }

card::card(std::string id, serializable_value<std::tuple<int, int>> *val)
        : unique_serializable(id), _value(val)
{ }

card::card(std::tuple<int, int> val) :
        unique_serializable(),
        _value(new serializable_value<std::tuple<int, int>>(val))
{ }

// destructor
card::~card() = default;

// getter function, returns tuple of two ints indicating the number of the card and its color
std::tuple<int, int> card::get_value() const noexcept {
    return _value->get_value();
}

bool card::can_be_played(const trick* const current_trick, const hand* const current_hand) const noexcept {
    // return true if this card can be played according to the game rules:
    // if the card is a jester or wizard, it can be played
    // it the card has the same color as the trick color, it can be played
    // if the card does not have the same color as the trick color, it can only be played if no other card
    // on the players hand has the same color as the trick color

    // get values of this card
    std::tuple<int, int> value = this->get_value();

    // check if card is wizard or jester
    if (std::get<0>(value) > 13) return true;

    // if card is not wizard or jester, compare with trick color
    int trick_color = current_trick->get_trick_color();
    if (std::get<1>(value) == trick_color) return true;

    // TODO: make more efficient if possible
    // if card does not have trick color, check other cards on hand
    const std::vector<card*> cards = current_hand->get_cards();
    for (auto it = cards.begin(); it != cards.end(); ++it)
    {
        std::tuple<int, int> current_value = (*it)->get_value();
        // if there is a card on the hand with the trick color, the current card cannot be played
        if (std::get<1>(current_value) == trick_color) return false;
    }

    return true;
}


card *card::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") && json.HasMember("value")) {
        return new card(json["id"].GetString(), serializable_value<std::tuple<int, int>>::from_json(json["value"].GetObject()));
    } else {
        throw WizardException("Could not parse json of card. Was missing 'id' or 'val'.");
    }
}


void card::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& allocator) const {
    unique_serializable::write_into_json(json, allocator);

    rapidjson::Value value_val(rapidjson::kObjectType);
    _value->write_into_json(value_val, allocator);
    json.AddMember("value", value_val, allocator);
}






