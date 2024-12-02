// all in this block is only included if wizard_game_state_h hasnt already been included to prevent to include twice
#ifndef WIZARD_GAME_STATE_H
#define WIZARD_GAME_STATE_H

#include <vector>
#include <algorithm>
#include <ranges>
#include <string>
#include "../../rapidjson/include/rapidjson/document.h"
#include "player/player.h"
#include "cards/deck.h"
#include "cards/card.h"
#include "cards/trick.h"
#include "../serialization/serializable.h"
#include "../serialization/serializable_value.h"
#include "../serialization/unique_serializable.h"

class game_state : public unique_serializable {
private:

    static const int _max_nof_players = 6;
    static const int _min_nof_players = 3;

    std::vector<player*> _players;
    deck* _deck;
    trick* _trick; // only save current trick, won tricks are saved with the players

    serializable_value<bool>* _is_started;
    serializable_value<bool>* _is_finished;
    serializable_value<bool>* _is_estimation_phase;
    //TODO: possibly add boolean _determine_trump_color that can be used to tell player to decide on a trump color

    serializable_value<int>* _round_number;
    serializable_value<int>* _trick_number;
    serializable_value<int>* _starting_player_idx;
    serializable_value<int>* _trick_starting_player_idx;
    serializable_value<int>* _current_player_idx;
    serializable_value<int>* _trump_color;
    serializable_value<int>* _trick_estimate_sum;


    // from_diff constructor, new game is constructed
    explicit game_state(std::string id);

    // deserialization constructor
    game_state(
            std::string id,
            std::vector<player*>& players,
            deck* deck,
            trick* trick,

            serializable_value<bool>* is_started,
            serializable_value<bool>* is_finished,
            serializable_value<bool>* is_estimation_phase,

            serializable_value<int>* round_number,
            serializable_value<int>* trick_number,
            serializable_value<int>* starting_player_idx,
            serializable_value<int>* trick_starting_player_idx,
            serializable_value<int>* current_player_idx,
            serializable_value<int>* trump_color,
            serializable_value<int>* trick_estimate_sum
            );

    // returns the index of 'player' in the '_players' vector
    int get_player_index(player* player) const;
    int get_number_of_turns();
    void determine_trump_color();

public:
    game_state();
    ~game_state() override;

// accessors
    bool is_full() const;
    bool is_started() const;
    bool is_finished() const;
    bool is_player_in_game(player* player) const;
    std::vector<player*>& get_players();
    int get_round_number() const;
    int get_trick_number() const;
    int get_trick_estimate_sum() const;
    int get_max_round_number() const;
    int get_trump_color() const;
    player* get_current_player() const;
    trick* get_trick() const;



// all in block behind ifdef is only included if wizard server is defined
// TODO: remove // before ifdef and endif below (if it is still there otherwise ignore this)
#ifdef WIZARD_SERVER
// server-side state update functions
    bool remove_player(player* player, std::string& err);
    bool add_player(player* player, std::string& err);
    bool start_game(std::string& err);
    bool finish_game(std::string& err);

    void setup_round(std::string& err);   // server side initialization
    void wrap_up_round(std::string& err);
    bool estimate_tricks(player *player, std::string &err, int trick_estimate);
    bool can_be_played(player* player, const card* card, std::string& err) const noexcept;
    bool play_card(player* player, const std::string& card_id, std::string& err);
    bool update_current_player(std::string& err);

#endif

// serializable interface
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static game_state* from_json(const rapidjson::Value& json);

};


#endif //WIZARD_GAME_STATE_H
