//
// Created by Marie Becker on 08.11.24.
//
#include "round_state.h"

#include "../exceptions/WizardException.h"
#include "../serialization/vector_utils.h"


round_state::round_state(const std::vector<player*>& players, serializable_value<int>* starting_player_idx,
                         serializable_value<int>* round_number) : unique_serializable() {
    this->_starting_player_idx = starting_player_idx;
    this->_current_player_idx = starting_player_idx; //current player is starting player when round is created
    this->_players = players;
    this->_deck = new deck();
    this->_trick = new trick(); //attributes need to possibly be added to trick constructor
    this->_trump_color = new serializable_value<int>(0); //no trump set when round is created
    this->_is_finished = new serializable_value<bool>(false);
    this->_is_estimation_phase = new serializable_value<bool>(true);
    this->_round_number = round_number;
    this->_trick_estimate_sum = new serializable_value<int>(0);
    this->_on_round_end = nullptr;
}

round_state::round_state(std::string id, const std::vector<player*>& players, serializable_value<int>* starting_player_idx,
                         serializable_value<int>* round_number) : unique_serializable(id) {
    this->_starting_player_idx = starting_player_idx;
    this->_current_player_idx = starting_player_idx; //current player is starting player when round is created
    this->_players = players;
    this->_deck = new deck();
    this->_trick = new trick(); //attributes need to possibly be added to trick constructor
    this->_trump_color = new serializable_value<int>(0); //no trump set when round is created
    this->_is_finished = new serializable_value<bool>(false);
    this->_is_estimation_phase = new serializable_value<bool>(true);
    this->_round_number = round_number;
    this->_trick_estimate_sum = new serializable_value<int>(0);
    this->_on_round_end = nullptr;
}

round_state::round_state(
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
            serializable_value<int>* trick_estimate_sum,
            std::function<void()> on_round_end): unique_serializable(id)
{
    this->_starting_player_idx = starting_player_idx;
    this->_current_player_idx = current_player_idx; //current player is starting player when round is created
    this->_players = players;
    this->_deck = deck;
    this->_trick = current_trick; //attributes need to possibly be added to trick constructor
    this->_trump_color = trump_color; //no trump set when round is created
    this->_is_finished = is_finished;
    this->_is_estimation_phase = is_estimation_phase;
    this->_round_number = round_number;
    this->_trick_estimate_sum = trick_estimate_sum;
    this->_on_round_end = on_round_end;
}


round_state::~round_state() {
    // for some reason players doesn't have to be deleted in Lama
        delete _deck;
        delete _trick;
        delete _is_finished;
        delete _is_estimation_phase;
        delete _current_player_idx;
        delete _starting_player_idx;
        delete _trump_color;
        delete _round_number;
        delete _trick_estimate_sum;

        _deck = nullptr;
        _trick = nullptr;
        _is_finished = nullptr;
        _is_estimation_phase = nullptr;
        _current_player_idx = nullptr;
        _starting_player_idx = nullptr;
        _trump_color = nullptr;
        _round_number = nullptr;
        _trick_estimate_sum = nullptr;
}

// accessors
player* round_state::get_current_player() const {
    if(_current_player_idx == nullptr || _players.size() == 0) {
        return nullptr;
    }
    return _players[_current_player_idx->get_value()];
}

player* round_state::get_starting_player() const {
  if(_starting_player_idx == nullptr || _players.size() == 0) {
        return nullptr;
    }
    return _players[_starting_player_idx->get_value()];
}

bool round_state::is_finished() const {
    return _is_finished->get_value();
}

bool round_state::is_estimation_phase() const {
  return _is_estimation_phase->get_value();
}

int round_state::get_round_number() const {
    return _round_number->get_value();
}

std::vector<player*>& round_state::get_players() {
    return _players;
}

deck* round_state::get_deck() const {
  return _deck;
}

trick* round_state::get_trick() const {
  return _trick;
}

int round_state::get_trump_color() const {
  return _trump_color->get_value();
}

int round_state::get_trick_estimate_sum() const {
  return _trick_estimate_sum->get_value();
}


// setter
void round_state::set_trump_color(const int trump_color) {
  this->_trump_color->set_value(trump_color);
}

void round_state::set_is_estimation_phase(bool is_estimation_phase){
  this->_is_estimation_phase->set_value(is_estimation_phase);
}

void round_state::set_finished(bool is_finished){
  this->_is_finished->set_value(is_finished);
}

void set_round_number(int round_number){
  this->_round_number->set_value(round_number);
}

void set_starting_player_idx(player* starting_player_idx){
  this->_starting_player_idx->set_value(starting_player_idx);
}

void set_current_player_idx(player* current_player_idx){
  this->_current_player_idx->set_value(current_player_idx);
}


#ifdef WIZARD_SERVER

// Method to set the callback
void round_state::set_on_round_end(std::function<void()> callback) {
    _on_round_end = callback;
}

// Call this when the round ends
void round_state::finish_round() {
    if (_on_round_end) {  // Check if the callback is set
        _on_round_end();  // Invoke the callback
    }
}

void round_state::determine_trump_color(std::string& err, deck* deck) {
  //TODO
}

// set up round
void round_state::setup_round(std::string& err, const int round_number, const int starting_player_idx){
  this->_starting_player_idx->set_value(starting_player_idx);
  this->_current_player_idx->set_value(starting_player_idx);
  this->_round_number->set_value(round_number);

  determine_trump_color(err, this->_deck);


  //TODO: possibly set up other parts of round (players, deck, trick, ...)
}
