//
// Created by Michel Tarnow on 04.12.24.
//

#include "gtest/gtest.h"
#include "../src/common/exceptions/WizardException.h"
#include "../src/common/game_state/game_state.h"
#include "../src/common/serialization/json_utils.h"


// ########################## Some basic test ########################## //

// test the public constructor of game_state
TEST(GameStateTest, Constructor)
{
    auto test_game_state = game_state();

    ASSERT_EQ(test_game_state.get_players().size(), 0);
    ASSERT_EQ(test_game_state.get_round_number(), 0);
    ASSERT_EQ(test_game_state.get_trick_number(), 0);
    ASSERT_EQ(test_game_state.is_started(), 0);
    ASSERT_EQ(test_game_state.is_finished(), 0);
    ASSERT_EQ(test_game_state.is_full(), 0);
}

// test adding a player to an empty game
TEST(GameStateTest, AddPlayerToEmptyPlayers)
{
    auto test_game_state = game_state();
    auto test_player = new player("player1");
    std::string error = "error message";

    ASSERT_TRUE(test_game_state.add_player(test_player, error));
    ASSERT_EQ(test_game_state.get_players().size(), 1);
}

// test adding a player to a full game --> cannot add this player anymore
TEST(GameStateTest, AddPlayerToFullPlayers)
{
    auto test_game_state = game_state();
    auto test_player1 = new player("player1");
    auto test_player2 = new player("player2");
    auto test_player3 = new player("player3");
    auto test_player4 = new player("player4");
    auto test_player5 = new player("player5");
    auto test_player6 = new player("player6");
    auto test_player7 = new player("player7");
    std::string error = "error message";

    ASSERT_TRUE(test_game_state.add_player(test_player1, error));
    ASSERT_TRUE(test_game_state.add_player(test_player2, error));
    ASSERT_TRUE(test_game_state.add_player(test_player3, error));
    ASSERT_TRUE(test_game_state.add_player(test_player4, error));
    ASSERT_TRUE(test_game_state.add_player(test_player5, error));
    ASSERT_TRUE(test_game_state.add_player(test_player6, error));
    ASSERT_FALSE(test_game_state.add_player(test_player7, error));
    ASSERT_EQ(error, "Could not join game, because the max number of players is already reached.");
    ASSERT_EQ(test_game_state.get_players().size(), 6);
}

// test if player can be found in game
TEST(GameStateTest, IsPlayerInGame)
{
    auto test_game_state = game_state();
    auto test_player1 = new player("player1");
    std::string error = "error message";

    ASSERT_TRUE(test_game_state.add_player(test_player1, error));
    ASSERT_TRUE(test_game_state.is_player_in_game(test_player1));
}

// test if max round number calculation works
TEST(GameStateTest, GetMaxRoundNumber)
{
    auto test_game_state = game_state();
    auto test_player1 = new player("player1");
    auto test_player2 = new player("player2");
    auto test_player3 = new player("player3");
    auto test_player4 = new player("player4");
    std::string error = "error message";

    // 3 players --> 20 rounds
    ASSERT_TRUE(test_game_state.add_player(test_player1, error));
    ASSERT_TRUE(test_game_state.add_player(test_player2, error));
    ASSERT_TRUE(test_game_state.add_player(test_player3, error));
    ASSERT_EQ(test_game_state.get_max_round_number(), 20);

    // 4 players --> 15 rounds
    ASSERT_TRUE(test_game_state.add_player(test_player4, error));
    ASSERT_EQ(test_game_state.get_max_round_number(), 15);
}

TEST(GameStateTest, Serialization)
{
    auto test_game_state = game_state();
    auto test_player1 = new player("player1");
    auto test_player2 = new player("player2");
    auto test_player3 = new player("player3");
    std::string error = "error message";

    ASSERT_TRUE(test_game_state.add_player(test_player1, error));
    ASSERT_TRUE(test_game_state.add_player(test_player2, error));
    ASSERT_TRUE(test_game_state.add_player(test_player3, error));
    ASSERT_TRUE(test_game_state.start_game(error));
    ASSERT_EQ(test_game_state.is_started(), true);

    rapidjson::Document* json_send = test_game_state.to_json();
    std::string message = json_utils::to_string(json_send);
    delete json_send;

    auto json_received = rapidjson::Document(rapidjson::kObjectType);
    json_received.Parse(message.c_str());
    auto game_state_received = game_state::from_json(json_received);

    ASSERT_EQ(test_game_state.get_players().size(), game_state_received->get_players().size());
    ASSERT_EQ(test_game_state.get_round_number(), game_state_received->get_round_number());
    ASSERT_EQ(test_game_state.get_trick_number(), game_state_received->get_trick_number());
    ASSERT_EQ(test_game_state.get_id(), game_state_received->get_id());
}


// ########################## Starting Game and Setting Up Round ########################## //

class GameStateStartGameTest : public ::testing::Test
{
protected:

    void SetUp() override
    {
        test_game_state->add_player(test_player1, error);
        test_game_state->add_player(test_player2, error);
    }

    void TearDown() override
    {
        delete test_game_state;
    }

    game_state* test_game_state = new game_state();
    player* test_player1 = new player("player1");
    player* test_player2 = new player("player2");
    player* test_player3 = new player("player3");
    player* test_player4 = new player("player4");
    std::string error = "error message";
};

// test starting the game with too few players --> cannot be started
TEST_F(GameStateStartGameTest, StartGameTooFewPlayers)
{
    ASSERT_FALSE(test_game_state->start_game(error));
    ASSERT_EQ(error, "You need at least 3 players to start the game.");
}

// test starting the game with enough players --> can be started
TEST_F(GameStateStartGameTest, StartGameEnoughPlayers)
{
    ASSERT_TRUE(test_game_state->add_player(test_player3, error));
    ASSERT_TRUE(test_game_state->start_game(error));
    ASSERT_TRUE(test_game_state->is_started());
}

// test starting the game already started --> cannot be started
TEST_F(GameStateStartGameTest, StartGameAlreadyStarted)
{
    // add third player and start the game
    ASSERT_TRUE(test_game_state->add_player(test_player3, error));
    ASSERT_TRUE(test_game_state->start_game(error));
    ASSERT_TRUE(test_game_state->is_started());

    // game is already started --> cannot be started again
    ASSERT_FALSE(test_game_state->start_game(error));
    ASSERT_EQ(error, "Could not start game, as the game was already started");
}

// test if setting up of the round works (automatically called when game is started)
TEST_F(GameStateStartGameTest, StartGameSetUpRound)
{
    // add players and start the game (all of this has been tested already above)
    ASSERT_TRUE(test_game_state->add_player(test_player3, error));
    ASSERT_TRUE(test_game_state->add_player(test_player4, error));
    ASSERT_EQ(test_game_state->get_players().size(), 4);
    ASSERT_TRUE(test_game_state->start_game(error));
    ASSERT_TRUE(test_game_state->is_started());

    // now test if the round has been set up as well (e.g., players have 1 card in there hand)
    ASSERT_EQ(test_game_state->get_round_number(), 0);
    ASSERT_EQ(test_game_state->get_current_player()->get_hand()->get_cards().size(), 1);

}


// ########################## Estimate Tricks ########################## //

class GameStateEstimateTrickTest : public ::testing::Test
{
protected:

    void SetUp() override
    {
        // all of this has already been tested above
        test_game_state->add_player(test_player1, error);
        test_game_state->add_player(test_player2, error);
        test_game_state->add_player(test_player3, error);
        test_game_state->add_player(test_player4, error);
        test_game_state->add_player(test_player5, error);
        test_game_state->start_game(error);
    }

    void TearDown() override
    {
        delete test_game_state;
    }

    game_state* test_game_state = new game_state();
    player* test_player1 = new player("player1");
    player* test_player2 = new player("player2");
    player* test_player3 = new player("player3");
    player* test_player4 = new player("player4");
    player* test_player5 = new player("player5");
    std::string error = "error message";
};

// test the estimate_tricks function with valid estimates
TEST_F(GameStateEstimateTrickTest, EstimateTricksRound1Valid)
{
    ASSERT_EQ(test_game_state->get_round_number(), 0);

    // make some valid estimates
    ASSERT_TRUE(test_game_state->estimate_tricks(test_player1, error, 0));
    ASSERT_EQ(test_player1->get_nof_predicted(), 0);
    ASSERT_TRUE(test_game_state->estimate_tricks(test_player2, error, 1));
    ASSERT_EQ(test_player2->get_nof_predicted(), 1);
}

// test the estimate_tricks function with estimates that are < 0 or > round number
TEST_F(GameStateEstimateTrickTest, EstimateTricksRound1Invalid1)
{
    ASSERT_EQ(test_game_state->get_round_number(), 0);

    // make some invalid estimates
    ASSERT_FALSE(test_game_state->estimate_tricks(test_player3, error, 2));
    ASSERT_EQ(error, "Trick estimate is too big. You can't win more tricks than cards in your hand.");
    ASSERT_EQ(test_player3->get_nof_predicted(), 0);
    ASSERT_FALSE(test_game_state->estimate_tricks(test_player4, error, -1));
    ASSERT_EQ(error, "Trick estimate is too small. You can't win less than 0 tricks.");
    ASSERT_EQ(test_player4->get_nof_predicted(), 0);
}

// test the estimate_tricks function with estimates that sum up to be the round number
TEST_F(GameStateEstimateTrickTest, EstimateTricksRound1Invalid2)
{
    ASSERT_EQ(test_game_state->get_round_number(), 0);

    // all players but the last one estimate to win 0 tricks --> last player must predict 0 tricks as well
    ASSERT_TRUE(test_game_state->estimate_tricks(test_player1, error, 0));
    ASSERT_EQ(test_player1->get_nof_predicted(), 0);
    ASSERT_TRUE(test_game_state->estimate_tricks(test_player2, error, 0));
    ASSERT_EQ(test_player2->get_nof_predicted(), 0);
    ASSERT_TRUE(test_game_state->estimate_tricks(test_player3, error, 0));
    ASSERT_EQ(test_player3->get_nof_predicted(), 0);
    ASSERT_TRUE(test_game_state->estimate_tricks(test_player4, error, 0));
    ASSERT_EQ(test_player4->get_nof_predicted(), 0);

    // last player
    ASSERT_FALSE(test_game_state->estimate_tricks(test_player5, error, 1));
    ASSERT_EQ(test_player5->get_nof_predicted(), 0);
    ASSERT_EQ(error, "The tricks can't add up to the exact number of cards in the round. Please either choose a higher or lower number of tricks.");
    ASSERT_TRUE(test_game_state->estimate_tricks(test_player5, error, 0));
    ASSERT_EQ(test_player5->get_nof_predicted(), 0);
}


// ########################## Play Card ########################## //

class GameStatePlayCardTest : public ::testing::Test
{
protected:

    void SetUp() override
    {
        // all of this has already been tested above
        test_game_state->add_player(test_player1, error);
        test_game_state->add_player(test_player2, error);
        test_game_state->add_player(test_player3, error);
        test_game_state->add_player(test_player4, error);
        test_game_state->add_player(test_player5, error);
        test_game_state->start_game(error);
    }

    void TearDown() override
    {
        delete test_game_state;
    }

    game_state* test_game_state = new game_state();
    player* test_player1 = new player("player1");
    player* test_player2 = new player("player2");
    player* test_player3 = new player("player3");
    player* test_player4 = new player("player4");
    player* test_player5 = new player("player5");
    std::string error = "error message";
};

TEST_F(GameStatePlayCardTest, PlayCardRound1)
{
    std::string id = test_player1->get_hand()->get_cards().at(0)->get_id();
    ASSERT_TRUE(test_game_state->play_card(test_player1, id, error));
}


// ########################## Update Current Player ########################## //


// ########################## Wrap Up Round and Set Up Next Round ########################## //


// ########################## Wrap Up Round and Finish Game ########################## //


// ########################## Play Whole Game ########################## //
