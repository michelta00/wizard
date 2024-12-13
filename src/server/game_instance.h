#ifndef WIZARD_GAME_H
#define WIZARD_GAME_H

#include <vector>
#include <string>
#include <mutex>

#include "../common/game_state/player/player.h"
#include "../common/game_state/game_state.h"

/**
 * @class game_instance
 * @brief Class that modifies game state based on content received from client.
 * The game instance functionalities are called by the request handler via the game instance manager.
 * It handles the final step of interacting with the game instance (and the actual game logic).
 */
class game_instance {

private:
    game_state* _game_state; ///< Game state that is modified.
    inline static std::mutex modification_lock; ///< Mutex which makes sure that game state is only modified by one player at a time.

public:
    /**
     * @brief Constructs a new game instance object.
     */
    game_instance();
    /**
     * @brief Destructs game instance object
     */
    ~game_instance() {
        if (_game_state != nullptr) {
            delete _game_state;
        }
        _game_state = nullptr;
    }

    /**
     * @brief Accessor of game instance id.
     * @return id of game instance
     */
    std::string get_id();
    /**
     * @brief Accessor of current game state.
     * @return Current game state.
     */
    game_state* get_game_state();

    /**
     * @brief Checks whether game is already full.
     * The game is full if 6 players have joined already.
     * @return Boolean which states whether game is full.
     */
    bool is_full();
    /**
     * @brief Checks whether game is started.
     * @return Boolean that states whether game is started.
     */
    bool is_started();
    /**
     * @brief Checks whether game is finished.
     * @return Boolean that states whether game is finished.
     */
    bool is_finished();

    // game update functions
    /**
     * @brief Attempts to start the game.
     * @param player Player who started the game.
     * @param err Contains error message if something goes wrong while starting the game.
     * @return Boolean that states whether game could be started successfully
     */
    bool start_game(player* player, std::string& err);
    /**
     * @brief Attempts to add player to the game.
     * @param new_player Pointer to player that wants to join the game.
     * @param err Contains error message that possibly states what went wrong while joining the game.
     * @return Boolean which states whether player could successfully join the game.
     */
    bool try_add_player(player* new_player, std::string& err);
    /**
     * @brief Attempts to remove player from the game.
     * If the game hasn't started yet, the player can be easily removed from the game.
     * If the game has already started it is finished immediately.
     * @param player Pointer to player that leaves the game.
     * @param err Error message which states if the reason why the player couldn't leave the game if something went wrong.
     * @return Boolean which states whether the player successfully left the game.
     */
    bool try_remove_player(player* player, std::string& err);
    /**
     * @brief Attempts to play a card.
     * @param player Pointer to player who plays the card.
     * @param card_id Id of the card.
     * @param err Error message which contains possible errors.
     * @return Boolean which states whether card could be played successfully.
     */
    bool play_card(player* player, const std::string& card_id, std::string& err);
    /**
     * @brief Attempts to estimate tricks.
     * @param player Pointer to player who estimates tricks.
     * @param err Error message which contains possible errors.
     * @param nof_tricks Predicted number of tricks to win.
     * @return Boolean which states whether trick estimation was successful.
     */
    bool estimate_tricks(player *player, std::string& err, int nof_tricks);

};


#endif //WIZARD_GAME_H

