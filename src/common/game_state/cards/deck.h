//
// Created by Michel Tarnow on 15.11.24.
//

#ifndef DECK_H
#define DECK_H

#include "card.h"
#include "../player/hand.h"
#include "../player/player.h"
#include <vector>
#include "../../serialization/serializable.h"
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../game_state/player/player.h"
#include "../../../../rapidjson/include/rapidjson/document.h"


class deck : public unique_serializable
{
private:
    std::vector<card*> _all_cards;
    std::vector<card*> _remaining_cards;
    card* _last_card = nullptr;

    // deserialization constructor
    deck(std::string id, std::vector<card*> &all_cards, std::vector<card*> &remaining_cards, card* last_card);

    // from_diff constructor
    explicit deck(std::string id);

public:
    // constructors & destructor
    deck();     // main constructor which should be used by game state to create its deck
    explicit deck(std::vector<card*>& cards);
    ~deck();

    // accessors
    bool is_empty() const noexcept;
    int get_number_of_remaining_cards() const noexcept;

#ifdef WIZARD_SERVER
    // state update functions
    void setup_round(std::string& err);                                     // sets up the new round essentially by
                                                                            // refreshing _remaining_cards by copying
                                                                            // from _all_cards, and resets _last_card
    bool draw_cards(player* player, int round_number, std::string& err);    // draws a number of random cards based on
                                                                            // the current round number and places them
                                                                            // into the player's hand
    card* draw_trump() const;                                               // randomly draws a card from the remaining
                                                                            // cards as a trump, or if all cards have
                                                                            // been dealt, uses the _last_card as trump
#endif

    // serialization
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static deck* from_json(const rapidjson::Value& json);
};


#endif //DECK_H
