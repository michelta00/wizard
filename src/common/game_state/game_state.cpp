#include "game_state.h"
#include <vector>

#include "../exceptions/WizardException.h"
#include "../serialization/vector_utils.h"

// from_diff constructor
game_state::game_state(const std::string& id) : unique_serializable(id) {
    _players = std::vector<player*>();
    _deck = new deck();
    _trick = new trick();
    _last_trick = new trick();

    _is_started = new serializable_value<bool>(false);
    _is_finished = new serializable_value<bool>(false);
    _is_estimation_phase = new serializable_value<bool>(true);

    _round_number = new serializable_value<int>(0);
    _trick_number = new serializable_value<int>(0);
    _starting_player_idx = new serializable_value<int>(0);
    _trick_starting_player_idx = new serializable_value<int>(0);
    _current_player_idx = new serializable_value<int>(0);
    _trump_color = new serializable_value<int>(0);
    _trick_estimate_sum = new serializable_value<int>(0);
}

// deserialization constructor
game_state::game_state(const std::string& id, const std::vector<player*>& players, deck* deck, trick* current_trick,
                       trick* last_trick, serializable_value<bool>* is_started, serializable_value<bool>* is_finished,
                       serializable_value<bool>* is_estimation_phase, serializable_value<int>* round_number,
                       serializable_value<int>* trick_number, serializable_value<int>* starting_player_idx,
                       serializable_value<int>* trick_starting_player_idx, serializable_value<int>* current_player_idx,
                       serializable_value<int>* trump_color, serializable_value<int>* trick_estimate_sum)
        : unique_serializable(id),
          _players(players),
          _deck(deck),
          _trick(current_trick),
          _last_trick(last_trick),

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
game_state::game_state() : unique_serializable()
{
    _players = std::vector<player*>();
    _deck = new deck();
    _trick = new trick();
    _last_trick = new trick();

    _is_started = new serializable_value<bool>(false);
    _is_finished = new serializable_value<bool>(false);
    _is_estimation_phase = new serializable_value<bool>(true);

    _round_number = new serializable_value<int>(0);
    _trick_number = new serializable_value<int>(0);
    _starting_player_idx = new serializable_value<int>(0);
    _trick_starting_player_idx = new serializable_value<int>(0);
    _current_player_idx = new serializable_value<int>(0);
    _trump_color = new serializable_value<int>(0);
    _trick_estimate_sum = new serializable_value<int>(0);
}

// destructor
game_state::~game_state()
{
    _players.clear();
    delete _deck;
    delete _trick;
    delete _last_trick;

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

    _deck = nullptr;
    _trick = nullptr;
    _last_trick = nullptr;

    _is_started = nullptr;
    _is_finished = nullptr;
    _is_estimation_phase = nullptr;

    _round_number = nullptr;
    _trick_number = nullptr;
    _starting_player_idx = nullptr;
    _trick_starting_player_idx = nullptr;
    _current_player_idx = nullptr;
    _trump_color = nullptr;
    _trick_estimate_sum = nullptr;
}

// accessors
player* game_state::get_current_player() const
{
    if(_current_player_idx == nullptr || _players.empty()) {
        return nullptr;
    }
    return _players[_current_player_idx->get_value()];
}

bool game_state::is_estimation_phase() const
{
    return _is_estimation_phase->get_value();
}

int game_state::get_trump_color() const
{
    return _trump_color->get_value();
}

player* game_state::get_trick_starting_player() const
{
    if(_trick_starting_player_idx == nullptr || _players.empty()) {
        return nullptr;
    }
    return _players[_trick_starting_player_idx->get_value()];
}

player* game_state::get_starting_player() const
{
    if(_starting_player_idx == nullptr || _players.empty()) {
        return nullptr;
    }
    return _players[_starting_player_idx->get_value()];
}

trick* game_state::get_trick() const
{
    return _trick;
}

trick* game_state::get_last_trick() const
{
    return _last_trick;
}

bool game_state::is_full() const
{
    return _players.size() == _max_nof_players;
}

bool game_state::is_started() const
{
    return _is_started->get_value();
}

bool game_state::is_finished() const
{
    return _is_finished->get_value();
}

int game_state::get_round_number() const
{
    return _round_number->get_value();
}

int game_state::get_trick_number() const
{
    return _trick_number->get_value();
}

int game_state::get_trick_estimate_sum() const
{
    return _trick_estimate_sum->get_value();
}

unsigned int game_state::get_max_round_number() const
{
    return 1;
    return 60 / _players.size();
}

bool game_state::is_player_in_game(player *player) const
{
    return std::ranges::find(_players, player) < _players.end();
}

std::vector<player*>& game_state::get_players()
{
    return _players;
}

void game_state::set_trick(trick* new_trick)
{
    if(new_trick != nullptr){
        delete _trick;
        _trick = new_trick;
    }
}

#ifdef WIZARD_SERVER
// private functions used on server side
int game_state::get_player_index(player *player) const
{
    if (const auto it = std::ranges::find(_players, player); it == _players.end()) {
        return -1;
    } else {
        return it - _players.begin();
    }
}

void game_state::determine_trump_color() const
{
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

// return true if this card can be played according to the game rules (checked in that order):
// if there is no trick color yet (trick color is 0) or a wizard has set the trick color (-1), then the card can be played
// if the card is a jester or wizard, then it can be played
// it the card has the same color as the trick color, it can be played
// if the card does not have the same color as the trick color, it can only be played if no other card
// on the players hand has the same color as the trick color
bool game_state::can_be_played(player* player, const card* card, std::string& err) const noexcept
{
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

unsigned int game_state::get_number_of_turns() const
{
    const unsigned int nof_players = this->_players.size();

    if (this->_current_player_idx->get_value() >= this->_starting_player_idx->get_value()) {
        return (this->_current_player_idx->get_value() - this->_starting_player_idx->get_value() + 1); //in first round 0
    }
    else{
        return (this->_current_player_idx->get_value() - this->_starting_player_idx->get_value() + nof_players) % nof_players + 1;
    }
}

// state modification functions without diff
void game_state::wrap_up_round(std::string& err)
{
    _starting_player_idx->set_value((_starting_player_idx->get_value() + 1) % _players.size());

    for (const auto & player : _players) {
      player->wrap_up_round();
    }
    if (_round_number->get_value() == get_max_round_number() - 1) {
      finish_game(err);
    }
}

bool game_state::update_current_player(std::string& err)
{
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
                if (_trick != nullptr) {
                    delete _last_trick;
                    _last_trick = new trick(*_trick);
                }
      	        _trick->set_up_round(_trump_color->get_value(), err);

                // winner of trick is starting player of next trick
                if(const int winner_index = get_player_index(winner); winner_index == -1){
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


void game_state::setup_round(std::string &err)
{

    _trick_estimate_sum->set_value(0);
    _is_estimation_phase->set_value(true);
    _trick_number->set_value(0); //tricks numbers start by 0
    _trick_starting_player_idx->set_value(_starting_player_idx->get_value());
    _current_player_idx->set_value(_starting_player_idx->get_value());

    _deck->setup_round();
    for (auto & player : _players) {
        player->setup_round();
        _deck->draw_cards(player, _round_number->get_value() + 1, err);
    }
    determine_trump_color();
    if (_trick != nullptr) {
        delete _last_trick;
        _last_trick = new trick(*_trick);
    }
    _trick->set_up_round(_trump_color->get_value(), err);
}

bool game_state::start_game(std::string &err)
{
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

bool game_state::estimate_tricks(const player *player, std::string &err, const int trick_estimate)
{
    if (trick_estimate > get_round_number() + 1) {
        err = "Trick estimate is too big. You can't win more tricks than cards in your hand.";
        return false;
    }
    if (trick_estimate < 0) {
        err = "Trick estimate is too small. You can't win less than 0 tricks.";
        return false;
    }
    if(get_number_of_turns() == this->_players.size() &&
       trick_estimate + _trick_estimate_sum->get_value() == _round_number->get_value() + 1) {
        err = "The tricks can't add up to the exact number of cards in the round. Please either choose a higher or lower number of tricks.";
        return false;
    }

    player->set_nof_predicted(trick_estimate);
    _trick_estimate_sum->set_value(trick_estimate + _trick_estimate_sum->get_value());
    return update_current_player(err); //handles logic to switch from estimation to playing round etc.
}


bool game_state::play_card(player* player, const std::string& card_id, std::string& err)
{
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

    // adapted remove_card function in hand (doesnt need to save removed card anymore)
    player->get_hand()->remove_card(card_id, err);

    update_current_player(err);
    return true;
}

bool game_state::remove_player(player *player_ptr, std::string &err)
{
    if (const int idx = get_player_index(player_ptr); idx != -1) {
        if (_is_started->get_value() == false) {
            if (idx < _current_player_idx->get_value()) {
                // reduce current_player_idx if the player who left had a lower index
                _current_player_idx->set_value(_current_player_idx->get_value() - 1);
            }
            _players.erase(_players.begin() + idx);
            return true;
        } else {
            finish_game(err);
            return true;
        }
    } else {
        err = "Could not leave game, as the requested player was not found in that game.";
        return false;
    }
    return false;
}

bool game_state::add_player(player* player, std::string& err)
{
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

bool game_state::finish_game(std::string &err) const
{
    _is_finished->set_value(true);
    // TODO: part to determine winner of the game, it is done in game controller right now and could stay there.
    return true;
}
#endif

// serializable interface
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

    rapidjson::Value last_trick_val(rapidjson::kObjectType);
    _last_trick->write_into_json(last_trick_val, allocator);
    json.AddMember("last_trick", last_trick_val, allocator);



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

game_state* game_state::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id")
        && json.HasMember("players")
        && json.HasMember("deck")
        && json.HasMember("trick")
        && json.HasMember("last_trick")

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
                              trick::from_json(json["last_trick"].GetObject()),

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