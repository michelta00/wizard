//
// Created by Marie Becker on 08.11.24.
//

#ifndef WIZARD_ROUND_STATE_H
#define WIZARD_ROUND_STATE_H

#include <vector>
#include <string>
#include <functional>
#include <random>
#include "../../rapidjson/include/rapidjson/document.h"
#include "player/player.h"
#include "cards/trick.h"
#include "cards/deck.h"
#include "../serialization/serializable.h"
#include "../serialization/serializable_value.h"
#include "../serialization/unique_serializable.h"

class round_state : public unique_serializable {
// inherits id from unique_serializable

private:

    serializable_value<int>* _starting_player_idx;
    serializable_value<int>* _current_player_idx;  //maybe just use integer instead of pointer?
                                                   // i would leave it like this that's the easiest way for the
                                                   // json methods and also done like this in the other classes
    std::vector<player*>& _players;
    deck* _deck;
    trick* _current_trick; //only save current trick, won tricks are saved with the players
	serializable_value<int>* _trump_color;
    serializable_value<bool>* _is_finished; // maybe not needed
    serializable_value<bool>* _is_estimation_phase;
    serializable_value<int>* _round_number;
    serializable_value<int>* _current_trick_number;
    serializable_value<int>* _trick_estimate_sum;

    // for callback from game_state
    std::function<void()> _on_round_end;

    // from_diff constructor
    round_state(std::string id, const std::vector<player*>& players, serializable_value<int>* starting_player_idx,
                serializable_value<int>* round_number);

    // deserialization constructor
    round_state(
            std::string id,
        	serializable_value<int>* starting_player_idx,
        	serializable_value<int>* current_player_idx,
        	std::vector<player*>& players,
            deck* deck,
        	trick* current_trick,
        	serializable_value<int>* trump_color,
            serializable_value<bool>* is_finished,
        	serializable_value<bool>* is_estimation_phase,
            serializable_value<int>* round_number,
        	serializable_value<int>* current_trick_number,
            serializable_value<int>* trick_estimate_sum);

public:
    round_state(const std::vector<player*>& players, serializable_value<int>* starting_player_idx,
              serializable_value<int>* round_number);
    ~round_state();

// accessors
    player* get_current_player() const;
    player* get_starting_player() const;
    trick* get_trick() const;
    deck* get_deck() const;
    bool is_finished() const;
    bool is_estimation_phase() const;
    std::vector<player*>& get_players();
    int get_round_number() const;
    int get_trump_color() const;
    int get_trick_estimate_sum() const;

// setter
    void set_is_estimation_phase(bool is_estimation_phase);
    void set_finished(bool is_finished);
    void set_round_number(int round_number);
    void set_starting_player_idx(player* starting_player_idx);
    void set_current_player_idx(player* current_player_idx);
    void set_trump_color(int trump_color);
    void set_trick_estimate_sum(int trick_estimate_sum);


#ifdef WIZARD_SERVER
// server-side state update functions
    void set_on_round_end(std::function<void()> callback);
    void finish_round();
    void determine_trump_color()
	void setup_round(std::string& err, const int round_number, const int starting_player_idx);
    void update_current_player(std::string& err);

    int get_number_of_turns();
    bool estimate_tricks(player *player, std::string &err, int trick_estimate);

    bool can_be_played(hand* hand, trick* trick, const std::string& card_id, std::string& err);
    bool play_card(player* player, const std::string& card_id, std::string& err);

    // end of round functions
    //void update_current_player(std::string& err);
    //void wrap_up_round(std::string& err);
#endif

// serializable interface
    static round_state* from_json(const rapidjson::Value& json);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

};


#endif //WIZARD_ROUND_STATE_H
