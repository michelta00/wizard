#ifndef WIZARD_CARD_H
#define WIZARD_CARD_H

#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

/**
 * @class card
 * @brief Represents a card in the game.
 *
 * This class encapsulates all information about a card,
 * including its value and color.
 */
class card : public unique_serializable {
private:

    serializable_value<int>* _value{}; ///< The card's value (value between 1 and 13, or jester (0) / wizard (14)).
    serializable_value<int>* _color{}; ///< The card's color ( 0 : no color, 1: yellow, 2: red, 3: green, 4: yellow)

    /**
     * @brief Constructs a new card object (from_diff).
     * @param id The card's id.
     */
    explicit card(const std::string& id);

    /**
     * @brief Constructs a new card object during deserialization.
     * @param id The card's id.
     * @param value The card's value.
     * @param color The card's color.
     */
    card(const std::string& id, serializable_value<int>* value, serializable_value<int>* color);

public:

// constructor and destructor
    /**
     * @brief Constructs a new card object (public constructor).
     * @param value The card's value.
     * @param color The card's color.
     */
    card(int value, int color);

    /**
     * @brief Destructs a card object.
     */
    ~card() override;

// accessors
    /**
     * @brief Gets the card's value.
     * @return The card's value.
     */
    [[nodiscard]] int get_value() const noexcept;

    /**
     * @brief Gets the card's color.
     * @return The card's color.
     */
    [[nodiscard]] int get_color() const noexcept;

// serialization interface
    /**
     * @brief Serializes a card object into a json object.
     * @param json The json object for serializing the card.
     * @param allocator The json allocator for serializing the card.
     */
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

    /**
     * @brief Deserializes a card object from a json object.
     * @param json The json object containing the card information.
     * @return A pointer to a new card object created from the given json object.
     */
    static card* from_json(const rapidjson::Value& json);

};

#endif //WIZARD_CARD_H
