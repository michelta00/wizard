//
// Created by Michel Tarnow on 15.11.24.
//

#include "deck.h"
#include <random>
#include "../../serialization/vector_utils.h"
#include "../../exceptions/WizardException.h"

// deserialization constructor
deck::deck(const std::string& id, const std::vector<card*> &all_cards, const std::vector<card*> &remaining_cards)
        : unique_serializable(id),
          _all_cards(all_cards),
          _remaining_cards(remaining_cards)
{ }

// from_diff constructor
deck::deck(const std::string& id) : unique_serializable(id) { }


// constructors / destructor
deck::deck(const std::vector<card*> &cards)
        : unique_serializable(), _all_cards(cards)
{ }

deck::deck() : unique_serializable()
{
    // this is the main constructor used by the game state to create an object of class deck
    // it initializes the deck object with all wizard cards in the _all_cards vector
    _all_cards = std::vector<card*>();

    // create 52 regular cards with values 1-13 for each of the 4 colors
    for (int i = 1; i <= 13; ++i)
    {
        for (int j = 1; j <= 4; ++j)
        {
            _all_cards.push_back(new card(i, j));
        }
    }

    // create 4 wizards (value 14 and color 0)
    for (int i = 0; i < 4; ++i)
    {
        _all_cards.push_back(new card(14, 0));
    }

    // create 4 jesters (value 0 and color 0)
    for (int i = 0; i < 4; ++i)
    {
        _all_cards.push_back(new card(0, 0));
    }

    _remaining_cards = _all_cards;
}

deck::~deck() {
    // delete _all_cards vector
    for (card* & _card : _all_cards) {
        delete _card;
    }
    _all_cards.clear();
    // delete _remaining_cards_vector
    _remaining_cards.clear();
}

// accessors
bool deck::is_empty() const noexcept
{
    return _remaining_cards.empty();
}

unsigned int deck::get_number_of_remaining_cards() const noexcept
{
    return _remaining_cards.size();
}


#ifdef WIZARD_SERVER
// state update functions
void deck::setup_round()
{
    _remaining_cards = _all_cards;
}

bool deck::draw_cards(const player* player, const int round_number, std::string& err)
{
    // check if round_number is within the range
    if (round_number > _remaining_cards.size()) {
        // should never be true since cards are only drawn by the server according to round number
        // in the last round, exactly all possible cards are drawn, and before that less
        err = "Cannot draw more cards than remaining.";
        return false;
    }
    if (round_number < 1) {
        err = "Invalid number of rounds to draw.";
        return false;
    }
    // random device and generator
    std::random_device rd;
    std::mt19937 gen(rd());
    // get player hand
    hand* hand = player->get_hand();
    for (int i = 0; i < round_number; ++i) {
        // randomly select an index from remaining_cards
        std::uniform_int_distribution<> dist(0, _remaining_cards.size() - 1);
        const int random_index = dist(gen);
        // try to add card to player's hand
        if (!hand->add_card(_remaining_cards[random_index], err))
        {
            // card could not be placed into hand
            err = "Could not place a new card into the player's hand";
            return false;
        }
        // remove card from remaining_cards
        _remaining_cards.erase(_remaining_cards.begin() + random_index);
    }


    return true;
}

card* deck::draw_trump() const
{
    // if all cards has been dealt return nullptr, this should never be the case
    if (_remaining_cards.empty()) return nullptr;

    // if there are still cards left, one card is randomly drawn as trump from _remaining_cards
    // random device and generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // distribution to select a random index within the range of _remaining_cards
    std::uniform_int_distribution<> dis(0, _remaining_cards.size() - 1);

    // draw a single random index and return that card as trump
    const int random_index = dis(gen);
    return _remaining_cards[random_index];
}
#endif


// serialization
void deck::write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const
{
    unique_serializable::write_into_json(json, allocator);

    json.AddMember("all_cards", vector_utils::serialize_vector(_all_cards, allocator), allocator);

    json.AddMember("remaining_cards", vector_utils::serialize_vector(_remaining_cards, allocator), allocator);
}

deck* deck::from_json(const rapidjson::Value& json)
{
    if (json.HasMember("id") &&
        json.HasMember("all_cards") &&
        json.HasMember("remaining_cards"))
    {
        // deserialize all_cards
        auto des_all_cards = std::vector<card*>();
        for (auto &serialized_card : json["all_cards"].GetArray())
        {
            des_all_cards.push_back(card::from_json(serialized_card.GetObject()));
        }

        // deserialize remaining_cards
        auto des_remaining_cards = std::vector<card*>();
        for (auto &serialized_card : json["remaining_cards"].GetArray())
        {
            des_remaining_cards.push_back(card::from_json(serialized_card.GetObject()));
        }
        return new deck(json["id"].GetString(), des_all_cards, des_remaining_cards);
    }
    throw WizardException("Could not parse draw_pile from json. 'id' or 'cards' were missing.");
}
