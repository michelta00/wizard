//
// Created by martinalavanya on 4.12.24
//

#include "gtest/gtest.h"
#include "../src/common/exceptions/WizardException.h"
#include "../src/common/game_state/player/hand.h"
#include "../src/common/serialization/json_utils.h"


class HandTest : public ::testing::Test {

protected:
    std::vector<card*> cards;
    hand* test_hand = nullptr;
    std::string err = "";
//leave all destruction up to hand class or individual tests
};

//check constructor
TEST_F(HandTest, CreateAndAddCard) {
    // Create cards
    card* card1 = new card(14, 0);
    card* card2 = new card(2, 2);
    card* card3 = new card(4, 3);
    card* card4 = new card(11, 1);
    // Add cards to the vector
    cards = {card1, card2, card3, card4};

    // Pass the vector to the hand constructor
    test_hand = new hand("test_hand_id", cards);

    // Verify hand properties
    EXPECT_EQ(test_hand->get_cards().size(), cards.size());
    EXPECT_EQ(test_hand->get_id(), "test_hand_id");

    // Add a new card
    card* card5 = new card(5, 2);
    test_hand->add_card(card5, err);
    EXPECT_EQ(test_hand->get_nof_cards(), 5);

    // destructor of hand class deletes allocated memory
}

// remove card that is in hand
TEST_F(HandTest, RemoveExistingCard) {
    card* card1 = new card(14, 0);
    card* card2 = new card(2, 2);
    card* card3 = new card(4, 3);
    card* card4 = new card(11, 1);
    cards = {card1, card2, card3, card4};
    test_hand = new hand("test_hand_id", cards);

    //remove card3
    test_hand->remove_card(card3->get_id(), err);
    cards.erase(cards.begin() + 2);
    EXPECT_EQ(test_hand->get_cards(), cards);

}

// remove card that is not in hand
TEST_F(HandTest, RemoveNonExistingCard) {
    card* card1 = new card(14, 0);
    card* card2 = new card(2, 2);
    card* card3 = new card(4, 3);
    card* card4 = new card(11, 1);
    cards = {card1, card2, card3};
    test_hand = new hand("test_hand_id");
    // add cards 1-3
    test_hand->add_card(card1, err);
    test_hand->add_card(card2, err);
    test_hand->add_card(card3, err);
    EXPECT_EQ(err, "");
    //remove card4
    EXPECT_FALSE(test_hand->remove_card(card4->get_id(), err));
    EXPECT_EQ(err, "Could not play card, as the requested card was not on the player's hand.");

}

// try get existing and nonexisting card
TEST_F(HandTest, TryGetCards) {
    card* card1 = new card(14, 0);
    card* card2 = new card(2, 2);
    card* card3 = new card(4, 3);
    card* card4 = new card(11, 1);
    test_hand = new hand("test_hand_id");
    // add cards 1-3
    test_hand->add_card(card1, err);
    test_hand->add_card(card2, err);
    test_hand->add_card(card3, err);
    EXPECT_EQ(err, "");
    //get existing card
    EXPECT_TRUE(test_hand->try_get_card(card3->get_id(), card3)); //card 3 pointer passed by reference
    EXPECT_EQ(card3->get_color(), 3);

    //get non-existing card
    EXPECT_FALSE(test_hand->try_get_card(card4->get_id(), card4));

}




// Serialization and subsequent deserialization must yield the same object
TEST_F(HandTest, SerializationEquality) {
    card* card1 = new card(14, 0);
    card* card2 = new card(2, 2);
    card* card3 = new card(4, 3);
    card* card4 = new card(11, 1);
    test_hand = new hand("test_hand_id");
    test_hand->add_card(card1, err);
    test_hand->add_card(card2, err);
    test_hand->add_card(card3, err);
    test_hand->add_card(card4, err);
    rapidjson::Document* json_send = test_hand->to_json();
    std::string message = json_utils::to_string(json_send);
    delete json_send;

    rapidjson::Document json_received = rapidjson::Document(rapidjson::kObjectType);
    json_received.Parse(message.c_str());
    hand* hand_received = hand::from_json(json_received);

    EXPECT_EQ(test_hand->get_id(), hand_received->get_id());
    EXPECT_EQ(test_hand->get_nof_cards(), hand_received->get_nof_cards());
    //test first and last card for equality
    EXPECT_EQ(test_hand->get_cards()[0]->get_color(), hand_received->get_cards()[0]->get_color());
    EXPECT_EQ(test_hand->get_cards()[0]->get_value(), hand_received->get_cards()[0]->get_value());
    EXPECT_EQ(test_hand->get_cards()[test_hand->get_nof_cards() - 1]->get_color(), hand_received->get_cards()[hand_received->get_nof_cards() - 1]->get_color());
    EXPECT_EQ(test_hand->get_cards()[test_hand->get_nof_cards() - 1]->get_value(), hand_received->get_cards()[hand_received->get_nof_cards() - 1]->get_value());
    delete hand_received;
}

// Deserializing an invalid string must throw a WizardException
TEST_F(HandTest, SerializationException) {
    rapidjson::Document json = rapidjson::Document(rapidjson::kObjectType);
    json.Parse("not json");
    EXPECT_THROW(hand::from_json(json), WizardException);
}

