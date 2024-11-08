#ifndef WIZARD_CARD_H
#define WIZARD_CARD_H

#include <tuple>
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "trick.h"
#include "../player/hand.h"

class card : public unique_serializable {
private:

    // TODO: make sure that _value can only take numbers from 1 to 15 in first position and from 0 to 4 in second
    // _value contains the card number as first int (number between 1 and 13, or wizard (14) / jester (15)),
    // and also the color as second int (encoded as 1: yellow, 2: red, 3: green, 4: blue, 0 if wizard or jester)
    serializable_value<std::tuple<int, int>>* _value;

    // from_diff constructor
    card(std::string id);
    // deserialization constructor
    card(std::string id, serializable_value<std::tuple<int, int>>* val);
public:
    card(std::tuple<int, int> val);
    ~card();

    // accessors
    std::tuple<int, int> get_value() const noexcept;

    // card functions
    bool can_be_played(const trick* const current_trick, const hand* const current_hand) const noexcept;

    // serializable interface
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static card* from_json(const rapidjson::Value& json);
};


#endif //WIZARD_CARD_H
