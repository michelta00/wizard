//
// Created by Aidan Meara on 23.11.24.
//

#include "gtest/gtest.h"
#include "../src/common/exceptions/WizardException.h"
#include "../src/common/game_state/cards/card.h"
#include "../src/common/game_state/cards/deck.h"
#include "../src/common/serialization/json_utils.h"

class CardTest : public ::testing::Test {
};

// check proper initialization and getters
TEST_F(CardTest, MakeCardGetMembers) {
    card c_1(0,0);
    card c_2(14,0);
    card c_3(1,2);
    card c_4(4,3);
    card c_5(5,4);
    card c_6(6,3);
    card c_7(7,2);

    EXPECT_EQ(c_1.get_color(), 0);
    EXPECT_EQ(c_2.get_color(), 0);
    EXPECT_EQ(c_3.get_color(), 2);
    EXPECT_EQ(c_4.get_color(), 3);
    EXPECT_EQ(c_5.get_color(), 4);
    EXPECT_EQ(c_6.get_color(), 3);
    EXPECT_EQ(c_7.get_color(), 2);

    EXPECT_EQ(c_1.get_value(), 0);
    EXPECT_EQ(c_2.get_value(), 14);
    EXPECT_EQ(c_3.get_value(), 1);
    EXPECT_EQ(c_4.get_value(), 4);
    EXPECT_EQ(c_5.get_value(), 5);
    EXPECT_EQ(c_6.get_value(), 6);
    EXPECT_EQ(c_7.get_value(), 7);

}

// Serialization and subsequent deserialization must yield the same object
TEST_F(CardTest, SerializationEquality) {
    card card_send(1,1);
    rapidjson::Document* json_send = card_send.to_json();
    std::string message = json_utils::to_string(json_send);
    delete json_send;

    rapidjson::Document json_recv = rapidjson::Document(rapidjson::kObjectType);
    json_recv.Parse(message.c_str());
    card* card_recv = card::from_json(json_recv);

    EXPECT_EQ(card_send.get_id(), card_recv->get_id());
    EXPECT_EQ(card_send.get_value(), card_recv->get_value());
    EXPECT_EQ(card_send.get_color(), card_recv->get_color());
    delete card_recv;


}

// Deserializing an invalid string must throw a WizardException
TEST_F(CardTest, SerializationException) {
    rapidjson::Document json = rapidjson::Document(rapidjson::kObjectType);
    json.Parse("not json");
    EXPECT_THROW(card::from_json(json), WizardException);
}
