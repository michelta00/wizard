//
// Created by martinalavanya on 23.11.24.
//

//stand 4.12, 16:04 uhr

#include "gtest/gtest.h"
#include "../src/common/exceptions/WizardException.h"
#include "../src/common/game_state/cards/trick.h"
#include "../src/common/serialization/json_utils.h"

class trick_testing : public ::testing::Test
{
protected:
    serializable_value<int>* trick_color = nullptr;
    serializable_value<int>* trump_color = nullptr;
    trick* test_trick = nullptr;
    std::vector<std::pair<card*, player*>> cards_and_players;


    // Delete dynamically allocated cards and players
    //NOT NEEDED, DESTRUCTION HANDLED IN TRICK CLASS!!
    /*virtual void TearDown() {
        for (int i = 0; i < cards_and_players.size(); i++) {
            delete cards_and_players[i].first;   // delete the `card*`
            delete cards_and_players[i].second;  // delete the `player*`
        }
        delete trick_color;
        delete trump_color;
        cards_and_players.clear();
    }*/
};

//note about cards: 1-13 are number cards (x4), 0 are jester cards (x4), 14 are wizard cards (x4)
//note about trick color setup: if trick color == 0, it means the trick color has not been set yet. if first card is a wizard, it's set to -1
//
TEST_F(trick_testing, CreateAndChangeTrick) {
    trick_color = new serializable_value<int>(2); //trick color is 2
    trump_color = new serializable_value<int>(3);

    card* test_card = new card(2, 4);
    player* test_player = new player("vatkruidvat");
    cards_and_players.push_back(std::make_pair(test_card, test_player));

    test_trick = new trick("test_trick_id", cards_and_players, trick_color, trump_color);
    //check cards and players
    EXPECT_EQ(1, test_trick->get_cards_and_players().size());
    EXPECT_EQ(cards_and_players, test_trick->get_cards_and_players());
    // check that trick color was set correctly

    EXPECT_EQ(test_trick->get_trick_color(), 2);
    test_trick->set_trick_color(3);
    EXPECT_EQ(test_trick->get_trick_color(), 3);

    //check trump color
    EXPECT_EQ(test_trick->get_trump_color(), 3);



}

// scoring scenarios
// have any card be a wizard --> wins
TEST_F(trick_testing, OneWizard)
{
    trick_color = new serializable_value<int>(0); // trick color is 0
    trump_color = new serializable_value<int>(3);// trump color is 3
    //if a wizard is played, then the other players may play any card they wish (of any suit)
    card* card1 = new card(14, 0); //winning wizard card
    card* card2 = new card(2, 2);
    card* card3 = new card(4, 3); //trump card
    card* card4 = new card(14, 0); //second wizard card

    player* player1 = new player("Player_1");
    player* player2 = new player("Player_2");
    player* player3 = new player("Player_3");
    player* player4 = new player("Player_4");

     std::vector<std::pair<card*, player*>> cards_and_players = {
        {card1, player1},
        {card2, player2},
        {card3, player3},
        {card4, player4}
    };


    test_trick = new trick("test_trick_id", cards_and_players, trick_color, trump_color);
    std::string error = "";
    player* winner = test_trick->wrap_up_trick(error);
    EXPECT_EQ(player1->get_player_name(), winner->get_player_name()); // player 1 expected to win
    EXPECT_EQ(cards_and_players[0].first->get_value(), 14); //first card is a wizard
}

//first wizard card wins
TEST_F(trick_testing, AllWizards)
{
    trick_color = new serializable_value<int>(0); // trick color is 0
    trump_color = new serializable_value<int>(3); // trump color is 3

    card* card1 = new card(14, 0); //winning wizard card
    card* card2 = new card(14, 0);
    card* card3 = new card(14, 0);
    card* card4 = new card(14, 0);

    player* player1 = new player("Player_1");
    player* player2 = new player("Player_2");
    player* player3 = new player("Player_3");
    player* player4 = new player("Player_4");

    std::vector<std::pair<card*, player*>> cards_and_players = {
        {card1, player1},
        {card2, player2},
        {card3, player3},
        {card4, player4}
    };


    test_trick = new trick("test_trick_id", cards_and_players, trick_color, trump_color);
    std::string error = "";
    player* winner = test_trick->wrap_up_trick(error);
    EXPECT_EQ(player1->get_player_name(), winner->get_player_name()); // player 1 expected to win
    EXPECT_EQ(cards_and_players[1].first->get_value(), 14); //second card is a wizard
}


//first card is a jester --> wins only if everyone plays a jester, otherwise the jester looses
TEST_F(trick_testing, OneJesterHighestCard)
{
    trick_color = new serializable_value<int>(2); // trick color is 2
    trump_color = new serializable_value<int>(4);// trump color is 4
    //jester has color 0, value 0
    card* card1 = new card(0, 0); //jester looses
    card* card2 = new card(2, 2); //first number card --> decides trick color
    card* card3 = new card(4, 2); //winning card (highest of color 2)
    card* card4 = new card(3, 1);//player can't match suit and has no trump

    player* player1 = new player("Player_1");
    player* player2 = new player("Player_2");
    player* player3 = new player("Player_3");
    player* player4 = new player("Player_4");

    std::vector<std::pair<card*, player*>> cards_and_players = {
        {card1, player1},
        {card2, player2},
        {card3, player3},
        {card4, player4}
    };

    test_trick = new trick("test_trick_id", cards_and_players, trick_color, trump_color);
    std::string error = "";
    player* winner = test_trick->wrap_up_trick(error);
    EXPECT_EQ(player3->get_player_name(), winner->get_player_name()); // player 3 expected to win
    EXPECT_EQ(cards_and_players[3].first->get_value(), 3); //last card should be a 3
}

//first card is a jester, trump card wins
TEST_F(trick_testing, OneJesterWithTrump)
{
    trick_color = new serializable_value<int>(2); // trick color is 2
    trump_color = new serializable_value<int>(4);// trump color is 4
    //jester has color 0, value 0
    card* card1 = new card(0, 0); //jester looses
    card* card2 = new card(2, 2); //first number card --> decides trick color
    card* card3 = new card(4, 2);
    card* card4 = new card(3, 4);//winning card -> trump

    player* player1 = new player("Player_1");
    player* player2 = new player("Player_2");
    player* player3 = new player("Player_3");
    player* player4 = new player("Player_4");

    std::vector<std::pair<card*, player*>> cards_and_players = {
        {card1, player1},
        {card2, player2},
        {card3, player3},
        {card4, player4}
    };


    test_trick = new trick("test_trick_id", cards_and_players, trick_color, trump_color);
    std::string error = "";
    player* winner = test_trick->wrap_up_trick(error);
    EXPECT_EQ(player4->get_player_name(), winner->get_player_name()); // player 4 expected to win
    //auto third_card = cards_and_players[2].first;
    EXPECT_EQ(cards_and_players[2].first->get_value(), 4); //third card should be a 4
}


//first card is a jester, wizard card wins
TEST_F(trick_testing, OneJesterWithWizard)
{
    trick_color = new serializable_value<int>(0); // trick color is 0
    trump_color = new serializable_value<int>(4);// trump color is 4
    //jester has color 0, value 0
    card* card1 = new card(0, 0); //jester looses
    card* card2 = new card(14, 0); //wizard card wins
    card* card3 = new card(13, 3); //remaining players may play any suit because wizard was played
    card* card4 = new card(3, 4);

    player* player1 = new player("Player_1");
    player* player2 = new player("Player_2");
    player* player3 = new player("Player_3");
    player* player4 = new player("Player_4");

    std::vector<std::pair<card*, player*>> cards_and_players = {
        {card1, player1},
        {card2, player2},
        {card3, player3},
        {card4, player4}
    };


    test_trick = new trick("test_trick_id", cards_and_players, trick_color, trump_color);
    std::string error = "";
    player* winner = test_trick->wrap_up_trick(error);
    EXPECT_EQ(player2->get_player_name(), winner->get_player_name()); // player 2 expected to win
    EXPECT_EQ(cards_and_players[2].first->get_value(), 13); //third card should be a 13
}

//first card is a jester, wizard card wins
TEST_F(trick_testing, OneJesterWithWizardAndTrump)
{
    trick_color = new serializable_value<int>(2); // trick color is 2
    trump_color = new serializable_value<int>(4);// trump color is 4
    //jester has color 0, value 0
    card* card1 = new card(0, 0); //jester looses
    card* card2 = new card(4, 2); //trick color 2
    card* card3 = new card(13, 4); //trump card
    card* card4 = new card(14, 0); //wizard wins

    player* player1 = new player("Player_1");
    player* player2 = new player("Player_2");
    player* player3 = new player("Player_3");
    player* player4 = new player("Player_4");

    std::vector<std::pair<card*, player*>> cards_and_players = {
        {card1, player1},
        {card2, player2},
        {card3, player3},
        {card4, player4}
    };


    test_trick = new trick("test_trick_id", cards_and_players, trick_color, trump_color);
    std::string error = "";
    player* winner = test_trick->wrap_up_trick(error);
    EXPECT_EQ(player4->get_player_name(), winner->get_player_name()); // player 4 expected to win
    EXPECT_EQ(cards_and_players[3].first->get_value(), 14); //last card is a wizard
}

//all jesters
TEST_F(trick_testing, AllJesters)
{
    trick_color = new serializable_value<int>(0); // trick color is 0
    trump_color = new serializable_value<int>(4);// trump color for this trick is 4
    //jester has color 0, value 0
    card* card1 = new card(0, 0); //winning jester card
    card* card2 = new card(0, 0);
    card* card3 = new card(0, 0);
    card* card4 = new card(0, 0);


    player* player1 = new player("Player_1");
    player* player2 = new player("Player_2");
    player* player3 = new player("Player_3");
    player* player4 = new player("Player_4");

    std::vector<std::pair<card*, player*>> cards_and_players = {
        {card1, player1},
        {card2, player2},
        {card3, player3},
        {card4, player4}
    };


    test_trick = new trick("test_trick_id", cards_and_players, trick_color, trump_color);
    std::string error = "";
    player* winner = test_trick->wrap_up_trick(error);
    EXPECT_EQ(player1->get_player_name(), winner->get_player_name()); // player 1 expected to win
    EXPECT_EQ(cards_and_players[3].first->get_value(), 0); //last card should be a jester
}



//highest trump card wins
TEST_F(trick_testing, HighestTrump)
{
    trick_color = new serializable_value<int>(3); // trick color is 3
    trump_color = new serializable_value<int>(2);// trump color is 2


    card* card1 = new card(2, 3); //decides trick color
    card* card2 = new card(6, 2); //highest trump card --> wins trick
    card* card3 = new card(3, 2); //lower trump card
    card* card4 = new card(8, 3); //highest non-trump of trick color --> still looses

    player* player1 = new player("Player_1");
    player* player2 = new player("Player_2");
    player* player3 = new player("Player_3");
    player* player4 = new player("Player_4");

    std::vector<std::pair<card*, player*>> cards_and_players = {
        {card1, player1},
        {card2, player2},
        {card3, player3},
        {card4, player4}
    };


    test_trick = new trick("test_trick_id", cards_and_players, trick_color, trump_color);
    std::string error = "";
    player* winner = test_trick->wrap_up_trick(error);
    EXPECT_EQ(player2->get_player_name(), winner->get_player_name()); // player 2 expected to win
    EXPECT_EQ(cards_and_players[1].first->get_value(), 6); //second card should be a 6
}


//only trump played -> highest trump card wins
TEST_F(trick_testing, OnlyTrump)
{
    trick_color = new serializable_value<int>(1); // trick color is 1
    trump_color = new serializable_value<int>(1);// trump color is 1


    card* card1 = new card(12, 1);
    card* card2 = new card(6, 1);
    card* card3 = new card(13, 1);//highest trump wins
    card* card4 = new card(8, 1);

    player* player1 = new player("Player_1");
    player* player2 = new player("Player_2");
    player* player3 = new player("Player_3");
    player* player4 = new player("Player_4");

    std::vector<std::pair<card*, player*>> cards_and_players = {
        {card1, player1},
        {card2, player2},
        {card3, player3},
        {card4, player4}
    };


    test_trick = new trick("test_trick_id", cards_and_players, trick_color, trump_color);
    std::string error = "";
    player* winner = test_trick->wrap_up_trick(error);
    EXPECT_EQ(player3->get_player_name(), winner->get_player_name()); // player 3 expected to win
    EXPECT_EQ(cards_and_players[2].first->get_value(), 13); //third card should be a 13
}
// if no trump is played, the highest card wins
TEST_F(trick_testing, NoTrump)
{
    trick_color = new serializable_value<int>(1); // trick color is 1
    trump_color = new serializable_value<int>(3);// trump color is 3

    card* card1 = new card(1, 1);
    card* card2 = new card(6, 2);
    card* card3 = new card(11, 1); //winner
    card* card4 = new card(8, 1);

    player* player1 = new player("Player_1");
    player* player2 = new player("Player_2");
    player* player3 = new player("Player_3");
    player* player4 = new player("Player_4");

    std::vector<std::pair<card*, player*>> cards_and_players = {
        {card1, player1},
        {card2, player2},
        {card3, player3},
        {card4, player4}
    };

    test_trick = new trick("test_trick_id", cards_and_players, trick_color, trump_color);
    std::string error = "";
    player* winner = test_trick->wrap_up_trick(error);
    EXPECT_EQ(player3->get_player_name(), winner->get_player_name()); // player 3 expected to win
    EXPECT_EQ(cards_and_players[2].first->get_value(), 11); //third card should be a 11
}

//check setup round
TEST_F(trick_testing, SetupRound)
{
    trick_color = new serializable_value<int>(1); // trick color is 1
    trump_color = new serializable_value<int>(3);// trump color is 3


    card* card1 = new card(1, 1);
    card* card2 = new card(6, 2);
    card* card3 = new card(11, 1); //winner
    card* card4 = new card(8, 1);

    player* player1 = new player("Player_1");
    player* player2 = new player("Player_2");
    player* player3 = new player("Player_3");
    player* player4 = new player("Player_4");

    std::vector<std::pair<card*, player*>> cards_and_players = {
        {card1, player1},
        {card2, player2},
        {card3, player3},
        {card4, player4}
    };

    test_trick = new trick("test_trick_id", cards_and_players, trick_color, trump_color);
    std::string error = "";
    test_trick->set_up_round(error, 4);
    EXPECT_EQ(0, test_trick->get_trick_color()); //expect trick color 0 after reset
    EXPECT_EQ(0, test_trick->get_cards_and_players().size()); //expect empty cards and players vector

}

//check add card
TEST_F(trick_testing, AddCard)
{
    int trump_color = 3;
    std::string err = "";

    card* card1 = new card(1, 1);
    card* card2 = new card(6, 2);
    //create player and add card to player hand
    player* player1 = new player("Player_1");
    player1->add_card(card1, err);

    std::vector<std::pair<card*, player*>> cards_and_players = {{card1, player1}};

    test_trick = new trick();
    test_trick->set_up_round(err, trump_color);
    EXPECT_TRUE(test_trick->add_card(card1, player1, err));//card1 is in hand
    //EXPECT_FALSE(test_trick->add_card(card2, player1, err));//card 2 is not in hand
    EXPECT_EQ(test_trick->get_cards_and_players(), cards_and_players); //check if player was added to cards_


}


//test a full round
TEST_F(trick_testing, EntireRound)
{
    int trump_color = 4;
    std::string err = "";

    card* card1 = new card(2, 4);
    card* card2 = new card(4, 3);
    card* card3 = new card(8, 2);
    card* card4 = nullptr;// Null card


    player* player1 = new player("Player_1");
    player* player2 = new player("Player_2");
    player* player3 = new player("Player_3");

    // add cards to players' hands
    player1->add_card(card1, err);
    player2->add_card(card2, err);
    player3->add_card(card3, err);

    //create trick
    test_trick = new trick();
    test_trick->set_up_round(err, trump_color);

    // Test adding valid cards from player 1
    EXPECT_TRUE(test_trick->add_card(card1, player1, err));
    EXPECT_TRUE(test_trick->add_card(card2, player1, err));

    // Test adding a card not in the player's hand
    EXPECT_FALSE(test_trick->add_card(nullptr, player1, err));
    // Test adding a null card
    EXPECT_FALSE(test_trick->add_card(card4, player2, err));
    EXPECT_EQ(err, "The desired card cannot be played");

    // Test adding a valid card from player 2
    EXPECT_TRUE(test_trick->add_card(card3, player2, err));

    // Check the cards and players stored in the trick
    std::vector<std::pair<card*, player*>> expected_cards_and_players = {
        {card1, player1},
        {card2, player1},
        {card3, player2}
    };
    EXPECT_EQ(test_trick->get_cards_and_players(), expected_cards_and_players);

    player* winner = test_trick->wrap_up_trick(err);
    EXPECT_EQ(player1, winner); //player 1 played trump --> wins
}


// Serialization and subsequent deserialization must yield the same object
TEST_F(trick_testing, SerializationEquality) {
    trick_color = new serializable_value<int>(2); //trick color is 2
    trump_color = new serializable_value<int>(3);

    card* test_card = new card(2, 4);
    player* test_player = new player("vatkruidvat");
    cards_and_players.push_back(std::make_pair(test_card, test_player));

    test_trick = new trick("test_trick_id", cards_and_players, trick_color, trump_color);

    rapidjson::Document* json_send = test_trick->to_json();
    std::string message = json_utils::to_string(json_send);
    delete json_send;

    rapidjson::Document json_received = rapidjson::Document(rapidjson::kObjectType);
    json_received.Parse(message.c_str());
    trick* trick_received = trick::from_json(json_received);

    EXPECT_EQ(test_trick->get_id(), trick_received->get_id());
    EXPECT_EQ(test_trick->get_trump_color(), trick_received->get_trump_color());
    EXPECT_EQ(test_trick->get_cards_and_players()[0].first->get_color(), trick_received->get_cards_and_players()[0].first->get_color());
    EXPECT_EQ(test_trick->get_cards_and_players()[0].first->get_value(), trick_received->get_cards_and_players()[0].first->get_value());
    EXPECT_EQ(test_trick->get_trick_color(), trick_received->get_trick_color());

    delete trick_received;
}

// Deserializing an invalid string must throw a WizardException
TEST_F(trick_testing, SerializationException) {
    rapidjson::Document json = rapidjson::Document(rapidjson::kObjectType);
    json.Parse("not json");
    EXPECT_THROW(trick::from_json(json), WizardException);
}

