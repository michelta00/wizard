// all in this block is only included if wizard_game_state_h hasnt already been included to prevent to include twice
#ifndef WIZARD_GAME_STATE_H
#define WIZARD_GAME_STATE_H

#include <vector>
#include <string>
#include "../../rapidjson/include/rapidjson/document.h"
#include "player/player.h"
#include "../serialization/serializable.h"
#include "../serialization/serializable_value.h"
#include "../serialization/unique_serializable.h"

class game_state : public unique_serializable {
private:

    static const int _max_nof_players = 6;
    static const int _min_nof_players = 3;

    std::vector<player*> _players;
    serializable_value<bool>* _is_started;
    serializable_value<bool>* _is_finished;
    serializable_value<int>* _round_number;
    serializable_value<int>* _trick_number;
    serializable_value<bool>* _is_estimation_phase;
    serializable_value<int>* _starting_player_idx;
    serializable_value<int>* _current_player_idx;

    deck* _deck;
    trick* _trick; // only save current trick, won tricks are saved with the players
    serializable_value<int>* _trump_color;
    serializable_value<int>* _trick_estimate_sum;

    // from_diff constructor, new game is constructed
    game_state(std::string id);

    // deserialization constructor
    game_state(
            std::string id,
            std::vector<player*>& players,
        	round_state* round_state,
            serializable_value<bool>* is_started,
            serializable_value<bool>* is_finished,
            serializable_value<int>* round_number,
            serializable_value<int>* current_player_idx);

    // returns the index of 'player' in the '_players' vector
    int get_player_index(player* player) const;
    int get_number_of_turns();

public:
    game_state();
    ~game_state();

// accessors
    bool is_full() const;
    bool is_started() const;
    bool is_finished() const;
    bool is_player_in_game(player* player) const;
    std::vector<player*>& get_players();
    round_state* get_round_state() const;
    int get_round_number() const;
    int get_max_round_number() const;
    player* get_current_player() const;



// all in block behind ifdef is only included if wizard server is defined
//#ifdef WIZARD_SERVER
// server-side state update functions
    void update_current_player(); //starting player of round
    void setup_round(std::string& err);   // server side initialization
    bool remove_player(player* player, std::string& err);
    bool add_player(player* player, std::string& err);
    bool start_game(std::string& err);
    bool finish_game(std::string& err);


    void determine_trump_color();
    void setup_round(std::string& err);   // server side initialization
    void wrap_up_round(std::string& err);
    bool estimate_tricks(player *player, std::string &err, int trick_estimate);
    bool can_be_played(const card* card, std::string& err) const noexcept;
    bool play_card(player* player, const std::string& card_id, std::string& err);
    void update_current_player(std::string& err);

//#endif

// serializable interface
    static game_state* from_json(const rapidjson::Value& json);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

};


#endif //WIZARD_GAME_STATE_H
