#ifndef MAINGAMEPANELWIZARD_H
#define MAINGAMEPANELWIZARD_H

#include <wx/wx.h>
#include <wx/gbsizer.h>
#include "../../common/game_state/game_state.h"

class MainGamePanelWizard : public wxPanel{

public:
    MainGamePanelWizard(wxWindow* parent);

    void buildGameState(game_state* gameState, player* me);

private:
    void buildThisPlayer(wxGridBagSizer* sizer, game_state* gameState, player* me);
    void buildTurnIndicator(wxGridBagSizer* sizer, game_state* gameState, player* me);
    void buildOtherPlayers(wxGridBagSizer* sizer, game_state* gameState, player *me, int myPosition);

    void buildTrickPile(wxGridBagSizer* sizer, game_state* gameState, player *me);
    void buildTrumpCard(wxGridBagSizer* sizer, game_state* gameState);

    void buildScoreLeaveButtons(wxGridBagSizer* sizer, game_state* gameState);

    wxSize const panelSize = wxSize(960, 680); // also set in the constructor implementation
    wxSize const cardSize = wxSize(70, 108);
};

#endif //MAINGAMEPANELWIZARD_H
