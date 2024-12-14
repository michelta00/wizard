#ifndef WIZARD_GAME_STATE_H
#define WIZARD_GAME_STATE_H

#include <vector>
#include <ranges>
#include "../../rapidjson/include/rapidjson/document.h"
#include "player/player.h"
#include "cards/deck.h"
#include "cards/card.h"
#include "cards/trick.h"
#include "../serialization/serializable_value.h"
#include "../serialization/unique_serializable.h"

/**
 * @class game_state
 * @brief Represents the state of the game.
 *
 * This class encapsulates all information about a game, including the players, the deck, the trick, and some other
 * variables.
 *
 * The game_state class contains most of the game logic and also holds all the other common classes as members
 * (or as members of members, e.g., game_state has players and players have hands). It also contains information
 * about the progress and current state of the game, e.g., if the game has already started, is finished, or which
 * phase the game is in (estimation phase or play phase). Moreover, it also keeps track of who is currently able to
 * make predictions or play cards, and changes the state of the game according to the players' actions.
 */
class game_state : public unique_serializable {
private:

    static constexpr int _max_nof_players = 6;                  ///< The maximum number of players that can join a game.
    static constexpr int _min_nof_players = 3;                  ///< The minimum number of players needed to start a game.

    std::vector<player*> _players;                          ///< The players that joined the game
    deck* _deck;                                            ///< The game's deck.
    trick* _trick;                                          ///< The current trick of the game.
    trick* _last_trick;                                     ///< The previous trick.

    serializable_value<bool>* _is_started;                  ///< A boolean indicating whether the game is started.
    serializable_value<bool>* _is_finished;                 ///< A boolean indicating whether the game is finished.
    serializable_value<bool>* _is_estimation_phase;         ///< A boolean indicating whether the game is in estimation phase.
    //TODO: add boolean _determine_trump_color that can be used to tell player to decide on a trump color

    serializable_value<int>* _round_number;                 ///< The current round number.
    serializable_value<int>* _trick_number;                 ///< The current trick number.
    serializable_value<int>* _starting_player_idx;          ///< The index of the player that started the current round.
    serializable_value<int>* _trick_starting_player_idx;    ///< The index of the player that started the current trick.
    serializable_value<int>* _current_player_idx;           ///< The index of the player that is currently playing.
    serializable_value<int>* _trump_color;                  ///< The trump color of the current round.
    serializable_value<int>* _trick_estimate_sum;           ///< The sum of trick estimates.

// constructors
    /**
     * @brief Constructs a new game_state object (from_diff).
     * @param id The game state's id.
     */
    explicit game_state(const std::string& id);

    /**
     * @brief Constructs a new game_state object during deserialization.
     * @param id The game state's id.
     * @param players The game state's players.
     * @param deck The game state's deck.
     * @param current_trick The game state's current trick.
     * @param last_trick The game state's previous trick.
     * @param is_started A boolean indicating whether the game is started.
     * @param is_finished A boolean indicating whether the game is finished.
     * @param is_estimation_phase A boolean indicating whether the game is in estimation phase.
     * @param round_number The round number.
     * @param trick_number The trick number.
     * @param starting_player_idx The round starting player's index.
     * @param trick_starting_player_idx The trick starting player's index.
     * @param current_player_idx The current player's index.
     * @param trump_color The trump color.
     * @param trick_estimate_sum The sum of trick estimates.
     */
    game_state(
            const std::string& id,
            const std::vector<player*>& players,
            deck* deck,
            trick* current_trick,
            trick* last_trick,

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

#ifdef WIZARD_SERVER
// private functions (only used by game_state member functions)
    /**
     * @brief Gets the index of the input player in the players vector.
     * @param player The player whose index is searched.
     * @return The player's index in the players vector.
     */
    int get_player_index(player* player) const;

    /**
     * @brief Gets the number of turns already played.
     * @return The number of turns already played.
     *
     * This function is used by the estimate_tricks method of the game state to check correct trick estimation of the
     * last player to estimate tricks. This player's trick estimate cannot lead to a trick estimate sum that equals the
     * round number, thus the last player must be treated differently and is determined by the number of turns already
     * played.
     */
    [[nodiscard]] unsigned int get_number_of_turns() const;

    /**
     * @brief Determines the trump color.
     *
     * This function determines the trump color of a round by calling the draw_trump function from deck after the cards
     * have been dealt. If the drawn card is a jester, the trump color is set to no color (0) for this round. If the
     * drawn card is a wizard, in the real game the starting player can choose a trump color. This is not implemented
     * yet in our game, in this case we randomly select one of the four colors so far.
     */
    void determine_trump_color() const;

    /**
     * @brief Checks if a given card can be played by a given player.
     * @param player The player that tries to play the card.
     * @param card The card that is played.
     * @param err The error message updated in case something does not work.
     * @return A boolean indicating whether the card can be played by the player.
     *
     * This function checks if a given card can be played by a given player according to the game rules. The following
     * points are checked in the given order:
     * If there is no trick color yet (trick color is 0) or a wizard has set the trick color (-1), then the card can be
     * played.
     * If the card is a jester or wizard, then it can be played.
     * If the card has the same color as the trick color, it can be played.
     * If the card does not have the same color as the trick color, it can only be played if no other card on the players
     * hand has the same color as the trick color.
     */
    bool can_be_played(player* player, const card* card, std::string& err) const noexcept;
#endif

public:
    /**
     * @brief Constructs a new game_state object.
     */
    game_state();

    /**
     * Destructs a trick object.
     */
    ~game_state() override;

// accessors
    /**
     * @brief Checks if the game is full.
     * @return A boolean indicating whether the game is full
     */
    [[nodiscard]] bool is_full() const;

    /**
     * @brief Checks if the game is started.
     * @return A boolean indicating whether the game is started.
     */
    [[nodiscard]] bool is_started() const;

    /**
     * @brief Checks if the game is finished.
     * @return A boolean indicating whether the game is finished.
     */
    [[nodiscard]] bool is_finished() const;

    /**
     * @brief Checks if the game is in estimation phase.
     * @return A boolean indicating whether the game is in estimation phase.
     */
    [[nodiscard]] bool is_estimation_phase() const;

    /**
     * @brief Checks if the given player is in the game.
     * @param player The player that is checked.
     * @return A boolean indicating whether the given player is in the game.
     */
    bool is_player_in_game(player* player) const;

    /**
     * @brief Gets the round number.
     * @return The round number.
     */
    [[nodiscard]] int get_round_number() const;

    /**
     * @brief Gets the trick number.
     * @return The trick number.
     */
    [[nodiscard]] int get_trick_number() const;

    /**
     * @brief Gets the trick estimate sum.
     * @return The trick estimate sum.
     */
    [[nodiscard]] int get_trick_estimate_sum() const;

    /**
     * @brief Gets the maximum round number.
     * @return The maximum round number.
     *
     * The maximum number of rounds is determined by the number of players in the game. It is calculated by dividing
     * 60 by the number of players. With 3 players, there are 20 rounds, while with 6 players, there are 10 rounds.
     */
    [[nodiscard]] unsigned int get_max_round_number() const;

    /**
     * @brief Gets the trump color.
     * @return The trump color.
     */
    [[nodiscard]] int get_trump_color() const;

    /**
     * @brief Gets the current player.
     * @return The current player.
     */
    [[nodiscard]] player* get_current_player() const;

    /**
     * @brief Gets the starting player of the current trick.
     * @return The starting player of the current trick.
     */
    [[nodiscard]] player* get_trick_starting_player() const;

    /**
     * @brief Gets the starting player of the current round.
     * @return The starting player of the current round.
     */
    [[nodiscard]] player* get_starting_player() const;

    /**
     * @brief Gets the current trick.
     * @return The current trick.
     */
    [[nodiscard]] trick* get_trick() const;

    /**
     * @brief Gets the previous trick.
     * @return The previous trick.
     */
    [[nodiscard]] trick* get_last_trick() const;

    /**
     * @brief Gets the players that joined the game.
     * @return The players that joined the game.
     */
    std::vector<player*>& get_players();

    /**
     * @brief Sets the trick to the given trick.
     * @param new_trick The new trick.
     */
    void set_trick(trick* new_trick);

#ifdef WIZARD_SERVER
// server-side state update functions
    /**
     * @brief Removes a player from the game.
     * @param player The player that is removed.
     * @param err The error message updated in case something does not work.
     * @return A boolean indicating whether removing the player worked or not.
     *
     * This function tries to remove a player from the game. While this is done, the players vector is updated so that
     * the remaining players can start a new round.
     */
    bool remove_player(player* player, std::string& err);

    /**
     * @brief Adds a player to the game.
     * @param player The player that is added.
     * @param err The error message updated in case something does not work.
     * @return A boolean indicating whether adding the player worked or not.
     *
     * This function tries to add a player to a game. To do so, it checks several things including if the maximum number
     * of players is already reached and if the game has already started.
     */
    bool add_player(player* player, std::string& err);

    /**
     * @brief Starts the game.
     * @param err The error message updated in case something does not work.
     * @return A boolean indicating whether the game could be started or not.
     *
     * This function tries to start the game. To do so, it checks if the round is already started and if enough players
     * have joint the game.
     */
    bool start_game(std::string& err);

    /**
     * @brief Finishes a game.
     * @param err The error message updated in case something does not work.
     * @return A boolean indicating whether finishing the game worked or not.
     *
     * This function finishes the game by setting _is_finished to true.
     */
    bool finish_game(std::string& err) const;

    /**
     * @brief Sets up a round.
     * @param err The error message updated in case something does not work.
     *
     * This function sets up a new round. To do so, it changes the phase to estimation phase, sets the trick estimate sum
     * to zero, and updates the starting player indices. It also sets up the deck (which draws cards into players' hands),
     * determines the trump, and sets up the trick.
     */
    void setup_round(std::string& err);

    /**
     * @brief Wraps up a round.
     * @param err The error message updated in case something does not work.
     *
     * This function wraps up a round by moving the starting player index further to the next player, and by calling
     * the wrap_up_round function of each player (this calculates the scores, see player class for more information).
     * If the game is finished after the current round, the function also call finish_game to finish the game.
     */
    void wrap_up_round(std::string& err);

    /**
     * @brief Sets the trick estimates for a player.
     * @param player The player that is estimating tricks.
     * @param err The error message updated in case something does not work.
     * @param trick_estimate The trick estimate.
     * @return A boolean indicating whether the trick estimate was valid or not.
     *
     * This function is called whenever a player estimates their tricks (so predicts how many tricks they will win in
     * the next round). To do so, the functions checks that the trick estimate is neither too low (smaller than zero),
     * nor too high (greater than the round number), and for the last player, that the trick estimate sum of all players
     * is not equal to the round number. If all of these checks pass, the players trick estimate is updated. Afterward,
     * the game proceeds by calling update_current_player (see below).
     */
    bool estimate_tricks(const player *player, std::string &err, int trick_estimate);

    /**
     * @brief Plays a card.
     * @param player The player that plays the card.
     * @param card_id The id of the card that is played.
     * @param err The error message updated in case something does not work.
     * @return A boolean indicating whether the card could be played or not.
     *
     * This function is called whenever a player tries to play a card. If the card is in the player's hand, and it can
     * be played (see can_be_played method of game_state), the card is added to the trick and removed from the player's
     * hand. Afterward, the game proceeds by calling update_current_player (see below).
     */
    bool play_card(player* player, const std::string& card_id, std::string& err);

    /**
     * @brief Updates the current player after estimating tricks or playing cards.
     * @param err The error message updated in case something does not work.
     * @return A boolean indicating whether the current player could be updated or not.
     *
     * This function is called whenever a trick estimation or playing of a card was successful, and it progresses the
     * game after these actions. First, the function updates the current player to be the next player. If the updated
     * current player still has to estimate their tricks or play their card in the current round or trick, the game just
     * proceeds. However, if the updated current player is the same player that started the current trick or round,
     * the current trick or round is wrapped up and a new one is set up.
     */
    bool update_current_player(std::string& err);
#endif

// serializable interface
    /**
     * @brief Serializes a game_state object into a json object.
     * @param json The json object for serializing the game_state.
     * @param allocator The json allocator for serializing the game_state.
     */
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

    /**
     * @brief Deserializes a game_state object from a json object.
     * @param json The json object containing the game_state information.
     * @return A pointer to a new game_state object created from the given json object.
     */
    static game_state* from_json(const rapidjson::Value& json);

};

#endif //WIZARD_GAME_STATE_H
