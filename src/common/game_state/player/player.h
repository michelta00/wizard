#ifndef WIZARD_PLAYER_H
#define WIZARD_PLAYER_H

#include <string>
#include "hand.h"
#include "../../serialization/uuid_generator.h"
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"

/**
 * @class player
 * @brief Represents a player in the game.
 *
 * This class encapsulates all information about a player,
 * including the player's name, scores, hand, and the number of tricks they predicted and won.
 */
class player : public unique_serializable {
private:

    serializable_value<std::string>* _player_name;  ///< The player's name chosen by the player.
    serializable_value<int>* _nof_tricks;           ///< The number of tricks won in the current round.
    serializable_value<int>* _nof_predicted;        ///< The number of predicted tricks in the current round.
    std::vector<serializable_value<int>*> _scores;  ///< The scores of the player (total game score, current and past ones).
    serializable_value<bool>* _has_left_game;       ///< Boolean whether player has left the game.
    hand* _hand;                                    ///< The player's hand holding the player's cards.

#ifdef WIZARD_SERVER
    std::string _game_id;                           ///< The ID of the game the player has joint.
#endif

    /**
     * @brief Constructs a new player object during deserialization.
     * @param id The player's is.
     * @param name The player's name.
     * @param nof_tricks The number of tricks won by the player in the current round.
     * @param nof_predicted The number of tricks predicted to be won by the player in the current round.
     * @param scores The player's scores.
     * @param has_left_game Boolean whether player has left the game.
     * @param hand The player's hand.
     */
    player(const std::string& id,
           serializable_value<std::string>* name,
           serializable_value<int>* nof_tricks,
           serializable_value<int>* nof_predicted,
           const std::vector<serializable_value<int>*>& scores,
           serializable_value<bool>* has_left_game,
           hand* hand);

public:

// constructors
    /**
     * @brief Constructs a new player object (used by client).
     * @param name The player's name.
     */
    explicit player(const std::string& name);

    /**
     * @brief Destructs a player object.
     */
    ~player() override;

#ifdef WIZARD_SERVER
    /**
     * @brief Constructs a new player object (used by the server)
     * @param id The player's id.
     * @param name The player's name.
     */
    player(const std::string& id, const std::string& name);

    /**
     * @brief Gets the game id of the game the player has joint.
     * @return The game id of the game the player has joint.
     */
    std::string get_game_id();

    /**
     * @brief Sets the game id of the player.
     * @param game_id The game id of the game the player has joint.
     */
    void set_game_id(const std::string& game_id);
#endif

// accessors
    /**
     * @brief Gets the scores of the player.
     * @return The scores of the player as a vector.
     */
    [[nodiscard]] std::vector<serializable_value<int>*> get_scores() const noexcept;

    /**
     * @brief Sets the player's scores to the given scores.
     * @param score The new scores of the player.
     */
    void set_scores(int score);

    /**
     * @brief Gets the number of tricks won by the player in the current round.
     * @return The number of tricks won by the player in the current round.
     */
    [[nodiscard]] int get_nof_tricks() const noexcept;

    /**
     * @brief Sets the player's number of won tricks to the given input.
     * @param nof_tricks The new number of tricks won by the player in the current round.
     *
     * This function is used to update the player's won number of tricks in case a player wins a trick.
     */
    void set_nof_tricks(int nof_tricks) const;

    /**
     * @brief State whether player has left the game.
     * @return Boolean stating whether player has left the game.
     */
    [[nodiscard]] bool has_left_game() const;

    /**
     * @brief Sets the players game to left when he leaves the game
     * @param has_left_game Boolean whether player has left game
     */
    void set_has_left_game(bool has_left_game);

    /**
     * @brief Gets the number of tricks predicted to be won by the player in the current round.
     * @return The number of tricks predicted to be won by the player in the current round.
     */
    [[nodiscard]] int get_nof_predicted() const noexcept;

    /**
     * @brief Sets the player's number of predicted tricks to the given input.
     * @param nof_predicted The new number of tricks predicted by the player in the current round.
     *
     * This function is used to update the player's predicted number of tricks during the estimation
     * phase of the game.
     */
    void set_nof_predicted(int nof_predicted) const;

    /**
     * @brief Gets the number of cards in the player's hand.
     * @return The number of cards in the player's hand.
     */
    [[nodiscard]] unsigned int get_nof_cards() const noexcept;


    /**
     * @brief Gets the player's hand.
     * @return The player's hand.
     */
    [[nodiscard]] hand* get_hand() const noexcept;

    /**
     * @brief Gets the player's name.
     * @return The player's name.
     */
    [[nodiscard]] std::string get_player_name() const noexcept;

    /**
     * @brief Sets the player's name to the given input.
     * @param new_name The new player name.
     *
     * This function is used to change the player's name in case duplicate names appear within one game.
     */

    void set_player_name(const std::string& new_name);

#ifdef WIZARD_SERVER
// state update functions
    /**
     * @brief Adds a card to the player's hand.
     * @param card The card to be added to the player's hand.
     * @param err The error message updated in case something does not work.
     * @return A boolean indicating whether adding the card worked or not.
     */
    bool add_card(card* card, std::string& err) const;

    /**
     * @brief Sets up the player for the next round.
     *
     * Sets the number of predicted tricks to -1 to show that no predictions have been made yet by the player
     * and sets the number of won tricks to zero. This sets up the player for the next round.
     */
    void setup_round() const;

    /**
     * @brief Calculates the new score of the player.
     *
     * Wraps up the player by calculating their score for the round that is wrapped up, based on the predicted
     * and won tricks.
     */
    void wrap_up_round();
#endif

// serialization interface
    /**
     * @brief Serializes a player object into a json object.
     * @param json The json object for serializing the player.
     * @param allocator The json allocator for serializing the player.
     */
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

    /**
     * @brief Deserializes a player object from a json object.
     * @param json The json object containing the player information.
     * @return A pointer to a new player object created from the given json object.
     */
    static player* from_json(const rapidjson::Value& json);
};

#endif //WIZARD_PLAYER_H
