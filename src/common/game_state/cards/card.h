#ifndef WIZARD_CARD_H
#define WIZARD_CARD_H

#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

class card : public unique_serializable {
private:

    // _value contains the card number as first int (number between 1 and 13 as the cards value,
    // or wizard (14) / jester (0)), and also the color as second int
    // (encoded as 1: yellow, 2: red, 3: green, 4: blue, 0 if wizard or jester)
    std::pair<serializable_value<int>*, serializable_value<int>*> _value;

    // from_diff constructor
    card(std::string id);
    // deserialization constructor
    card(std::string id, std::pair<serializable_value<int>*, serializable_value<int>*> val);
public:
    // constructor & destructor
    card(std::pair<int, int> val);
    ~card();

    // accessors
    std::pair<int, int> get_value() const noexcept;

    // serializable interface
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static card* from_json(const rapidjson::Value& json);
};


#endif //WIZARD_CARD_H
