//
// Created by Manuel on 25.01.2021.
//
// The game_instance class is a wrapper around the game_state of an active instance of the game.
// This class contains functions to modify the contained game_state.

#include "game_instance.h"

#include "server_network_manager.h"
#include "../common/network/responses/full_state_response.h"


game_instance::game_instance() {
    _game_state = new game_state();
}

game_state *game_instance::get_game_state() {
    return _game_state;
}

std::string game_instance::get_id() {
    return _game_state->get_id();
}

bool game_instance::is_full() {
    return _game_state->is_full();
}

bool game_instance::is_started() {
    return _game_state->is_started();
}

bool game_instance::is_finished() {
    return _game_state->is_finished();
}


bool game_instance::play_card(player *player, const std::string& card_id, std::string& err) {
    modification_lock.lock();
    if (_game_state->play_card(player, card_id, err)) {
        full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::estimate_tricks(player *player, std::string& err, int nof_tricks){
    modification_lock.lock();
    if (_game_state->estimate_tricks(player, err, nof_tricks)) {
        full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}


bool game_instance::start_game(player* player, std::string &err) {
    modification_lock.lock();
    if (_game_state->start_game(err)) {
        // send state update to all other players
        full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player); // issue here
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::try_remove_player(player *player, std::string &err) {
    modification_lock.lock();
    if(_game_state->remove_player(player, err))
    {
        // player->set_game_id("");
        full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        modification_lock.unlock();
        return true;
    }
    /*
    if (_game_state->is_started()) {
        // send state update to all other players
        _game_state->finish_game(err);
        full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        modification_lock.unlock();
        return true;
    */
    //}// else if (_game_state->remove_player(player, err)){
      //  player->set_game_id("");
        // send state update to all other players
      //  full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        //server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        //modification_lock.unlock();
        //return true;
    //}
    modification_lock.unlock();
    return false;
}

bool game_instance::try_add_player(player *new_player, std::string &err) {
    modification_lock.lock();
    if (_game_state->add_player(new_player, err)) {
        new_player->set_game_id(get_id());
        // send state update to all other players
        full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), new_player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

