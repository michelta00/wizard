//
// Created by Michel Tarnow on 04.12.24.
//

#include "gtest/gtest.h"
#include "../src/common/exceptions/WizardException.h"
#include "../src/common/game_state/game_state.h"
#include "../src/common/serialization/json_utils.h"
#include "../src/common/serialization/vector_utils.h"
#include "../src/common/serialization/serializable_value.h"
#include "../src/common/serialization/unique_serializable.h"


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
    ASSERT_EQ(test_player3->get_nof_predicted(), -1);
    ASSERT_FALSE(test_game_state->estimate_tricks(test_player4, error, -1));
    ASSERT_EQ(error, "Trick estimate is too small. You can't win less than 0 tricks.");
    ASSERT_EQ(test_player4->get_nof_predicted(), -1);
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
    ASSERT_EQ(test_player5->get_nof_predicted(), -1);
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


// ########################## Create Game State From Json ########################## //

// this function enables us to start from arbitrary points in the game and test from there
void create_json(rapidjson::Value &json, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator,
                 std::vector<player*>& test_players, deck* test_deck, trick* test_trick, trick* test_last_trick,
                 serializable_value<bool>* test_is_finished, serializable_value<bool>* test_is_started,
                 serializable_value<bool>* test_is_estimation_phase, serializable_value<int>* test_round_number,
                 serializable_value<int>* test_trick_number, serializable_value<int>* test_starting_player_idx,
                 serializable_value<int>* test_trick_starting_player_idx, serializable_value<int>* test_current_player_idx,
                 serializable_value<int>* test_trump_color, serializable_value<int>* test_trump_card_value,
                 serializable_value<int>* test_trick_estimate_sum)
{
    // id
    json.AddMember("id", "4f38hvid20dj", allocator);

    // players
    json.AddMember("players", vector_utils::serialize_vector(test_players, allocator), allocator);

    // deck
    rapidjson::Value deck_val(rapidjson::kObjectType);
    test_deck->write_into_json(deck_val, allocator);
    json.AddMember("deck", deck_val, allocator);

    // trick
    rapidjson::Value trick_val(rapidjson::kObjectType);
    test_trick->write_into_json(trick_val, allocator);
    json.AddMember("trick", trick_val, allocator);

    // last trick
    rapidjson::Value last_trick_val(rapidjson::kObjectType);
    test_last_trick->write_into_json(last_trick_val, allocator);
    json.AddMember("last_trick", last_trick_val, allocator);

    // is finished
    rapidjson::Value is_finished_val(rapidjson::kObjectType);
    test_is_finished->write_into_json(is_finished_val, allocator);
    json.AddMember("is_finished", is_finished_val, allocator);

    // is started
    rapidjson::Value is_started_val(rapidjson::kObjectType);
    test_is_started->write_into_json(is_started_val, allocator);
    json.AddMember("is_started", is_started_val, allocator);

    // is estimation-phase
    rapidjson::Value is_estimation_phase_val(rapidjson::kObjectType);
    test_is_estimation_phase->write_into_json(is_estimation_phase_val, allocator);
    json.AddMember("is_estimation_phase", is_estimation_phase_val, allocator);

    // round number
    rapidjson::Value round_number_val(rapidjson::kObjectType);
    test_round_number->write_into_json(round_number_val, allocator);
    json.AddMember("round_number", round_number_val, allocator);

    // trick number
    rapidjson::Value trick_number_val(rapidjson::kObjectType);
    test_trick_number->write_into_json(trick_number_val, allocator);
    json.AddMember("trick_number", trick_number_val, allocator);

    // starting player index
    rapidjson::Value starting_player_idx_val(rapidjson::kObjectType);
    test_starting_player_idx->write_into_json(starting_player_idx_val, allocator);
    json.AddMember("starting_player_idx", starting_player_idx_val, allocator);

    // trick starting player index
    rapidjson::Value trick_starting_player_idx_val(rapidjson::kObjectType);
    test_trick_starting_player_idx->write_into_json(trick_starting_player_idx_val, allocator);
    json.AddMember("trick_starting_player_idx", trick_starting_player_idx_val, allocator);

    // current player index
    rapidjson::Value current_player_idx_val(rapidjson::kObjectType);
    test_current_player_idx->write_into_json(current_player_idx_val, allocator);
    json.AddMember("current_player_idx", current_player_idx_val, allocator);

    // trump color
    rapidjson::Value trump_color_val(rapidjson::kObjectType);
    test_trump_color->write_into_json(trump_color_val, allocator);
    json.AddMember("trump_color", trump_color_val, allocator);

    // trump card value
    rapidjson::Value trump_card_value_val(rapidjson::kObjectType);
    test_trump_card_value->write_into_json(trump_card_value_val, allocator);
    json.AddMember("trump_card_value", trump_card_value_val, allocator);

    // trick estimate sum
    rapidjson::Value trick_estimate_sum_val(rapidjson::kObjectType);
    test_trick_estimate_sum->write_into_json(trick_estimate_sum_val, allocator);
    json.AddMember("trick_estimate_sum", trick_estimate_sum_val, allocator);

}

TEST(GameStateJson, CreateJson)
{
    std::vector test_players = { new player("player1"), new player("player2"), new player("player3") };
    const auto test_deck = new deck();
    const auto test_trick = new trick();
    const auto test_last_trick = new trick();
    const auto test_is_finished = new serializable_value<bool>(false);
    const auto test_is_started = new serializable_value<bool>(false);
    const auto test_is_estimation_phase = new serializable_value<bool>(true);
    const auto test_round_number = new serializable_value<int>(0);
    const auto test_trick_number = new serializable_value<int>(0);
    const auto test_starting_player_idx = new serializable_value<int>(0);
    const auto test_trick_starting_player_idx = new serializable_value<int>(0);
    const auto test_current_player_idx = new serializable_value<int>(0);
    const auto test_trump_color = new serializable_value<int>(0);
    const auto test_trump_card_value = new serializable_value<int>(0);
    const auto test_trick_estimate_sum = new serializable_value<int>(0);

    auto* json = new rapidjson::Document();
    json->SetObject();
    create_json(*json, json->GetAllocator(), test_players, test_deck, test_trick, test_last_trick, test_is_finished,
                test_is_started, test_is_estimation_phase, test_round_number, test_trick_number, test_starting_player_idx,
                test_trick_starting_player_idx, test_current_player_idx, test_trump_color, test_trump_card_value,
                test_trick_estimate_sum);

    const std::string message = json_utils::to_string(json);
    delete json;

    auto json_received = rapidjson::Document(rapidjson::kObjectType);
    json_received.Parse(message.c_str());
    auto game_state_received = game_state::from_json(json_received);

    ASSERT_EQ(game_state_received->get_id(), "4f38hvid20dj");
    ASSERT_EQ(game_state_received->get_players().size(), 3);
}


// ########################## Update Current Player ########################## //

// test update current player within estimation phase
TEST(GameStateUpdateCurrentPlayerTest, UpatePLayerEstimation)
{
    std::vector test_players = { new player("player1"), new player("player2"),
                                 new player("player3"), new player("player4") };
    const auto test_deck = new deck();
    const auto test_trick = new trick();
    const auto test_last_trick = new trick();
    const auto test_is_finished = new serializable_value<bool>(false);
    const auto test_is_started = new serializable_value<bool>(true);
    const auto test_is_estimation_phase = new serializable_value<bool>(true);
    const auto test_round_number = new serializable_value<int>(4);                  // actually round 5
    const auto test_trick_number = new serializable_value<int>(0);                  // actually trick 1
    const auto test_starting_player_idx = new serializable_value<int>(0);           // in round 5, player 1 starts
    const auto test_trick_starting_player_idx = new serializable_value<int>(2);     // depends on who won last trick
    const auto test_current_player_idx = new serializable_value<int>(2);            // first one to estimate
    const auto test_trump_color = new serializable_value<int>(2);
    const auto test_trump_card_value = new serializable_value<int>(2);
    const auto test_trick_estimate_sum = new serializable_value<int>(0);

    auto* json = new rapidjson::Document();
    json->SetObject();
    create_json(*json, json->GetAllocator(), test_players, test_deck, test_trick, test_last_trick,
                test_is_finished, test_is_started, test_is_estimation_phase, test_round_number, test_trick_number,
                test_starting_player_idx, test_trick_starting_player_idx, test_current_player_idx, test_trump_color,
                test_trump_card_value, test_trick_estimate_sum);

    const std::string message = json_utils::to_string(json);
    delete json;

    auto json_received = rapidjson::Document(rapidjson::kObjectType);
    json_received.Parse(message.c_str());
    auto test_game_state = game_state::from_json(json_received);

    ASSERT_EQ(test_game_state->get_players().size(), 4);
    ASSERT_TRUE(test_game_state->is_estimation_phase());
    ASSERT_EQ(test_game_state->get_current_player()->get_player_name(), test_players.at(2)->get_player_name());

    std::string error = "error message";
    ASSERT_TRUE(test_game_state->update_current_player(error));
    ASSERT_EQ(test_game_state->get_current_player()->get_player_name(), test_players.at(3)->get_player_name());
}

// test update current player transition from estimation phase to play phase
TEST(GameStateUpdateCurrentPlayerTest, UpatePLayerEstimationToPlaying)
{
    std::vector test_players = { new player("player1"), new player("player2"),
                                 new player("player3"), new player("player4") };
    const auto test_deck = new deck();
    const auto test_trick = new trick();
    const auto test_last_trick = new trick();
    const auto test_is_finished = new serializable_value<bool>(false);
    const auto test_is_started = new serializable_value<bool>(true);
    const auto test_is_estimation_phase = new serializable_value<bool>(true);
    const auto test_round_number = new serializable_value<int>(4);                  // actually round 5
    const auto test_trick_number = new serializable_value<int>(0);                  // actually trick 1
    const auto test_starting_player_idx = new serializable_value<int>(0);           // in round 5, player 1 starts
    const auto test_trick_starting_player_idx = new serializable_value<int>(0);     // round starter starts
    const auto test_current_player_idx = new serializable_value<int>(3);            // last one to estimate
    const auto test_trump_color = new serializable_value<int>(2);
    const auto test_trump_card_value = new serializable_value<int>(2);
    const auto test_trick_estimate_sum = new serializable_value<int>(4);

    auto* json = new rapidjson::Document();
    json->SetObject();
    create_json(*json, json->GetAllocator(), test_players, test_deck, test_trick, test_last_trick,
                test_is_finished, test_is_started, test_is_estimation_phase, test_round_number, test_trick_number,
                test_starting_player_idx, test_trick_starting_player_idx, test_current_player_idx, test_trump_color,
                test_trump_card_value, test_trick_estimate_sum);

    const std::string message = json_utils::to_string(json);
    delete json;

    auto json_received = rapidjson::Document(rapidjson::kObjectType);
    json_received.Parse(message.c_str());
    auto test_game_state = game_state::from_json(json_received);

    ASSERT_EQ(test_game_state->get_players().size(), 4);
    ASSERT_TRUE(test_game_state->is_estimation_phase());
    ASSERT_EQ(test_game_state->get_current_player()->get_player_name(), test_players.at(3)->get_player_name());

    std::string error = "error message";
    ASSERT_TRUE(test_game_state->update_current_player(error));
    ASSERT_EQ(test_game_state->get_current_player()->get_player_name(), test_players.at(0)->get_player_name());
    ASSERT_FALSE(test_game_state->is_estimation_phase());
}

// test update current player transition from play phase to estimation phase of next round
TEST(GameStateUpdateCurrentPlayerTest, UpatePLayerPlayingToNextRound)
{
    std::vector test_players = { new player("player1"), new player("player2"),
                                 new player("player3"), new player("player4") };
    const auto test_deck = new deck();
    const auto test_trick = new trick();
    const auto test_last_trick = new trick();
    const auto test_is_finished = new serializable_value<bool>(false);
    const auto test_is_started = new serializable_value<bool>(true);
    const auto test_is_estimation_phase = new serializable_value<bool>(false);
    const auto test_round_number = new serializable_value<int>(4);                  // actually round 5
    const auto test_trick_number = new serializable_value<int>(4);                  // actually trick 5
    const auto test_starting_player_idx = new serializable_value<int>(0);           // in round 5, player 1 starts
    const auto test_trick_starting_player_idx = new serializable_value<int>(2);     // depends on last trick
    const auto test_current_player_idx = new serializable_value<int>(1);            // last one to play
    const auto test_trump_color = new serializable_value<int>(2);
    const auto test_trump_card_value = new serializable_value<int>(2);
    const auto test_trick_estimate_sum = new serializable_value<int>(4);

    auto* json = new rapidjson::Document();
    json->SetObject();
    create_json(*json, json->GetAllocator(), test_players, test_deck, test_trick, test_last_trick,
                test_is_finished, test_is_started, test_is_estimation_phase, test_round_number, test_trick_number,
                test_starting_player_idx, test_trick_starting_player_idx, test_current_player_idx, test_trump_color,
                test_trump_card_value, test_trick_estimate_sum);

    const std::string message = json_utils::to_string(json);
    delete json;

    auto json_received = rapidjson::Document(rapidjson::kObjectType);
    json_received.Parse(message.c_str());
    auto test_game_state = game_state::from_json(json_received);

    ASSERT_EQ(test_game_state->get_players().size(), 4);
    ASSERT_FALSE(test_game_state->is_estimation_phase());
    ASSERT_EQ(test_game_state->get_current_player()->get_player_name(), test_players.at(1)->get_player_name());

    std::string error = "error message";

    // give the players some values
    auto players = test_game_state->get_players();
    players.at(0)->set_nof_predicted(2);
    players.at(1)->set_nof_predicted(1);
    players.at(2)->set_nof_predicted(0);
    players.at(3)->set_nof_predicted(3);
    players.at(0)->set_nof_tricks(2);
    players.at(1)->set_nof_tricks(1);
    players.at(2)->set_nof_tricks(0);
    players.at(3)->set_nof_tricks(2);

    // give the trick some cards
    auto card1 = new card(1, 2);
    auto card2 = new card(10, 3);
    auto card3 = new card(3, 3);
    auto card4 = new card(6, 0);
    test_game_state->get_trick()->add_card(card1, test_game_state->get_players().at(0), error);
    test_game_state->get_trick()->add_card(card2, test_game_state->get_players().at(1), error);
    test_game_state->get_trick()->add_card(card3, test_game_state->get_players().at(2), error);
    test_game_state->get_trick()->add_card(card4, test_game_state->get_players().at(3), error);

    ASSERT_TRUE(test_game_state->update_current_player(error));
    ASSERT_TRUE(test_game_state->is_estimation_phase());
    ASSERT_EQ(test_game_state->get_current_player()->get_player_name(), test_players.at(1)->get_player_name());
}


// ########################## Wrap Up Round and Set Up Next Round ########################## //


// ########################## Wrap Up Round and Finish Game ########################## //

TEST(GameStateFinishGameTest, FinishGame)
{
    std::vector test_players = { new player("player1"), new player("player2"),
                                 new player("player3"), new player("player4") };
    const auto test_deck = new deck();
    const auto test_trick = new trick();
    const auto test_last_trick = new trick();
    const auto test_is_finished = new serializable_value<bool>(false);
    const auto test_is_started = new serializable_value<bool>(true);
    const auto test_is_estimation_phase = new serializable_value<bool>(false);
    const auto test_round_number = new serializable_value<int>(14);                 // actually round 15 (last)
    const auto test_trick_number = new serializable_value<int>(14);                 // actually trick 15
    const auto test_starting_player_idx = new serializable_value<int>(2);           // in round 5, player 3 starts
    const auto test_trick_starting_player_idx = new serializable_value<int>(2);     // depends on last trick
    const auto test_current_player_idx = new serializable_value<int>(1);            // last one to play
    const auto test_trump_color = new serializable_value<int>(2);
    const auto test_trump_card_value = new serializable_value<int>(2);
    const auto test_trick_estimate_sum = new serializable_value<int>(12);

    auto* json = new rapidjson::Document();
    json->SetObject();
    create_json(*json, json->GetAllocator(), test_players, test_deck, test_trick, test_last_trick,
                test_is_finished, test_is_started, test_is_estimation_phase, test_round_number, test_trick_number,
                test_starting_player_idx, test_trick_starting_player_idx, test_current_player_idx, test_trump_color,
                test_trump_card_value, test_trick_estimate_sum);

    const std::string message = json_utils::to_string(json);
    delete json;

    auto json_received = rapidjson::Document(rapidjson::kObjectType);
    json_received.Parse(message.c_str());
    auto test_game_state = game_state::from_json(json_received);

    ASSERT_EQ(test_game_state->get_players().size(), 4);
    ASSERT_FALSE(test_game_state->is_estimation_phase());
    ASSERT_EQ(test_game_state->get_current_player()->get_player_name(), test_players.at(1)->get_player_name());

    std::string error = "error message";

    // give the players some values
    auto players = test_game_state->get_players();
    players.at(0)->set_nof_predicted(6);
    players.at(1)->set_nof_predicted(5);
    players.at(2)->set_nof_predicted(0);
    players.at(3)->set_nof_predicted(1);
    players.at(0)->set_nof_tricks(5);
    players.at(1)->set_nof_tricks(5);
    players.at(2)->set_nof_tricks(0);
    players.at(3)->set_nof_tricks(1);

    // give the trick some cards
    auto card1 = new card(1, 2);
    auto card2 = new card(10, 3);
    auto card3 = new card(3, 3);
    auto card4 = new card(6, 0);
    test_game_state->get_trick()->add_card(card1, test_game_state->get_players().at(0), error);
    test_game_state->get_trick()->add_card(card2, test_game_state->get_players().at(1), error);
    test_game_state->get_trick()->add_card(card3, test_game_state->get_players().at(2), error);
    test_game_state->get_trick()->add_card(card4, test_game_state->get_players().at(3), error);

    ASSERT_TRUE(test_game_state->update_current_player(error));
    ASSERT_TRUE(test_game_state->is_finished());
}


// ########################## Others ########################## //

class GameStatePlayGameTest : public ::testing::Test
{
protected:

    void SetUp() override
    {
        // all of this has already been tested above
        test_game_state->add_player(test_player1, error);
        test_game_state->add_player(test_player2, error);
        test_game_state->add_player(test_player3, error);
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

TEST_F(GameStatePlayGameTest, ThreePlayers)
{
    // check if everything is set up correctly
    ASSERT_EQ(test_game_state->get_players().size(), 3);
    ASSERT_EQ(test_game_state->get_current_player()->get_id(), test_player1->get_id());
    ASSERT_TRUE(test_game_state->is_started());


    // estimation phase
    ASSERT_TRUE(test_game_state->is_estimation_phase());

    // first player
    ASSERT_TRUE(test_game_state->estimate_tricks(test_game_state->get_current_player(), error, 1));
    ASSERT_EQ(test_player1->get_nof_predicted(), 1);

    // second player
    ASSERT_TRUE(test_game_state->estimate_tricks(test_game_state->get_current_player(), error, 0));
    ASSERT_EQ(test_player2->get_nof_predicted(), 0);

    // third player
    ASSERT_TRUE(test_game_state->estimate_tricks(test_game_state->get_current_player(), error, 1));
    ASSERT_EQ(test_player3->get_nof_predicted(), 1);


    // playing phase
    ASSERT_FALSE(test_game_state->is_estimation_phase());

    // first player
    ASSERT_EQ(test_game_state->get_current_player()->get_hand()->get_cards().size(), 1);
    ASSERT_TRUE(test_game_state->play_card(test_game_state->get_current_player(), test_game_state->get_current_player()->get_hand()->get_cards().at(0)->get_id(), error));
    ASSERT_EQ(test_player1->get_hand()->get_cards().size(), 0);

    // second player
    ASSERT_EQ(test_game_state->get_current_player()->get_hand()->get_cards().size(), 1);
    ASSERT_TRUE(test_game_state->play_card(test_game_state->get_current_player(), test_game_state->get_current_player()->get_hand()->get_cards().at(0)->get_id(), error));
    ASSERT_EQ(test_player2->get_hand()->get_cards().size(), 0);

    // third player
    ASSERT_EQ(test_game_state->get_current_player()->get_hand()->get_cards().size(), 1);
    ASSERT_TRUE(test_game_state->play_card(test_game_state->get_current_player(), test_game_state->get_current_player()->get_hand()->get_cards().at(0)->get_id(), error));
    ASSERT_EQ(test_player3->get_hand()->get_cards().size(), 2);

    // after the third player played their card, the round is wrapped up and the new round is set up
    ASSERT_EQ(test_game_state->get_round_number(), 1);
    ASSERT_TRUE(test_game_state->is_estimation_phase());
}
