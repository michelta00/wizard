#include "game_state.h"

#include <vector>

#include "../exceptions/WizardException.h"
#include "../serialization/vector_utils.h"

// from_diff constructor
game_state::game_state(std::string id) : unique_serializable(id) {
    _players = std::vector<player*>();
    _deck = new deck();
    _trick = new trick();

    _is_started = new serializable_value<bool>(false);
    _is_finished = new serializable_value<bool>(false);
    _is_estimation_phase = new serializable_value<bool>(true);

    _round_number = new serializable_value<int>(0);
    _trick_number = new serializable_value<int>(0);
    _starting_player_idx = new serializable_value<int>(0);
    _trick_starting_player_idx = new serializable_value<int>(0);
    _current_player_idx = new serializable_value<int>(0);
    _trump_color = nullptr;
    _trick_estimate_sum = new serializable_value<int>(0);
}

// deserialization constructor
game_state::game_state(std::string id, std::vector<player*>& players, deck* deck, trick* trick,
                       serializable_value<bool>* is_started, serializable_value<bool>* is_finished,
                       serializable_value<bool>* is_estimation_phase, serializable_value<int>* round_number,
                       serializable_value<int>* trick_number, serializable_value<int>* starting_player_idx,
                       serializable_value<int>* trick_starting_player_idx, serializable_value<int>* current_player_idx,
                       serializable_value<int>* trump_color, serializable_value<int>* trick_estimate_sum)
        : unique_serializable(id),
          _players(players),
          _deck(deck),
          _trick(trick),

          _is_started(is_started),
          _is_finished(is_finished),
          _is_estimation_phase(is_estimation_phase),

          _round_number(round_number),
          _trick_number(trick_number),
          _starting_player_idx(starting_player_idx),
          _trick_starting_player_idx(trick_starting_player_idx),
          _current_player_idx(current_player_idx),
          _trump_color(trump_color),
          _trick_estimate_sum(trick_estimate_sum)
{ }

// public constructor
game_state::game_state() : unique_serializable() {
    _players = std::vector<player*>();
    _deck = new deck();
    _trick = new trick();

    _is_started = new serializable_value<bool>(false);
    _is_finished = new serializable_value<bool>(false);
    _is_estimation_phase = new serializable_value<bool>(true);

    _round_number = new serializable_value<int>(0);
    _trick_number = new serializable_value<int>(0);
    _starting_player_idx = new serializable_value<int>(0);
    _trick_starting_player_idx = new serializable_value<int>(0);
    _current_player_idx = new serializable_value<int>(0);
    _trump_color = nullptr;
    _trick_estimate_sum = new serializable_value<int>(0);
}

// destructor
game_state::~game_state() {
    for (auto & player : _players) {
        delete player;
        player = nullptr;
    }
    delete _deck;
    delete _trick;

    delete _is_started;
    delete _is_finished;
    delete _is_estimation_phase;

    delete _round_number;
    delete _trick_number;
    delete _starting_player_idx;
    delete _trick_starting_player_idx;
    delete _current_player_idx;
    delete _trump_color;
    delete _trick_estimate_sum;
}

// accessors
player* game_state::get_current_player() const {
    if(_current_player_idx == nullptr || _players.empty()) {
        return nullptr;
    }
    return _players[_current_player_idx->get_value()];
}

bool game_state::is_full() const {
    return _players.size() == _max_nof_players;
}

bool game_state::is_started() const {
    return _is_started->get_value();
}

bool game_state::is_finished() const {
    return _is_finished->get_value();
}

int game_state::get_round_number() const {
    return _round_number->get_value();
}

int game_state::get_trick_number() const
{
    return _trick_number->get_value();
}

int game_state::get_max_round_number() const {
    return 60 / _players.size();
}

int game_state::get_player_index(player *player) const {
    auto it = std::ranges::find(_players, player);
    if (it == _players.end()) {
        return -1;
    } else {
        return it - _players.begin();
    }
}

bool game_state::is_player_in_game(player *player) const {
    return std::ranges::find(_players, player) < _players.end();
}

std::vector<player*>& game_state::get_players() {
    return _players;
}


//TODO: remove // before ifdef and endif below (if it is still there otherwise ignore this)
#ifdef WIZARD_SERVER

// state modification functions without diff

    void game_state::wrap_up_round(std::string& err){
        _starting_player_idx->set_value((_starting_player_idx->get_value() + 1) % _players.size());

        for (auto & player : _players) {
          player->wrap_up_round();
        }
        if (_round_number->get_value() == get_max_round_number() - 1) {
          finish_game(err);
        }
    }

    bool game_state::update_current_player(std::string& err){
        _current_player_idx->set_value((_current_player_idx->get_value() + 1) % _players.size());

        //if current player is last player of round, switch from estimation to playing or end round and send callback to game_state
        if (_current_player_idx->get_value() == _trick_starting_player_idx->get_value()) {
            if (_is_estimation_phase->get_value() == true) {
                _is_estimation_phase->set_value(false);
            } else {
                //determine trick winner
                player* winner = _trick->wrap_up_trick(err);
                winner->set_nof_tricks(winner->get_nof_tricks() + 1);

                // round has not ended yet
                if (_trick_number->get_value() < _round_number->get_value()){
                    _trick_number->set_value(_trick_number->get_value() + 1);
                    // TODO: add arguments to function call below
      	            _trick->set_up_round();

                    // winner of trick is starting player of next trick
                    int winner_index = get_player_index(winner);
                    if(winner_index == -1){
                      err = "Player is not part of the game!";
                      return false;
                    } else {
                      _trick_starting_player_idx->set_value(winner_index);
                      _current_player_idx->set_value(winner_index);
                    }
                } else {
                  wrap_up_round(err);
                  _round_number->set_value(_round_number->get_value() + 1);
                  setup_round(err);
                }
            }
        }
        return true;
    }

    void game_state::determine_trump_color() {
        if(_round_number->get_value() * _players.size() == 60) {
            _trump_color->set_value(0); // there is no trump since it is the last round
        } else {
            card* trump_card = _deck->draw_trump();
            if (trump_card->get_color() != 0){
              _trump_color->set_value(trump_card->get_color());
            }
            else if (trump_card->get_value() == 0) {	//jester
              _trump_color->set_value(0);
            }
            else if (trump_card->get_value() == 14){	//wizard
	          // for now: just randomly generates number
              std::random_device rd;
              std::mt19937 gen(rd());
              std::uniform_int_distribution<> distrib(1, 4);
              _trump_color->set_value(distrib(gen));
              // TODO: include get trump_color_request()
            }
        }
    }

    void game_state::setup_round(std::string &err) {

        _trick_estimate_sum->set_value(0);
        _is_estimation_phase->set_value(true);
        _trick_number->set_value(0); //tricks numbers start by 0
        _trick_starting_player_idx->set_value(_starting_player_idx->get_value());
        _current_player_idx->set_value(_starting_player_idx->get_value());

        _deck->setup_round();
        for (auto & player : _players) {
            player->setup_round();
            _deck->draw_cards(player, _round_number->get_value(), err);
        }
        determine_trump_color();
        _trick->set_up_round(err, _trump_color->get_value());
    }

    bool game_state::start_game(std::string &err) {
        if (_players.size() < _min_nof_players) {
            err = "You need at least " + std::to_string(_min_nof_players) + " players to start the game.";
            return false;
        }
        if (!_is_started->get_value()) {
            _is_started->set_value(true);
            setup_round(err);
            return true;
        } else {
            err = "Could not start game, as the game was already started";
            return false;
        }
    }

    int game_state::get_number_of_turns(){
        int nof_players = this->_players.size();

        if (this->_current_player_idx->get_value() >= this->_starting_player_idx->get_value()) {
        return (this->_current_player_idx->get_value() - this->_starting_player_idx->get_value() + 1); //in first round 0
        }
        else{
        return (this->_current_player_idx->get_value() - this->_starting_player_idx->get_value() + nof_players) % nof_players + 1;
        }
    }

    bool game_state::estimate_tricks(player *player, std::string &err, int trick_estimate){

        if (trick_estimate > get_round_number()) {
            err = "Trick estimate is too big. You can't win more tricks than cards in your hand.";
            return false;
        }
        if (trick_estimate < 0) {
            err = "Trick estimate is too small. You can't win less than 0 tricks.";
            return false;
        }
        if(get_number_of_turns() == this->_players.size() &&
           trick_estimate + _trick_estimate_sum->get_value() == _round_number->get_value()) {
            err = "The tricks can't add up to the exact number of cards in the round. Please either choose a higher or lower number of tricks.";
            return false;
        }

        player->set_nof_tricks(trick_estimate);
        _trick_estimate_sum->set_value(trick_estimate + _trick_estimate_sum->get_value());
        return update_current_player(err); //handles logic to switch from estimation to playing round etc.
    }

    // return true if this card can be played according to the game rules (checked in that order):
    // if there is no trick color yet (trick color is 0) or a wizard has set the trick color (-1), then the card can be played
    // if the card is a jester or wizard, then it can be played
    // it the card has the same color as the trick color, it can be played
    // if the card does not have the same color as the trick color, it can only be played if no other card
    // on the players hand has the same color as the trick color
    bool game_state::can_be_played(player* player, const card* card, std::string& err) const noexcept {

        // get trick color -> if 0, card can be played
        int trick_color = _trick->get_trick_color();
        int card_value = card->get_value();
        int card_color = card->get_color();

        // everything can be played if no trick color yet or a wizard has set the trick color
        if (trick_color == 0 || trick_color == -1) {
          return true;
        }

        // check if card is wizard or jester -> if yes, card can be played
        if (card_value == 0 || card_value == 14) {
            return true;
        }

        // compare card color with trick color -> if same, can be played
        if (card_color == trick_color) {
            return true;
        }

        // if card does not have trick color, check other cards on hand
        auto cards = player->get_hand()->get_cards();
        for (const auto & c : cards) {
          if (c->get_color() == trick_color && c->get_value() != card_value) {
              err = "You can't play this card because you have another card which fits the trick color";
              return false;
          }
        }

        return true;
    }

    bool game_state::play_card(player* player, const std::string& card_id, std::string& err){
        card* card = nullptr;
        if (player->get_hand()->try_get_card(card_id, card) == false) { //also gives us card pointer
            err = "This card is not in your hand.";
        return false;
        }
        if (!can_be_played(player, card, err)){
          return false;
        }

        // play card
        _trick->add_card(card, player, err); //also sets trick color
        // TODO: this remove_card tries to access the private remove_card function of the hand class, so we should either
        // TODO: make that one public, or (possible better) change the inputs here to access the public remove_card function
        player->get_hand()->remove_card(card);

        update_current_player(err);
        return true;
    }

    bool game_state::remove_player(player *player_ptr, std::string &err) {
        int idx = get_player_index(player_ptr);
        if (idx != -1) {
            if (_is_started->get_value() == false) {
                if (idx < _current_player_idx->get_value()) {
                    // reduce current_player_idx if the player who left had a lower index
                    _current_player_idx->set_value(_current_player_idx->get_value() - 1);
                }
                _players.erase(_players.begin() + idx);
                return true;
            } else {
                finish_game(err);
            }
        } else {
            err = "Could not leave game, as the requested player was not found in that game.";
            return false;
        }
        return false;
    }

    bool game_state::add_player(player* player, std::string& err) {
        if (_is_started->get_value()) {
            err = "Could not join game, because the requested game is already started.";
            return false;
        }
        if (_is_finished->get_value()) {
            err = "Could not join game, because the requested game is already finished.";
            return false;
        }
        if (_players.size() >= _max_nof_players) {
            err = "Could not join game, because the max number of players is already reached.";
            return false;
        }
        if (std::ranges::find(_players, player) != _players.end()) {
            err = "Could not join game, because this player is already subscribed to this game.";
            return false;
        }

        _players.push_back(player);
        return true;
    }

    bool game_state::finish_game(std::string &err) {
        _is_finished->set_value(true);
        // TODO: part to determine winner of the game, it is done in game controller right now and could stay there.
        return true;
    }

#endif


// Serializable interface
void game_state::write_into_json(rapidjson::Value &json,
                                 rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    unique_serializable::write_into_json(json, allocator);

    json.AddMember("players", vector_utils::serialize_vector(_players, allocator), allocator);

    rapidjson::Value deck_val(rapidjson::kObjectType);
    _deck->write_into_json(deck_val, allocator);
    json.AddMember("deck", deck_val, allocator);

    rapidjson::Value trick_val(rapidjson::kObjectType);
    _trick->write_into_json(trick_val, allocator);
    json.AddMember("trick", trick_val, allocator);



    rapidjson::Value is_finished_val(rapidjson::kObjectType);
    _is_finished->write_into_json(is_finished_val, allocator);
    json.AddMember("is_finished", is_finished_val, allocator);

    rapidjson::Value is_started_val(rapidjson::kObjectType);
    _is_started->write_into_json(is_started_val, allocator);
    json.AddMember("is_started", is_started_val, allocator);

    rapidjson::Value is_estimation_phase_val(rapidjson::kObjectType);
    _is_estimation_phase->write_into_json(is_estimation_phase_val, allocator);
    json.AddMember("is_estimation_phase", is_estimation_phase_val, allocator);



    rapidjson::Value round_number_val(rapidjson::kObjectType);
    _round_number->write_into_json(round_number_val, allocator);
    json.AddMember("round_number", round_number_val, allocator);

    rapidjson::Value trick_number_val(rapidjson::kObjectType);
    _trick_number->write_into_json(trick_number_val, allocator);
    json.AddMember("trick_number", trick_number_val, allocator);

    rapidjson::Value starting_player_idx_val(rapidjson::kObjectType);
    _starting_player_idx->write_into_json(starting_player_idx_val, allocator);
    json.AddMember("starting_player_idx", starting_player_idx_val, allocator);

    rapidjson::Value trick_starting_player_idx_val(rapidjson::kObjectType);
    _trick_starting_player_idx->write_into_json(trick_starting_player_idx_val, allocator);
    json.AddMember("trick_starting_player_idx", trick_starting_player_idx_val, allocator);

    rapidjson::Value current_player_idx_val(rapidjson::kObjectType);
    _current_player_idx->write_into_json(current_player_idx_val, allocator);
    json.AddMember("current_player_idx", current_player_idx_val, allocator);

    rapidjson::Value trump_color_val(rapidjson::kObjectType);
    _trump_color->write_into_json(trump_color_val, allocator);
    json.AddMember("trump_color", trump_color_val, allocator);

    rapidjson::Value trick_estimate_sum_val(rapidjson::kObjectType);
    _trick_estimate_sum->write_into_json(trick_estimate_sum_val, allocator);
    json.AddMember("trick_estimate_sum_val", trick_estimate_sum_val, allocator);

}


// TODO: implement that function
game_state* game_state::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id")
        && json.HasMember("players")
        && json.HasMember("deck")
        && json.HasMember("trick")

        && json.HasMember("is_finished")
        && json.HasMember("is_started")
        && json.HasMember("is_estimation_phase")

        && json.HasMember("round_number")
        && json.HasMember("trick_number")
        && json.HasMember("starting_player_idx")
        && json.HasMember("trick_starting_player_idx")
        && json.HasMember("current_player_idx")
        && json.HasMember("trump_color")
        && json.HasMember("trick_estimate_sum_val"))
    {
        std::vector<player*> deserialized_players;
        for (auto &serialized_player : json["players"].GetArray()) {
            deserialized_players.push_back(player::from_json(serialized_player.GetObject()));
        }
        return new game_state(json["id"].GetString(),
                              deserialized_players,
                              deck::from_json(json["deck"].GetObject()),
                              trick::from_json(json["trick"].GetObject()),

                              serializable_value<bool>::from_json(json["is_started"].GetObject()),
                              serializable_value<bool>::from_json(json["is_finished"].GetObject()),
                              serializable_value<bool>::from_json((json["is_estimation_phase"].GetObject())),

                              serializable_value<int>::from_json(json["round_number"].GetObject()),
                              serializable_value<int>::from_json(json["trick_number"].GetObject()),
                              serializable_value<int>::from_json(json["starting_player_idx"].GetObject()),
                              serializable_value<int>::from_json(json["trick_starting_player_idx"].GetObject()),
                              serializable_value<int>::from_json(json["current_player_idx"].GetObject()),
                              serializable_value<int>::from_json(json["trump_color"].GetObject()),
                              serializable_value<int>::from_json(json["trick_estimate_sum_val"].GetObject()));
    }
    throw WizardException("Failed to deserialize game_state. Required entries were missing.");
}