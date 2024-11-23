//
// Created by martinalavanya on 23.11.24.
//

#include "gtest/gtest.h"
#include "../src/common/exceptions/WizardException.h"
#include "../src/common/game_state/player/player.h"
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

    //check that removing card from empty hand does not work

    //check wrap_up_round()

}

TEST(HandTest, RemoveFromEmptyHand)
{


}