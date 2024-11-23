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
    this->_current_trick_number = new serializable_value<int>(0);
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
    this->_current_trick_number = new serializable_value<int>(0);
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
    		serializable_value<int>* current_trick_number,
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
    this->_current_trick_number = current_trick_number;
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

int round_state::get_player_idx()


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

void set_trick_estimate_sum(int trick_estimate_sum){
  this->_trick_estimate_sum->set_value(trick_estimate_sum);
}


#ifdef WIZARD_SERVER

// Method to set the callback
void round_state::set_on_round_end(std::function<void()> callback) {
    _on_round_end = callback;
}

// Call this when the round ends
void round_state::finish_round() {
 	_is_finished->set_value(true);

    //TODO: needs wrap up that calculates scores

    if (_on_round_end) {  // Check if the callback is set
        _on_round_end();  // Invoke the callback
    }
}

void round_state::determine_trump_color() {
  if(_round_number->get_value() * _players.size() == 60){
    _trump_color->set_value(0); // there is no trump
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

// set up round
void round_state::setup_round(std::string& err, const int round_number, const int starting_player_idx){
  this->_starting_player_idx->set_value(starting_player_idx);
  this->_current_player_idx->set_value(starting_player_idx);
  this->_round_number->set_value(round_number);

  _trick_estimate_sum->set_value(0);
  _is_estimation_phase->set_value(true);
  _current_trick_number->set_value(1); //tricks have number 1, ..., round_number

  _deck->setup_round();
  for (int i = 0; i < _players.size(); i++) {
    _players[i]->setup_round();
    _deck->draw_cards(_players[i], _round_number->get_value(), err);
  }
  determine_trump_color();

  _trick->set_up_round(err, _trump_color->get_value());
}

void round_state::update_current_player(std::string& err){
  //TODO: new starting player is player who won trick
  _current_player_idx->set_value((_current_player_idx->get_value() + 1) % _players.size());

  //if current player is last player of round, switch from estimation to playing or end round and send callback to game_state
  if (_current_player_idx->get_value() == _starting_player_idx->get_value()){
    if (_is_estiamtion_phase->get_value() == true) {
    	_is_estimation_phase->set_value(false);
    }
    else if (_current_trick_number->get_value() < _round_number->get_value()){ //not final trick
      	_current_trick_number->set_value(_current_trick_number->get_value() + 1);
      	_trick->set_up_round();
        player* winner = wrap_up_trick(err);
        // TODO: adapt player trick count etc.
        _starting_player_idx->set_value()
    }
	else {
      //TODO: don't finish round while not all players have played
    	finish_round();
    }
  }
}

// TODO: add play_card function and adapt can_be_played to the used by that function as in SDS
bool round_state::can_be_played(const trick* const current_trick, const hand* const current_hand, const card* card) const noexcept {
    // return true if this card can be played according to the game rules (checked in that order):
    // if there is no trick color yet (trick color is 0), then the card can be played
    // if the card is a jester or wizard, then it can be played
    // it the card has the same color as the trick color, it can be played
    // if the card does not have the same color as the trick color, it can only be played if no other card
    // on the players hand has the same color as the trick color

    // get trick color -> if 0, card can be played
    int trick_color = current_trick->get_trick_color();
    if (trick_color == 0) return true;

    // get value of this card
    std::pair<int, int> value = this->get_value();

    // check if card is wizard or jester -> if yes, card can be played
    if (value.first == 0 || value.first == 14) return true;

    // if card is not wizard or jester, compare with trick color -> if same, can be played
    if (value.second == trick_color) return true;

    // if card does not have trick color, check other cards on hand
    const std::vector<card*> cards = current_hand->get_cards();
    for (auto it = cards.begin(); it != cards.end(); ++it)
    {
        std::pair<int, int> hand_value = (*it)->get_value();
        // if there is a card on the hand with the trick color, the current card cannot be played
        if (hand_value.second == trick_color) return false;
    }

    return true;
}

bool play_card(player* player, const std::string& card_id, std::string& err){
  // TODO: handle card id or card question
  if (can_be_played(player->get_hand(), )){
    //TODO: play card
  }
}

int get_number_of_turns(){
  int nof_players = this->_players.size();

  if (this->_current_player_idx->get_value() >= this->_starting_player_idx->get_value()) {
    return (this->_current_player_idx->get_value() - this->_starting_player_idx->get_value() + 1); //in first round 0
  }
  else{
    return (this->_current_player_idx->get_value() - this->_starting_player_idx->get_value() + nof_players) % nof_players + 1;
  }
}

bool round_state::estimate_tricks(player *player, std::string &err, int trick_estimate){

  if (trick_estimate > get_round_number()) {
    err = "Trick estimate is too big. You can't win more tricks than cards in your hand.";
    return false;
  }
  if(trick_estimate < 0) {
    err = "Trick estimate is too small. You can't win less than 0 tricks."
    return false;
  }

  if(get_number_of_turns() == this->_players.size() && trick_estimate + _trick_estimate_sum->get_value() == _round_number->get_value()){
    err = "The tricks can't add up to the exact number of cards in the round. Please either choose a higher or lower number of tricks."
    return false;
  }

  player->set_trick_estimate(trick_estimate);
  _trick_estimate_sum->set_value(trick_estimate + _trick_estimate_sum->get_value());
  update_current_player(err); //handles logic to switch from estimation to playing round etc.
  return true;
}


#endif


// TODO: add from_json and write_into_json functions