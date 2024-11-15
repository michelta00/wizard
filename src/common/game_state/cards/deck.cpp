//
// Created by Michel Tarnow on 15.11.24.
//

#include "deck.h"
#include <random>
#include <algorithm>
#include "../../serialization/vector_utils.h"
#include "../../exceptions/WizardException.h"


// deserialization constructor
deck::deck(std::string id, std::vector<card*> &all_cards, std::vector<card*> &remaining_cards, card* last_card)
        : unique_serializable(id),
          _all_cards(all_cards),
          _remaining_cards(remaining_cards),
          _last_card(last_card)
{ }

// from_diff constructor
deck::deck(std::string id) : unique_serializable(id) { }


// constructors / destructor
deck::deck(std::vector<card*> &cards)
        : unique_serializable(), _all_cards(cards)
{ }

deck::deck() : unique_serializable()
{
    // this is the main constructor used by the game state to create an object of class deck
    // it initializes the deck object with all wizard cards in the _all_cards vector
    _all_cards = std::vector<card*>();

    // create 52 regular cards with values 1-13 for each of the 4 colors
    for (unsigned int i = 1; i <= 13; ++i)
    {
        for (unsigned int j = 1; j <= 4; ++j)
        {
            _all_cards.push_back(new card(std::make_pair(i, j)));
        }
    }

    // create 4 wizards (value 14 and color 0)
    for (unsigned int i = 0; i < 4; ++i)
    {
        _all_cards.push_back(new card(std::make_pair(14, 0)));
    }

    // create 4 jesters (value 0 and color 0)
    for (unsigned int i = 0; i < 4; ++i)
    {
        _all_cards.push_back(new card(std::make_pair(0, 0)));
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
    for (card* & _card : _remaining_cards) {
        delete _card;
    }
    _remaining_cards.clear();
    // delete _last_card pointer
    delete _last_card;
    _last_card = nullptr;
}

// accessors
bool deck::is_empty() const noexcept
{
    return _remaining_cards.empty();
}

int deck::get_number_of_remaining_cards() const noexcept
{
    return _remaining_cards.size();
}


#ifdef WIZARD_SERVER
// state update functions
void deck::setup_round(std::string& err)
{
    _remaining_cards = _all_cards;
    _last_card = nullptr;
}

bool deck::draw_cards(player* player, const int round_number, std::string& err)
{

    // draw randomly n indices from _remaining_cards by shuffling a vector of indices and then choosing the first
    // n ones

    // check if round_number is within the range
    if (round_number > _remaining_cards.size()) {
        // should never be true since cards are only drawn by the server according to round number
        // in the last round, exactly all possible cards are drawn, and before that less
        err = "Cannot draw more cards than remaining.";
        return false;
    }

    // create a vector of indices with size of _remaining_cards
    std::vector<int> indices(_remaining_cards.size());
    std::iota(indices.begin(), indices.end(), 0); // fill with 0, 1, ..., vec.size() - 1

    // random device and generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::ranges::shuffle(indices, gen); // shuffle the indices

    // take the first n indices
    std::vector<int> selected_indices(indices.begin(), indices.begin() + round_number);


    // place cards at selected indices into the player's hand

    // first check if all cards will be dealt -> in that case remember the last dealt card
    if (_remaining_cards.size() - selected_indices.size() == 0)
    {
        _last_card = _remaining_cards[selected_indices[selected_indices.size() - 1]];
    }

    // get the player's hand to place the cards in
    hand* hand = player->get_hand();

    // loop over selected indices and add the corresponding cards to the player's hand
    bool control = true;    // checks if all cards were successfully placed into the player's hand
    for (int & indice : selected_indices)
    {
        if (!hand->add_card(_remaining_cards[indice], err))
        {
            // card could not be placed into hand
            control = false;
            err = "Could not place a new card into the player's hand";
        }
        else
        {
            // card could be placed into hand and should be removed from _remaining_cards
            _remaining_cards.erase(_remaining_cards.begin() + indice);
        }
    }

    return control;
}

card* deck::draw_trump() const
{
    // if all cards has been dealt, the last dealt card is chosen as trump
    if (_remaining_cards.empty()) return _last_card;

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
void deck::write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override
{
    unique_serializable::write_into_json(json, allocator);

    json.AddMember("all_cards", vector_utils::serialize_vector(_all_cards, allocator), allocator);

    json.AddMember("remaining_cards", vector_utils::serialize_vector(_remaining_cards, allocator), allocator);

    rapidjson::Value last_card_val(rapidjson::kObjectType);
    _last_card->write_into_json(last_card_val, allocator);
    json.AddMember("last_card", last_card_val, allocator);
}

deck* deck::from_json(const rapidjson::Value& json)
{
    if (json.HasMember("id") &&
        json.HasMember("all_cards") &&
        json.HasMember("remaining_cards") &&
        json.HasMember("last_card"))
    {
        // deserialize all_cards
        std::vector<card*> des_all_cards = std::vector<card*>();
        for (auto &serialized_card : json["all_cards"].GetArray())
        {
            des_all_cards.push_back(card::from_json(serialized_card.GetObject()));
        }

        // deserialize remaining_cards
        std::vector<card*> des_remaining_cards = std::vector<card*>();
        for (auto &serialized_card : json["remaining_cards"].GetArray())
        {
            des_remaining_cards.push_back(card::from_json(serialized_card.GetObject()));
        }

        // deserialize last_card
        card* des_last_card = card::from_json(json["last_card"].GetObject());

        return new deck(json["id"].GetString(), des_all_cards, des_remaining_cards, des_last_card);
    }

    throw WizardException("Could not parse draw_pile from json. 'id' or 'cards' were missing.");
}





