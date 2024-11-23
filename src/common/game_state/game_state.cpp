#include "game_state.h"
#include "../exceptions/WizardException.h"
#include "../serialization/vector_utils.h"

// public constructor
game_state::game_state() : unique_serializable() {
    this->_players = std::vector<player*>();
    this->_round_state = nullptr; //round state only initialized when round is set up
    this->_is_started = new serializable_value<bool>(false);
    this->_is_finished = new serializable_value<bool>(false);
    this->_current_player_idx = new serializable_value<int>(0);
    this->_round_number = new serializable_value<int>(0);
}

// deserialization constructor
game_state::game_state(std::string id, std::vector<player*>& players, round_state* round_state,
                       serializable_value<bool>* is_started, serializable_value<bool>* is_finished,
                       serializable_value<int>* round_number, serializable_value<int>* current_player_idx)
        : unique_serializable(id),
          _players(players),
      	  _round_state(round_state),
          _is_started(is_started),
          _is_finished(is_finished),
          _round_number(round_number),
          _current_player_idx(current_player_idx)
{ }

// from_diff constructor
game_state::game_state(std::string id) : unique_serializable(id) {
    this->_players = std::vector<player*>();
    this->_round_state = nullptr;
    this->_is_started = new serializable_value<bool>(false);
    this->_is_finished = new serializable_value<bool>(false);
    this->_current_player_idx = new serializable_value<int>(0);
    this->_round_number = new serializable_value<int>(0);
}

game_state::~game_state() {
    if (_is_started != nullptr) {
        // for some reason players doesn't have to be deleted in Lama
        delete _round_state;
        delete _is_started;
        delete _is_finished;
        delete _current_player_idx;
        delete _round_number;

        _round_state = nullptr;
        _is_started = nullptr;
        _is_finished = nullptr;
        _current_player_idx = nullptr;
        _round_number = nullptr;
    }
}

// accessors
player* game_state::get_current_player() const {
    if(_current_player_idx == nullptr || _players.empty()) {
        return nullptr;
    }
    return _players[_current_player_idx->get_value()];
}

round_state* game_state::get_round_state() const {
  return _round_state;
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





//#ifdef WIZARD_SERVER

// state modification functions without diff

void game_state::increase_round_number() {
  int number = get_round_number() + 1;
  _round_number->set_value(number);
}

void game_state::update_current_player() {
    _current_player_idx->set_value((_current_player_idx->get_value() + 1) % _players.size());
}

void game_state::setup_round(std::string &err) {


    increase_round_number();
    update_current_player();

    //TODO: possibly set up other parts of round (players, deck, trick, ...) --> maybe do in round state

    // Provide the callback to be called when the round ends
    _round_state->set_on_round_end([this]() {  // [this] captures the game_state instance
        if (get_round_number() < get_max_round_number()) {
            setup_round(err);  // Start a new round
        } else {
            //TODO: possibly more complicated finish game logic
            this->_is_finished->set_value(true);  // End the game
            finish_game(err);

        }
    });
}

bool game_state::start_game(std::string &err) {
    if (_players.size() < _min_nof_players) {
        err = "You need at least " + std::to_string(_min_nof_players) + " players to start the game.";
        return false;
    }

    if (!_is_started->get_value()) {
        this->_is_started->set_value(true);
        // TODO: think about whether this will cause a problem since setup_round iteratively calls it self and start_game only ends when game has ended
        if (_round_state) {
        	delete _round_state;  // Clean up previous round state if it exists
        	_round_state = nullptr;
    	}
    	_round_state = new round_state(_players, _current_player_idx, _round_number);
        this->setup_round(err); //iteratively starts all rounds until game is over
        return true;
    } else {
        err = "Could not start game, as the game was already started";
        return false;
    }
}

bool game_state::remove_player(player *player_ptr, std::string &err) {
    int idx = get_player_index(player_ptr);
    if (idx != -1) {
        if (idx < _current_player_idx->get_value()) {
            // reduce current_player_idx if the player who left had a lower index
            _current_player_idx->set_value(_current_player_idx->get_value() - 1);
        }
        _players.erase(_players.begin() + idx);
        return true;
    } else {
        err = "Could not leave game, as the requested player was not found in that game.";
        return false;
    }
}


bool game_state::add_player(player* player_ptr, std::string& err) {
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
    if (std::find(_players.begin(), _players.end(), player_ptr) != _players.end()) {
        err = "Could not join game, because this player is already subscribed to this game.";
        return false;
    }

    _players.push_back(player_ptr);
    return true;
}

bool game_state::finish_game(std::string &err) {
  // TODO: part to determine winner of the game, show game over message and send players their final score
  return true;
}

//#endif


// Serializable interface
void game_state::write_into_json(rapidjson::Value &json,
                                 rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    unique_serializable::write_into_json(json, allocator);

    rapidjson::Value is_finished_val(rapidjson::kObjectType);
    _is_finished->write_into_json(is_finished_val, allocator);
    json.AddMember("is_finished", is_finished_val, allocator);

    rapidjson::Value is_started_val(rapidjson::kObjectType);
    _is_started->write_into_json(is_started_val, allocator);
    json.AddMember("is_started", is_started_val, allocator);

    rapidjson::Value current_player_idx_val(rapidjson::kObjectType);
    _current_player_idx->write_into_json(current_player_idx_val, allocator);
    json.AddMember("current_player_idx", current_player_idx_val, allocator);

    rapidjson::Value starting_player_idx_val(rapidjson::kObjectType);
    _starting_player_idx->write_into_json(starting_player_idx_val, allocator);
    json.AddMember("starting_player_idx", starting_player_idx_val, allocator);

    rapidjson::Value round_number_val(rapidjson::kObjectType);
    _round_number->write_into_json(round_number_val, allocator);
    json.AddMember("round_number", round_number_val, allocator);

    rapidjson::Value round_state_val(rapidjson::kObjectType);
    _round_state->write_into_json(round_state_val, allocator);
    json.AddMember("round_state", round_state_val, allocator);

    json.AddMember("players", vector_utils::serialize_vector(_players, allocator), allocator);
}


game_state* game_state::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id")
        && json.HasMember("is_finished")
        && json.HasMember("is_started")
        && json.HasMember("current_player_idx")
        && json.HasMember("round_number")
        && json.HasMember("starting_player_idx")
        && json.HasMember("players")
        && json.HasMember("round_state"))
    {
        std::vector<player*> deserialized_players;
        for (auto &serialized_player : json["players"].GetArray()) {
            deserialized_players.push_back(player::from_json(serialized_player.GetObject()));
        }
        return new game_state(json["id"].GetString(),
                              deserialized_players,
                              round_state::from_json(json["round_state"].GetObject()),
                              serializable_value<bool>::from_json(json["is_started"].GetObject()),
                              serializable_value<bool>::from_json(json["is_finished"].GetObject()),
                              serializable_value<int>::from_json(json["current_player_idx"].GetObject()),
                              serializable_value<int>::from_json(json["round_number"].GetObject()),
                              serializable_value<int>::from_json(json["starting_player_idx"].GetObject()));


    } else {
        throw WizardException("Failed to deserialize game_state. Required entries were missing.");
    }
}