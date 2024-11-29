#ifndef WIZARDUI_GAMECONTROLLER_H
#define WIZARDUI_GAMECONTROLLER_H

#include "windows/GameWindow.h"
#include "panels/ConnectionPanel.h"
#include "panels/MainGamePanel.h"
#include "panels/MainGamePanelWizard.h"
#include "panels/TrickEstimationPanel.h"
#include "network/ResponseListenerThread.h"
#include "../common/game_state/game_state.h"


class GameController {

public:
    static void init(GameWindow* gameWindow);

    static void connectToServer();
    static void updateGameState(game_state* newGameState);
    static void startGame();
    static void drawCard();
    static void fold();
    static void playCard(card* cardToPlay);
    static void estimateTrick();

    static wxEvtHandler* getMainThreadEventHandler();
    static void showError(const std::string& title, const std::string& message);
    static void showStatus(const std::string& message);
    static void showNewRoundMessage(game_state* oldGameState, game_state* newGameState);
    static void showGameOverMessage();
    static void showTrickEstimationPanel(int roundNumber);

private:
    static GameWindow* _gameWindow;
    static ConnectionPanel* _connectionPanel;
    static MainGamePanel* _mainGamePanel;
    static MainGamePanelWizard* _mainGamePanelWizard;
    static TrickEstimationPanel* _trickEstimationPanel;

    static player* _me;
    static game_state* _currentGameState;

};


#endif //WIZARDUI_GAMECONTROLLER_H
