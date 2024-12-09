#include "card.h"
#include "../../exceptions/WizardException.h"


// constructors (from_diff and deserialization)
card::card(const std::string& id) : unique_serializable(id) { }

card::card(const std::string& id, serializable_value<int>* value, serializable_value<int>* color)
        : unique_serializable(id), _value(value), _color(color)
{ }

// constructor
card::card(const int value, const int color) : unique_serializable()
{
    _value = new serializable_value<int>(value);
    _color = new serializable_value<int>(color);
}

// destructor
card::~card() {
    delete _value;
    delete _color;
}

// getter functions, return the value of the card or its color
// (see header file for more details)
int card::get_value() const noexcept
{
    return _value->get_value();
}
int card::get_color() const noexcept
{
    return _color->get_value();
}


// serializable interface
void card::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& allocator) const {
    unique_serializable::write_into_json(json, allocator);

    rapidjson::Value value(rapidjson::kObjectType);
    _value->write_into_json(value, allocator);
    json.AddMember("value", value, allocator);

    rapidjson::Value color(rapidjson::kObjectType);
    _color->write_into_json(color, allocator);
    json.AddMember("color", color, allocator);
}

card* card::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") &&
        json.HasMember("value") &&
        json.HasMember("color"))
    {
        const auto value = serializable_value<int>::from_json(json["value"].GetObject());
        const auto color = serializable_value<int>::from_json(json["color"].GetObject());
        return new card(json["id"].GetString(), value, color);
    }
    throw WizardException("Could not parse json of card. Was missing 'id', 'value', or 'color'.");
}









