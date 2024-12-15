#ifndef MAINGAMEPANELWIZARD_H
#define MAINGAMEPANELWIZARD_H

#include <wx/wx.h>
#include <wx/gbsizer.h>
#include "../../common/game_state/game_state.h"

/**
 * @class MainGamePanelWizard
 * @brief Visualizes Game State to user during the card playing phase.
 * The Main Game Panel is shown before the game has started (Lobby)
 * and also while the game is played (outside the trick estimation phase).
 * It ensures that the game state is shown to all players in the GUI, and creates and positions all graphical elements.
 * It also allows interaction of the user with the GUI via button clicks.
 */
class MainGamePanelWizard : public wxPanel{

public:
    /**
     * @brief Constructs main game panel.
     * @param parent Game window the main game panel is put onto.
     */
    MainGamePanelWizard(wxWindow* parent);

    /**
     * @brief Shows the game state in the GUI the client gets from the server.
     * This function is called by the game controller to update the shown game state.
     * @param gameState New game state to show.
     * @param me Player to show the game state for (me).
     */
    void buildGameState(game_state* gameState, player* me);

private:
    /**
     * @brief Used by buildGameState to build the area of the panel that concerns the player of the user (me).
     * @param sizer Sizer which states the position of the player of the user.
     * @param gameState Current game state.
     * @param me Player.
     */
    void buildThisPlayer(wxGridBagSizer* sizer, game_state* gameState, player* me);
    /**
     * @brief Builds indicator that shows whoes turn it is.
     * This function colors the area of the player who is going to play next.
     * @param sizer Positioning.
     * @param gameState Current game state.
     * @param me Player of the user.
     */
    void buildTurnIndicator(wxGridBagSizer* sizer, game_state* gameState, player* me);
    /**
     * @brief Used by buildGameState to build the areas of the panel that concern all other players.
     * @param sizer Sizer which defines positioning.
     * @param gameState Current game state.
     * @param me Player of user (me).
     * @param myPosition Position of player of the user (me).
     */
    void buildOtherPlayers(wxGridBagSizer* sizer, game_state* gameState, player *me, int myPosition);
    /**
     * @brief Displays area of game which shows round number.
     * @param roundNumber Number of current round.
     */
    void showRoundOverlay(int roundNumber);
    /**
     * @brief Builds area of the deck which shows the trick pile.
     * @param sizer Positioning
     * @param gameState Current game state.
     * @param me Player of the user (me).
     */
    void buildTrickPile(wxGridBagSizer* sizer, game_state* gameState, player *me);
    /**
     * @brief Builds area of the game panel that shows trump card.
     * @param sizer Positioning.
     * @param gameState Current game state.
     */
    void buildTrumpCard(wxGridBagSizer* sizer, game_state* gameState);

    /**
     * @brief Builds score board button and leave game button.
     * The score board button can be used to look at the current scores.
     * The leave game button can be used to either leave the game before it has started without any consequences
     * or to leave the game after it was started. This leads to the game ending.
     * @param sizer Positioning
     * @param gameState Current game state.
     */
    void buildScoreLeaveButtons(wxGridBagSizer* sizer, game_state* gameState);

    // also set in the constructor implementation
    wxSize const panelSize = wxSize(960, 680); ///< Size of the panel.
    wxSize const cardSize = wxSize(70, 108); ///< Size of the shown cards.
};

#endif //MAINGAMEPANELWIZARD_H
