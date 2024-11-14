#include "card.h"
#include "../../exceptions/WizardException.h"

// TODO: check if the asserts are necessary

// constructors (from_diff and deserialization)
card::card(std::string id) : unique_serializable(id) { }

card::card(std::string id, std::pair<serializable_value<int>*, serializable_value<int>*> val)
        : unique_serializable(id), _value(val)
{ }

// constructor
card::card(std::pair<int, int> val) : unique_serializable()
{
    assert(val.first <= 14 && val.first >= 0);   // check the cards value (or wizard / jester)
    assert(val.second <= 4 && val.second >= 0);  // check the cards color
    _value = std::make_pair(new serializable_value<int>(val.first), new serializable_value<int>(val.second));
}

// destructor
card::~card() = default;

// getter function, returns a pair of two ints indicating the number of the card and its color
// (see header file for more details)
std::pair<int, int> card::get_value() const noexcept {
    return std::make_pair(_value.first->get_value(), _value.second->get_value());
}

// serializable interface
card* card::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") && json.HasMember("number") && json.HasMember("color")) {
        auto number = serializable_value<int>::from_json(json["number"].GetObject());
        auto color = serializable_value<int>::from_json(json["color"].GetObject());
        return new card(json["id"].GetString(), std::make_pair(number, color));
    } else {
        throw WizardException("Could not parse json of card. Was missing 'id', 'number', or 'color'.");
    }
}


void card::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& allocator) const {
    unique_serializable::write_into_json(json, allocator);

    rapidjson::Value number(rapidjson::kObjectType);
    _value.first->write_into_json(number, allocator);
    json.AddMember("number", number, allocator);

    rapidjson::Value color(rapidjson::kObjectType);
    _value.second->write_into_json(color, allocator);
    json.AddMember("color", color, allocator);
}






