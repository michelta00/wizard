//
// Created by Marie Becker on 08.11.24.
//

#include "../src/common/game_state/game_state.h"
#include "gtest/gtest.h"
#include "../src/common/exceptions/WizardException.h"


/* A test fixture allows to reuse the same configuration of objects for all
 * tests in a test suite. The name of the fixture must match the test suite.
 *
 * For each test defined with TEST_F(), googletest will create a fresh test
 * fixture at runtime, immediately initialize it via SetUp(), run the test,
 * clean up by calling TearDown(), and then delete the test fixture.
 * Note that different tests in the same test suite have different test fixture
 * objects, and googletest always deletes a test fixture before it creates the
 * next one. googletest does not reuse the same test fixture for multiple
 * tests. Any changes one test makes to the fixture do not affect other tests.
 */
class GameStateTest : public ::testing::Test {

protected:
    virtual void SetUp() {
        players.resize(3);
        for(int i = 0; i < 3; i++) {
          players[i] = new player(); // TODO: add attributes to player
        }

        // TODO: add other attributed needed for game state like round state
    }

    /* Any object and subroutine declared here can be accessed in the tests */

    std::vector<player*> players;
    game_state* game;
    std::string err;
};