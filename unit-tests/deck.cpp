//
// Created by Aidan Meara on 23.11.24.
//

#include "gtest/gtest.h"
#include "../src/common/game_state/cards/deck.h"
#include "../src/common/serialization/json_utils.h"
#include "../src/common/exceptions/WizardException.h"

// new deck must hold 60 cards
TEST(DeckTest, Creation) {
    deck mydeck;
    ASSERT_EQ(mydeck.get_number_of_remaining_cards(), 60);
    EXPECT_TRUE(mydeck.is_empty() == false);
}

// can draw trump from new deck
TEST(DeckTest, TrumpDraw) {
    deck mydeck;
    card* trump = mydeck.draw_trump();
    EXPECT_TRUE(trump);

}

// draw cards to player, make sure proper number of cards
TEST(DeckTest, CardDraw) {
    deck mydeck;
    player* player1 = new player("player1");
    std::string err;
    mydeck.draw_cards(player1, 5, err);

    // check card numbers
    ASSERT_EQ(mydeck.get_number_of_remaining_cards(), 55);
    EXPECT_TRUE(mydeck.is_empty() == false);

    // check hand
    ASSERT_EQ(player1->get_hand()->get_nof_cards(), 5);
}

// draw last few cards
TEST(DeckTest, DrawFinalCards) {
    deck mydeck;
    player* player1 = new player("player1");
    std::string err;
    mydeck.draw_cards(player1, 60, err);

    // check card numbers
    ASSERT_EQ(mydeck.get_number_of_remaining_cards(), 0);
    ASSERT_EQ(err, "");
    EXPECT_TRUE(mydeck.is_empty());

    // check hand
    ASSERT_EQ(player1->get_hand()->get_nof_cards(), 60);

    // draw trump
    ASSERT_EQ(mydeck.draw_trump(), nullptr);

}

// Drawing too many cards must fail
TEST(DeckTest, OverDraw) {
    deck mydeck;
    player* player1 = new player("player1");
    std::string err;

    EXPECT_FALSE(mydeck.draw_cards(player1, 65, err));

}

// check round set up
TEST(DeckTest, Setup) {
    deck mydeck;
    player* player1 = new player("player1");
    std::string err;
    mydeck.draw_cards(player1, 60, err);

    mydeck.setup_round();

    ASSERT_EQ(mydeck.get_number_of_remaining_cards(), 60);


}

// Serialization and subsequent deserialization must yield the same object
TEST(CardTest, SerializationEquality) {
    deck deck_send;
    rapidjson::Document* json_send = deck_send.to_json();
    std::string message = json_utils::to_string(json_send);
    delete json_send;

    rapidjson::Document json_recv = rapidjson::Document(rapidjson::kObjectType);
    json_recv.Parse(message.c_str());
    deck* deck_receive = deck::from_json(json_recv);

    EXPECT_EQ(deck_send.get_id(), deck_receive->get_id());
    EXPECT_EQ(deck_send.get_number_of_remaining_cards(), deck_receive->get_number_of_remaining_cards());
    delete deck_receive;
}

// Deserializing an invalid string must throw a WizardException
TEST(CardTest, SerializationException) {
    rapidjson::Document json = rapidjson::Document(rapidjson::kObjectType);
    json.Parse("not json");
    EXPECT_THROW(deck::from_json(json), WizardException);
}

