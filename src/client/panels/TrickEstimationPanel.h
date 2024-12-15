//
// Created by hannah on 11/10/24.
//

#ifndef TRICKESTIMATIONPANEL_H
#define TRICKESTIMATIONPANEL_H

#include "../../common/game_state/game_state.h"
#include "../uiElements/InputField.h"
#include <wx/wx.h>
#include <wx/gbsizer.h>

/**
* @brief The TrickEstimationPanel class handles the GUI layout and functionality for the trick estimation phase of the game.
* @class TrickEstimationPanel
*/
class TrickEstimationPanel : public wxPanel{

public:
    /**
     * @brief Constructor for TrickEstimationPanel.
     * @param parent Pointer to the parent window.
     */
    TrickEstimationPanel(wxWindow* parent);
    /**
     * @brief Destructor for TrickEstimationPanel.
     */
    ~TrickEstimationPanel();
    /**
     * @brief Gets the trick estimate entered by the player.
     * @return A wxString containing the entered trick estimate.
     */
    wxString getTrickEstimate();
    /**
      * @brief Builds the game state for the trick estimation phase.
      * @param gameState Pointer to the current game state.
      * @param me Pointer to the player being the current user.
      */
    void buildGameState(game_state* gameState, player* me);
private:
    wxSize const panelSize = wxSize(1200, 850); ///< panel size used for layout calculations
    wxSize const cardSize = wxSize(85, 131); ///< size of displayed cards

    /**
     * @brief builds center panel showing round number and prediction sum
     * @param sizer Pointer to the grid bag sizer used for the layout
     * @param gameState Pointer to the current game state
     */
    void buildCenter(wxGridBagSizer* sizer, game_state* gameState);
    /**
     * @brief Displays trump card
     * @param sizer Pointer to the grid bag sizer used for the layout
     * @param gameState Pointer to the current game state
     */
    void buildTrumpColor(wxGridBagSizer* sizer, game_state* gameState);
    /**
     * Displays hand of player being the user
     * @param sizer Pointer to the grid bag sizer used for the layout
     * @param gameState Pointer to the current game state
     * @param me Pointerto the player being the user
     */
    void buildHand(wxGridBagSizer* sizer, game_state* gameState, player* me);
    /**
     * @brief Displays name and estimated tricks of the player being the user
     * @param sizer Pointer to the grid bag sizer used for the layout
     * @param gameState Pointer to the current game state
     * @param me Pointer to the player being the user
     */
    void buildThisPlayer(wxGridBagSizer* sizer, game_state* gameState, player* me);
    /**
     * @brief Displays name and estimated tricks of the players not being the user
     * @param sizer Pointer to the grid bag sizer used for the layout
     * @param gameState Pointer to the current game state
     * @param myPosition Index of the player being the user in the player list.
     */
    void buildOtherPlayers(wxGridBagSizer* sizer, game_state* gameState, int myPosition);
    /**
     * @brief Builds scoreboard and leave game button
     * @param sizer Pointer to the grid bag sizer used for the layout
     * @param gameState Pointer to the current game state
     */
    void buildScoreLeaveButtons(wxGridBagSizer* sizer, game_state* gameState);

    InputField* _trickEstimateField; ///< input field for entering trick estimates
};

#endif //TRICKESTIMATIONPANEL_H
