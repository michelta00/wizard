#ifndef WIZARDUI_GAMECONTROLLER_H
#define WIZARDUI_GAMECONTROLLER_H

#include "windows/GameWindow.h"
#include "panels/ConnectionPanel.h"
#include "panels/MainGamePanelWizard.h"
#include "panels/TrickEstimationPanel.h"
#include "../common/game_state/game_state.h"

// TODO: include decide_trump_request
/**
 * @class GameController
 * @brief Controls visualization of the game state and processes client actions.
 *
 * This class handles the visualization of the game state a client received.
 * It adapts the visualization based on the parameters found in the game state
 * and reacts to interactions of a user with the GUI by sending out requests
 * via the client network manager to the server.
 */
class GameController {

public:
    /**
     * @brief Initializes game window when a new Wizard-client is started
     * @param gameWindow The game window that needs to be initialized
     */
    static void init(GameWindow* gameWindow);

    /**
     * @brief Handles connection to server when a user joins a game.
     */
    static void connectToServer();
    /**
     * @brief Updates game state whenever any player interacted with the server based on parameters of current game state.
     * @param newGameState The new game state send from the server.
     */
    static void updateGameState(game_state* newGameState);
    /**
     * @brief Send out 'start game' request to server.
     */
    static void startGame();
    /**
     * @brief Send out 'leave game' request to server.
     */
    static void leaveGame();
    /**
     * @brief Process input from trick estimation panel and trigger estimateTricks function.
     */
    static void processEstimateTricks();
    /**
     * @brief Send out 'estimate tricks' request to server.
     * @param nof_tricks Number of predicted tricks to win in the next round.
     */
    static void estimateTricks(int nof_tricks);
    /**
     * @brief Send out 'play card' request to server.
     * @param cardToPlay Pointer to card that user chooses to play.
     */

    static void playCard(card* cardToPlay);

    /**
     * @brief Finds event handler for main thread.
     * @return wxwidgets event handler
     */
    static wxEvtHandler* getMainThreadEventHandler();
    /**
     * @brief Handles showing of error messages.
     * @param title Title of the error message.
     * @param message Text of the error message.
     */
    static void showError(const std::string& title, const std::string& message);
    /**
     * @brief Set status message.
     * @param message Text of status message.
     */
    static void showStatus(const std::string& message);
    /**
     * @brief Shows message that round has finished and current scores of players at the end of a round.
     * @param oldGameState Game state of last round.
     * @param newGameState Updated game state of new round.
     */
    static void showNewRoundMessage(game_state* oldGameState, game_state* newGameState);
    /**
     * @brief Shows message at the end of each trick stating which player won the trick.
     * @param winner Winner of the trick.
     */
    static void showTrickOverMessage(const player* winner);
    /**
     * @brief Shows message at the end of the game that the game has ended and states the winner.
     */
    static void showGameOverMessage();

private:
    static GameWindow* _gameWindow; ///< The window which contains all panels shown to the user.
    static ConnectionPanel* _connectionPanel; ///< The pannel used to connect to the game.
    static MainGamePanelWizard* _mainGamePanelWizard; ///< The main panel used throughout playing the game.
    static TrickEstimationPanel* _trickEstimationPanel; ///< The panel used to estimate tricks at the beginning of each round.

    static player* _me; ///< The player for which the GUI is build.
    static game_state* _currentGameState; ///< The state of the game which is currently displayed in the GUI.

};


#endif //WIZARDUI_GAMECONTROLLER_H
