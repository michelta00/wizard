#include "GameController.h"
#include "../common/network/requests/join_game_request.h"
#include "../common/network/requests/start_game_request.h"
#include "../common/network/requests/play_card_request.h"
#include "../common/network/requests/estimate_tricks_request.h"
#include "../common/network/requests/leave_game_request.h"
#include "../client/messageBoxes/ErrorDialog.h"
#include "../client/messageBoxes/ScoreDialog.h"
#include "network/ClientNetworkManager.h"


// initialize static members
GameWindow* GameController::_gameWindow = nullptr;
ConnectionPanel* GameController::_connectionPanel = nullptr;
MainGamePanelWizard* GameController::_mainGamePanelWizard = nullptr;
TrickEstimationPanel* GameController::_trickEstimationPanel = nullptr;

player* GameController::_me = nullptr;
game_state* GameController::_currentGameState = nullptr;


void GameController::init(GameWindow* gameWindow) {

    //TODO: panels need to be adapted
    GameController::_gameWindow = gameWindow;

    // Set up main panels
    GameController::_connectionPanel = new ConnectionPanel(gameWindow);
    GameController::_mainGamePanelWizard = new MainGamePanelWizard(gameWindow);
    GameController::_trickEstimationPanel = new TrickEstimationPanel(gameWindow);

    // Hide all panels
    GameController::_connectionPanel->Show(false);
    GameController::_mainGamePanelWizard->Show(false);
    GameController::_trickEstimationPanel->Show(false);

    // Only show connection panel at the start of the game
    GameController::_gameWindow->showPanel(GameController::_connectionPanel);

    // Set status bar
    GameController::showStatus("Not connected");
}


void GameController::connectToServer() {

    // get values form UI input fields
    wxString inputServerAddress = GameController::_connectionPanel->getServerAddress().Trim();
    wxString inputServerPort = GameController::_connectionPanel->getServerPort().Trim();
    wxString inputPlayerName = GameController::_connectionPanel->getPlayerName().Trim();

    // check that all values were provided
    if(inputServerAddress.IsEmpty()) {
        GameController::showError("Input error", "Please provide the server's address");
        return;
    }
    if(inputServerPort.IsEmpty()) {
        GameController::showError("Input error", "Please provide the server's port number");
        return;
    }
    if(inputPlayerName.IsEmpty()) {
        GameController::showError("Input error", "Please enter your desired player name");
        return;
    }

    // convert host from wxString to std::string
    std::string host = inputServerAddress.ToStdString();

    // convert port from wxString to uint16_t
    unsigned long portAsLong;
    if(!inputServerPort.ToULong(&portAsLong) || portAsLong > 65535) {
        GameController::showError("Connection error", "Invalid port");
        return;
    }
    auto port = static_cast<uint16_t>(portAsLong);

    // convert player name from wxString to std::string
    std::string playerName = inputPlayerName.ToStdString();
    //player name length check
    if (playerName.size() > 15)
    {
        GameController::showError("Connection error", "Invalid player name length. Please enter a player name between 1 and 15 characters.");
        return;
    }


    // connect to network
    ClientNetworkManager::init(host, port);

    // send request to join game
    GameController::_me = new player(playerName);
    join_game_request request = join_game_request(GameController::_me->get_id(), GameController::_me->get_player_name());
    ClientNetworkManager::sendRequest(request);


}


void GameController::updateGameState(game_state* newGameState) {

    // the existing game state is now old
    game_state* oldGameState = GameController::_currentGameState;

    // save the new game state as our current game state
    GameController::_currentGameState = newGameState;
    if(oldGameState != nullptr) {

        if(GameController::_currentGameState->is_finished()) {
            GameController::showGameOverMessage();
        }
        else if(GameController::_currentGameState->is_started())
        {
            int round_number = _currentGameState->get_round_number();

            // end of trick
            if(_currentGameState->get_trick_number() != oldGameState->get_trick_number() || round_number != oldGameState->get_round_number())
            {
                trick* trick_to_show = new trick(*_currentGameState->get_last_trick());
                oldGameState->set_trick(trick_to_show);
                player* winner = oldGameState->get_trick()->get_winner();
                // get player of oldGameState that has same id as winner (winner is player of current game state)
                for (auto& player_ : oldGameState->get_players()) {
                    if (player_->get_id() == winner->get_id()) {
                        winner = player_;
                    }
                }
                // add trick to winning player
                winner->set_nof_tricks(winner->get_nof_tricks() + 1);

                // make sure that last card is removed from hand
                const player* last_player = oldGameState->get_current_player();
                std::string last_player_error = "Card of last player of trick could not be removed from hand";
                last_player->get_hand()->remove_card(trick_to_show->get_cards_and_players().back().first->get_id(), last_player_error);

                GameController::_gameWindow->showPanel(GameController::_mainGamePanelWizard);
                GameController::_mainGamePanelWizard->buildGameState(oldGameState, GameController::_me);
                showTrickOverMessage(winner);
            }

            if(round_number != oldGameState->get_round_number())
            {
                // new round has started
                showNewRoundMessage(oldGameState, newGameState);
                showStatus("Round " + std::to_string(newGameState->get_round_number() + 1));
            }

            // estimation phase
            if(GameController::_currentGameState->is_estimation_phase()) {
                GameController::_gameWindow->showPanel(GameController::_trickEstimationPanel);
                GameController::_trickEstimationPanel->buildGameState(GameController::_currentGameState, GameController::_me);
                delete oldGameState;
                return;
            }
        }
        // delete the old game state, we don't need it anymore
        delete oldGameState;
    }



    // make sure we are showing the main game panel in the window (if we are already showing it, nothing will happen)
    GameController::_gameWindow->showPanel(GameController::_mainGamePanelWizard);
    GameController::_mainGamePanelWizard->buildGameState(GameController::_currentGameState, GameController::_me);
}


void GameController::startGame() {
    start_game_request request = start_game_request(GameController::_currentGameState->get_id(), GameController::_me->get_id());
    ClientNetworkManager::sendRequest(request);

}

void GameController::leaveGame() {
    leave_game_request request = leave_game_request(GameController::_currentGameState->get_id(), GameController::_me->get_id(), _me->get_player_name());
    ClientNetworkManager::sendRequest(request);
}

void GameController::estimateTricks(int nof_tricks) {
    estimate_tricks_request request = estimate_tricks_request(GameController::_currentGameState->get_id(), GameController::_me->get_id(), nof_tricks);
    ClientNetworkManager::sendRequest(request);
}

void GameController::playCard(card* cardToPlay) {
    play_card_request request = play_card_request(GameController::_currentGameState->get_id(), GameController::_me->get_id(), cardToPlay->get_id());
    ClientNetworkManager::sendRequest(request);
}

// TODO: estimate trick request, here message box with entry
void GameController::estimateTrick()
{
    wxString trickEstimate = GameController::_trickEstimationPanel->getTrickEstimate().Trim();

    //check whether value was provided
    if(trickEstimate.IsEmpty()) {
        GameController::showError("Input error", "Please provide your trick estimate!");
        return;
    }

    // attempt to parse string into a long integer and cast to int if succesful
    long trickEstimateValue;
    int trickEstimateInt;
    if (trickEstimate.ToLong(&trickEstimateValue)) {
        trickEstimateInt = static_cast<int>(trickEstimateValue);
        GameController::estimateTricks(trickEstimateInt);
    } else {
        // Handle the error: the string was not a valid integer
        GameController::showError("Invalid input!"," Please enter a valid number for the trick estimate.");
    }

    /*
    std::string title = "How many tricks?";
    std::string message = "Enter estimated number of tricks";
    std::string buttonLabel = "OK";

    wxMessageDialog dialogBox = wxMessageDialog(nullptr, message, title, wxICON_NONE);
    dialogBox.SetOKLabel(wxMessageDialog::ButtonLabel(buttonLabel));
    dialogBox.ShowModal();
    */
}


wxEvtHandler* GameController::getMainThreadEventHandler() {
    return GameController::_gameWindow->GetEventHandler();
}


void GameController::showError(const std::string& title, const std::string& message) {
    // Load a custom image for the error dialog
    wxString fullpath = "assets/error.png";

    wxBitmap errorImage(fullpath, wxBITMAP_TYPE_ANY);
    // Create and show the custom error dialog
    ErrorDialog errorDialog(nullptr, title, message, errorImage);
    errorDialog.ShowModal();
}


void GameController::showStatus(const std::string& message) {
    GameController::_gameWindow->setStatus(message);
}


void GameController::showNewRoundMessage(game_state* oldGameState, game_state* newGameState) {
    std::string title = "Round Completed";
    std::string message = "The players gained the following points:\n";
    std::string buttonLabel = "Start next round";

    // add the point differences of all players to the messages
    for(int i = 0; i < oldGameState->get_players().size(); i++) {

        player* oldPlayerState = oldGameState->get_players().at(i);
        player* newPlayerState = newGameState->get_players().at(i);

        int scoreDelta = newPlayerState->get_scores().back()->get_value() - oldPlayerState->get_scores().back()->get_value();
        std::string scoreText = std::to_string(scoreDelta);
        if(scoreDelta > 0) {
            scoreText = "+" + scoreText;
        }

        std::string playerName = newPlayerState->get_player_name();
        if(newPlayerState->get_id() == GameController::_me->get_id()) {
            playerName = "You";
        }
        message += "\n" + playerName + ":     " + scoreText;
    }

    //TODO: what to do with these? Is message just going away or do you have to press okay
    //wxMessageDialog dialogBox = wxMessageDialog(nullptr, message, title, wxICON_NONE);
    //dialogBox.SetOKLabel(wxMessageDialog::ButtonLabel(buttonLabel));
    //dialogBox.ShowModal();

    auto* dialog = new ScoreDialog(GameController::_gameWindow, title, message);

    dialog->ShowModal();
}

void GameController::showTrickOverMessage(const player* winner)
{
    std::string title = "Trick Completed";
    std::string message = " won the trick\n";
    message = winner->get_player_name() + message;

    auto dialog = new ScoreDialog(GameController::_gameWindow, title, message);
    dialog->ShowModal();
}


void GameController::showGameOverMessage() {
    std::string title = "Game Over!";
    std::string message = "Final score:\n";
    std::string buttonLabel = "Close Game";

    // TODO: change logic to determine winner because now we have vector of scores
    // sort players by score
    std::vector<player*> players = GameController::_currentGameState->get_players();
    std::sort(players.begin(), players.end(), [](const player* a, const player* b) -> bool {
        return a->get_scores().back()->get_value() > b->get_scores().back()->get_value();
    });

    // list all players
    for(int i = 0; i < players.size(); i++) {

        player* playerState = players.at(i);
        std::string scoreText = std::to_string(playerState->get_scores().back()->get_value());

        // first entry is the winner
        std::string winnerText = "";
        if(i == 0) {
            winnerText = "     Winner!";
        }

        std::string playerName = playerState->get_player_name();
        if(playerState->get_id() == GameController::_me->get_id()) {
            playerName = "You";

            if(i == 0) {
                winnerText = "     You won!!!";
            }
        }
        message += "\n" + playerName + ":     " + scoreText + winnerText;
    }

    wxMessageDialog dialogBox = wxMessageDialog(nullptr, message, title, wxICON_NONE);
    dialogBox.SetOKLabel(wxMessageDialog::ButtonLabel(buttonLabel));
    int buttonClicked = dialogBox.ShowModal();

    if(buttonClicked == wxID_OK) {
        GameController::_gameWindow->Close();
    }
}
