#ifndef WIZARD_CARD_H
#define WIZARD_CARD_H

#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

class card : public unique_serializable {
private:

    // _value contains the card value as int (number between 1 and 13 as the cards value,
    // or wizard (14) / jester (0))
    // _color contains the card color as an int encoded as 1: yellow, 2: red, 3: green, 4: yellow,
    // and 0 : no color (for wizard or jester)


    serializable_value<int>* _value{};
    serializable_value<int>* _color{};

    // from_diff constructor
    explicit card(const std::string& id);
    // deserialization constructor
    card(const std::string& id, serializable_value<int>* value, serializable_value<int>* color);
public:
    // constructor & destructor
    card(int value, int color);
    ~card();

    // accessors
    [[nodiscard]] int get_value() const noexcept;
    [[nodiscard]] int get_color() const noexcept;

    // serializable interface
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static card* from_json(const rapidjson::Value& json);
};


#endif //WIZARD_CARD_H
