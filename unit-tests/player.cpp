
//
// Created by martinalavanya on 23.11.24.
//

#include "gtest/gtest.h"
#include "../src/common/exceptions/WizardException.h"
#include "../src/common/game_state/player/player.h"
#include "../src/common/game_state/player/hand.h"
#include "../src/common/serialization/json_utils.h"


TEST(PlayerTest, CreatePlayer)
{
    std::string name =  "vatkruidvat";
    player test_player(name);

    //check name
    EXPECT_EQ(test_player.get_player_name(), name);

    // check tricks
    int nof_tricks = 2;

    test_player.set_nof_tricks(nof_tricks);
    EXPECT_FALSE(test_player.get_nof_tricks(), 3);//should fail

    //check predicted tricks

    int pred_tricks = 4;

    test_player.set_nof_predicted(pred_tricks);
    EXPECT_EQ(test_player.get_nof_predicted(), pred_tricks);

    //check score
    int score = 7;
    test_player.set_scores(score);
    EXPECT_EQ(test_player.get_scores(), score);


}

TEST(HandTest, RemoveFromEmptyHand)
{
    std::string name =  "vatkruidvat";

    player test_player(name);
    card* test_card = new card(2, 4);

    std::string error = "";
    std::string card_id = test_card->get_id();

    //remove card without adding it
    bool result = test_player.remove_card(card_id, test_card, error);
    //should have returned false
    EXPECT_FALSE(result);
    //clear memory
    delete test_card;


}
// checks add_card, get_nof_cards, remove card
TEST(HandTest, HandWithOneCard)
{
    std::string name =  "vatkruidvat";

    player test_player(name);
    card* test_card = new card(2, 4);

    std::string error = "";
    std::string card_id = test_card->get_id();
    test_player.add_card(test_card, error);

    //add card to card vector
    std::vector<card*> cards;
    cards.push_back(test_card);

    //should have one card in the hand
    EXPECT_EQ(test_player.get_nof_cards(), cards.size());

    // hand in test_player should be the same as cards vector
    EXPECT_EQ(test_player.get_hand(), cards);
    //remove card after adding it

    bool result = test_player.remove_card(card_id, test_card, error);

    EXPECT_TRUE(result);

    // clean up memory
    delete test_card;
}

TEST(HandTest, HandWithThreeCards)
{
    std::string name = "vatkruidvat";
    player test_player(name);

    // Create the cards
    card* test_card_1 = new card(2, 4);
    card* test_card_2 = new card(10, 3);
    card* test_card_3 = new card(1, 1);

    std::string error = "";
    std::vector<card*> cards;

    // add the 3 cards
    test_player.add_card(test_card_1, error);
    cards.push_back(test_card_1);

    test_player.add_card(test_card_2, error);
    cards.push_back(test_card_2);

    test_player.add_card(test_card_3, error);
    cards.push_back(test_card_3);

    // check if nr of cards in the hand matches the size of the vector
    EXPECT_EQ(test_player.get_nof_cards(), cards.size());
    std::vector<card*> hand = test_player.get_hand()->get_cards();

    //check if all cards are still the same
    for (size_t i = 0; i < cards.size(); ++i) {
        //check color and value because == operator not overloaded for card type
        EXPECT_EQ(hand[i]->get_id(), cards[i]->get_id());


        //alternative
    }

    // remove card 2 and check
    std::string card_id_to_remove = test_card_2->get_id();
    bool result = test_player.remove_card(card_id_to_remove, test_card_2, error);

    EXPECT_TRUE(result);

    // check nr of cards after removal
    cards.erase(cards.begin() + 1);
    EXPECT_EQ(test_player.get_nof_cards(), cards.size());

    // check that hand == updated vector
    EXPECT_EQ(test_player.get_nof_cards(), cards.size());
    std::vector<card*> updated_hand = test_player.get_hand()->get_cards();
    for (size_t i = 0; i < cards.size(); ++i) {
        //since it is the exact same card, we can compare card id
        EXPECT_EQ(updated_hand[i]->get_id(), cards[i]->get_id());

        //EXPECT_EQ(updated_hand[i]->get_color(), cards[i]->get_color());
        //EXPECT_EQ(updated_hand[i]->get_value(), cards[i]->get_value());
    }

    // clean up memory
    delete test_card_1;
    delete test_card_2;
    delete test_card_3;

}
// check wrap_up_round()

//correct estimate -> + 10 per trick, + 20 in general
//incorrect estimate -> -10 for each trick offset

// tests if score is computed correctly if predicted tricks == collected tricks
TEST(HandTest, WrapUpRoundBadPrediction)
{
    std::string name =  "vatkruidvat";

    player test_player(name);
    test_player.set_scores(20);
    test_player.set_nof_tricks(2);
    test_player.set_nof_predicted(4);
    int previous_score = test_player.get_scores().back()->get_value();

    test_player.wrap_up_round();
    std::vector<serializable_value<int>*> scores = test_player.get_scores();
    int new_score = scores.back()->get_value();
    //since two tricks off (2 made, 4 predicted) --> subtract 2 * 10 from previous score
    EXPECT_EQ(new_score, 0);

    // lost 20 points from previous point to this round
    EXPECT_EQ(std::abs(new_score, previous_score), 20);


}

// tests if score is computed correctly if predicted tricks == collected tricks
TEST(HandTest, WrapUpRoundGoodGameplay)
{
    std::string name =  "vatkruidvat";

    player test_player(name);
    test_player.set_scores(20);
    test_player.set_nof_tricks(2);
    test_player.set_nof_predicted(2);
    int previous_score = test_player.get_scores().back()->get_value();


    // compute score
    test_player.wrap_up_round();
    std::vector<serializable_value<int>*> scores = test_player.get_scores();
    int new_score = scores.back()->get_value();
    //since two tricks off (2 made, 2 predicted) --> add 2 * 10 + 20 to previous score
    EXPECT_EQ(new_score, 60);
    int collected_points = scores.back()->get_value();

    //made 40 points in this round
    EXPECT_EQ(std::abs(new_score, previous_score), 40);


}

// Serialization and subsequent deserialization must yield the same object
TEST(CardTest, SerializationEquality) {
    std::string name =  "vatkruidvat";
    player player_send(name);
    player_send.set_scores(20);
    player_send.set_nof_tricks(2);
    player_send.set_nof_predicted(4);

    rapidjson::Document* json_send = player_send.to_json();
    std::string message = json_utils::to_string(json_send);
    delete json_send;

    rapidjson::Document json_received = rapidjson::Document(rapidjson::kObjectType);
    json_received.Parse(message.c_str());
    player* player_received = player::from_json(json_received);

    EXPECT_EQ(player_send.get_id(), player_received->get_id());
    EXPECT_EQ(player_send.get_nof_cards(), player_received->get_nof_cards());
    EXPECT_EQ(player_send.get_nof_predicted(), player_received->get_nof_cards());
    EXPECT_EQ(player_send.get_player_name(), player_received->get_player_name());
    EXPECT_EQ(player_send.get_scores(), player_received->get_scores());
    EXPECT_EQ(player_send.get_hand(), player_received->get_hand());
    delete player_received;
}

// Deserializing an invalid string must throw a WizardException
TEST(CardTest, SerializationException) {
    rapidjson::Document json = rapidjson::Document(rapidjson::kObjectType);
    json.Parse("not json");
    EXPECT_THROW(player::from_json(json), WizardException);
}