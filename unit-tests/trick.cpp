//
// Created by martinalavanya on 23.11.24.
//

#include "gtest/gtest.h"
#include "../src/common/exceptions/WizardException.h"
#include "../src/common/game_state/cards/trick.h"
#include "../src/common/serialization/json_utils.h"

class trick_testing : public ::testing::Test {

protected:
    virtual void SetUp() {
        serializable_value<int>* trick_color = new serializable_value<int>(2);;
        serializable_value<int>* trump_color = new serializable_value<int>(3);
        std::vector<std::pair<card*, player*>> cards_and_players;

        card* test_card = new card(2, 4);
        player* test_player = new player("vatkruidvat");
        cards_and_players.push_back(std::make_pair(test_card, test_player));

    }

    /* Any object and subroutine declared here can be accessed in the tests */

    // cards[i][j] holds a pointer to the j-th copy of a card of value i
    std::vector<std::pair<card*, player*>> cards_and_players;
    trick* test_trick;
    std::string err;
};

TEST_F(trick_testing, CreatePlayer)
{

    std::string trick_id = "trick_1";
    trick test_trick(trick_id, cards_and_players, trick_color, trump_color);

    //check name

    EXPECT_EQ(test_trick.get_trick_color(), 2);


}