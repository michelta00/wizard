//
// Created by Aidan Meara on 2024.11.08.
//

#ifndef WIZARD_TRICK_H
#define WIZARD_TRICK_H

#include <vector>
#include "card.h"
#include "../player/player.h"
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

/**
 * @class trick
 * @brief Represents the trick in the game.
 *
 * This class encapsulates all information about a trick,
 * including the trick color (suit of the trick), the trump color, and the cards played in the trick.
 *
 * Every round consists of round-number-many tricks. In each trick, each player plays exactly one card. The first
 * played color determines the trick color, and other players must play cards of that color if they hold any (exception:
 * wizards and jesters can always be played). Each trick has one winner, which is determined by the player who played
 * the first wizard, or by the player who played the highest trump color card, or by the player who played the highest
 * trick color card (in that order).
 */
class trick : public unique_serializable {
private:

    serializable_value<int>* _trick_color;              ///< The tricks color (suit).
    serializable_value<int>* _trump_color;              ///< The rounds trump color.
    std::vector<std::pair<card*, player*>> _cards;      ///< The cards played during the current trick and the players who played them.

public:
// constructor and destructors
    /**
     * @brief Constructs a new trick object.
     */
    trick();

    /**
     * @brief Constructs a new trick object as a deep copy of another trick object.
     * @param other The trick it is copied from.
     */
    trick(const trick &other);

    /**
     * @brief Constructs a new trick object (from_diff).
     * @param id The trick's id.
     */
    explicit trick(const std::string& id);

    /**
     * @brief Constructs a new trick object during deserialization.
     * @param id The trick's id.
     * @param cards The trick's cards.
     * @param trick_color The trick color.
     * @param trump_color The trump color.
     */
    trick(const std::string& id,
          const std::vector<std::pair<card*, player*>> &cards,
          serializable_value<int>* trick_color,
          serializable_value<int>* trump_color);

    /**
     * @brief Constructs a new trick object.
     * @param trump The trump color.
     */
    explicit trick(int trump);

    /**
     * @brief Destructs a trick object.
     */
    ~trick() override;

// accessors
    /**
     * @brief Gets the trick color.
     * @return The trick color.
     */
    [[nodiscard]] int get_trick_color() const;

    /**
     * @brief Gets the played cards and players who played them as pairs.
     * @return Pairs of played cards and players who played them.
     */
    [[nodiscard]] std::vector<std::pair<card*, player*>> get_cards_and_players() const;

    /**
     * @brief Gets the trump color.
     * @return The trump color.
     */
    [[nodiscard]] int get_trump_color() const;

#ifdef WIZARD_SERVER
// state update functions
    /**
     * @brief Sets up a trick.
     * @param trump The trump color.
     * @param err The error message updated in case something does not work.
     *
     * To set up a trick, the played cards are removed from the trick (not the actual card objects, just the pointers),
     * the trick color is set to no color (0), and the trump color is set to the input value.
     */
    void set_up_round(int trump, std::string& err);

    /**
     * @brief Wraps up a trick at the end of a trick or round.
     * @param err The error message updated in case something does not work.
     * @return The player who won the trick.
     *
     * To wrap up the trick, this function determines the winner of the trick. This is done considering the cards
     * that have been played during the trick, and considering the trump color. The round is won by the player who
     * played the first wizard, or the first jester in case all players play jesters. If neither wizards nor 4 jesters
     * are played, the player who played the highest card of the trump color wins the trick. If no such card is played,
     * the player with the highest trick color (suit) card wins.
     */
    player* wrap_up_trick(std::string& err) const;

    /**
     * @brief Adds a card to the trick.
     * @param played_card The card that is played.
     * @param current_player The player who played the card.
     * @param err The error message updated in case something does not work.
     * @return A boolean indicating whether the card could be added to the trick or not.
     *
     * To keep track of who played which card, this function adds both the card that has been played and the player
     * who played the card as a pair to the trick. In case there is no trick color yet, the function also determines
     * the trick color.
     */
    bool add_card(card* played_card, player* current_player, std::string& err);

// server setter
    /**
     * @brief Sets the trick color.
     * @param color The trick color to be set.
     */
    void set_trick_color(int color) const;
#endif

// serializable interface
    /**
     * @brief Serializes a trick object into a json object.
     * @param json The json object for serializing the trick.
     * @param allocator The json allocator for serializing the trick.
     */
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

    /**
     * @brief Deserializes a trick object from a json object.
     * @param json The json object containing the trick information.
     * @return A pointer to a new trick object created from the given json object.
     */
    static trick* from_json(const rapidjson::Value& json);

};

#endif //WIZARD_TRICK_H
