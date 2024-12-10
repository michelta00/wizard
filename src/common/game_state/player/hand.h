#ifndef WIZARD_HAND_H
#define WIZARD_HAND_H

#include <vector>
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../cards/card.h"

/**
 * @class hand
 * @brief Represents a player's hand in the game.
 *
 * This class encapsulates all information about a player's hand,
 * including the cards a player holds.
 */
class hand : public unique_serializable {
private:

    std::vector<card*> _cards; ///< The cards a player holds in their hand.

    /**
     * @brief Removes a card from the hand.
     * @param pos An iterator pointing to the position of the card that should be removed.
     * @return A pointer to the removed card.
     */
    card* remove_card(std::vector<card*>::iterator pos);

    /**
     * @brief Removes a card from the hand.
     * @param idx The index of the card that should be removed.
     * @return A pointer to the removed card.
     */
    card* remove_card(int idx);

    /**
     * @brief Removes a card from the hand.
     * @param card A pointer to the card that should be removed.
     * @return A pointer to the removed card.
     */
    card* remove_card(card* card);

public:

// constructors and destructor
    /**
     * @brief Constructs a new hand object.
     */
    hand();

    /**
     * @brief Constructs a new hand object during deserialization.
     * @param id The hand's id.
     * @param cards The hand's cards.
     */
    hand(const std::string& id, const std::vector<card*>& cards);

    /**
     * @brief Constructs a new hand object (from_diff).
     * @param id The hand's id.
     */
    explicit hand(const std::string& id);

    /**
     * @brief Destructs a hand object.
     */
    ~hand() override;

// accessors
    /**
     * @brief Gets the number of cards in the hand.
     * @return The number of cards.
     */
    [[nodiscard]] unsigned int get_nof_cards() const;

    /**
     * @brief Gets the cards in the hand.
     * @return The cards in the hand.
     */
    [[nodiscard]] std::vector<card*> get_cards() const;

    /**
     * @brief Tries to get a specific card from the hand.
     * @param card_id The card's id.
     * @param hand_card Pointer that will point to the card if found.
     * @return A boolean indicating whether getting the card worked or not.
     *
     * This function tries to find a card in the hand given a provided card id. If the card is found, the given
     * pointer reference will be updated to point to that card and true is returned, otherwise false is returned.
     */
    bool try_get_card(const std::string& card_id, card*& hand_card) const;

#ifdef WIZARD_SERVER
// state update functions
    /**
     * @brief Adds a card to the hand.
     * @param card The card to be added.
     * @param err The error message updated in case something does not work.
     * @return A boolean indicating whether adding the card worked or not.
     */
    bool add_card(card* card, std::string& err);

    /**
     * @brief Removes a card from the hand.
     * @param card_id The id of the card that should be removed.
     * @param err The error message updated in case something does not work.
     * @return A boolean indicating whether removing the card worked or not.
     */
    bool remove_card(std::string card_id, std::string& err);
#endif

// serialization interface
    /**
     * @brief Serializes a hand object into a json object.
     * @param json The json object for serializing the hand.
     * @param allocator The json allocator for serializing the hand.
     */
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

    /**
     * @brief Deserializes a hand object from a json object.
     * @param json The json object containing the hand information.
     * @return A pointer to a new hand object created from the given json object.
     */
    static hand* from_json(const rapidjson::Value& json);

};

#endif //WIZARD_HAND_H
