//
// Created by Michel Tarnow on 15.11.24.
//

#ifndef DECK_H
#define DECK_H

#include "card.h"
#include "../player/player.h"
#include <vector>
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

/**
 * @class deck
 * @brief Represents the card deck in the game.
 *
 * This class encapsulates all information about a card deck,
 * including all cards of the game and the remaining cards not dealt yet.
 *
 * In the whole game, there is only one object of each possible cards. The _all_cards member of the deck class
 * holds pointers to these cards that are not changed once created and are used to update the _remaining_cards
 * member of the deck class when a new round is set up. The _remaining_cards member helps to keep track of which
 * cards are already dealt and which can still be dealt and used to draw a trump.
 */
class deck : public unique_serializable
{
private:

    std::vector<card*> _all_cards;          ///< All cards of the game.
    std::vector<card*> _remaining_cards;    ///< Remaining cards not dealt yet.

    /**
     * @brief Constructs a new deck object during deserialization.
     * @param id The deck's id.
     * @param all_cards All cards of the game.
     * @param remaining_cards Remaining cards of the deck.
     */
    deck(const std::string& id, const std::vector<card*> &all_cards, const std::vector<card*> &remaining_cards);

    /**
     * @brief Constructs a new deck object (from_diff).
     * @param id The deck's id.
     */
    explicit deck(const std::string& id);

public:
// constructors & destructor
    /**
     * @brief Constructs a new deck object.
     *
     * This is the main constructor of the deck class used by the game_state to create a deck for a game.
     * When this constructor is called, all possible cards in Wizard are created and added to the deck. This is the
     * only time actual card objects are created, every card only exists exactly once. In all other instances where
     * cards are removed or added somewhere (e.g., to a player's hand), actually only pointers are added or removed.
     */
    deck();

    /**
     * @brief Constructs a new deck object.
     * @param cards All cards of the game.
     */
    explicit deck(const std::vector<card*>& cards);

    /**
     * @brief Destructs a deck object.
     *
     * As explained for the deck's main constructor, the deck creates the actual card objects, and thus this is the
     * only place where the actual cards are deleted, and not only pointers.
     */
    ~deck() override;

// accessors
    /**
     * @brief Checks if _remaining_cards is empty, i.e., if all cards are dealt.
     * @return A boolean indicating whether all cards are dealt or not.
     *
     * All cards are dealt only in the last round of the game.
     */
    [[nodiscard]] bool is_empty() const noexcept;

    /**
     * @brief Gets the number of remaining cards, i.e., the number of cards not dealt yet.
     * @return The number of remaining cards.
     */
    [[nodiscard]] unsigned int get_number_of_remaining_cards() const noexcept;

#ifdef WIZARD_SERVER
// state update functions
    /**
     * @brief Sets up the deck for a new round.
     *
     * Setting up a deck for a new round is done by refreshing the remaining cards from all cards.
     */
    void setup_round();

    /**
     * @brief Draws a number of random cards based on the current round number and places them into a player's hand.
     * @param player The player to which the cards are added.
     * @param round_number The current round number determining how many cards are drawn.
     * @param err The error message updated in case something does not work.
     * @return A boolean indicated if drawing the cards worked or not.
     *
     * This function is used to place cards into the players' hands at the start of each round. The cards are randomly
     * drawn from the remaining cards, added to a player's hand, and then are removed from the remaining cards. The
     * number of cards to be drawn should be the current round number (if round number 0-indexed, the round_number
     * parameter must be the round number + 1).
     */
    bool draw_cards(const player* player, int round_number, std::string& err);

    /**
     * @brief Draws a random trump card from the remaining cards.
     * @return The card used as trump.
     *
     * This function randomly draws one card from the remaining cards, which is then used as the trump for a round.
     * In the last round, there are no more remaining cards after dealing. In this case, a nullptr is returned. The
     * logic to handle this case further is implemented in the game_state class.
     */
    [[nodiscard]] card* draw_trump() const;
#endif

// serialization

    /**
     * @brief Serializes a deck object into a json object.
     * @param json The json object for serializing the deck.
     * @param allocator The json allocator for serializing the deck.
     */
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

    /**
     * @brief Deserializes a deck object from a json object.
     * @param json The json object containing the deck information.
     * @return A pointer to a new deck object created from the given json object.
     */
    static deck* from_json(const rapidjson::Value& json);

};

#endif //DECK_H
