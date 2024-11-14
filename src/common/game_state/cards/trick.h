//
// Created by Aidan Meara on 2024.11.08.
//

// Holds current trick cards and color of trick, allows interaction

#ifndef WIZARD_TRICK_H
#define WIZARD_TRICK_H

#include <string>
#include <utility>
#include <vector>
#include "card.h"
#include "../player/player.h"
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

class trick : public unique_serializable {
private:
    serializable_value<int>* _trick_color = 0;
    int _trump_color = 0;
    std::vector<serializable_value<std::pair<card*, player*>>*> _cards;
    trick(std::string id);
    trick(std::string id, std::vector<serializable_value<std::pair<card*, player*>>*> &cards);
public:
    trick();
    trick(int trump);
    ~trick();

// accessors
    int get_trick_color();

// #ifdef WIZARD_SERVER
    // state update functions
    player* wrap_up_trick(std::string& err);  // determines winner
    void set_up_round(std::string& err); // resets attributes
    bool add_card(const std::string& card_id, player* player, std::string& err); // adds card
    // setters
    void set_trick_color(int color);
// #endif


// serializable interface
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static trick* from_json(const rapidjson::Value& json);
};

#endif //WIZARD_TRICK_H
