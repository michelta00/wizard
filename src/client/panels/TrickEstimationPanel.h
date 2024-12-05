//
// Created by hannah on 11/10/24.
//

#ifndef TRICKESTIMATIONPANEL_H
#define TRICKESTIMATIONPANEL_H

#include "../../common/game_state/game_state.h"
#include "../uiElements/InputField.h"
#include <wx/wx.h>
#include <wx/gbsizer.h>

class TrickEstimationPanel : public wxPanel{

public:
    TrickEstimationPanel(wxWindow* parent);
    ~TrickEstimationPanel();
    wxString getTrickEstimate();
    void buildGameState(game_state* gameState, player* me);
private:
    wxSize const panelSize = wxSize(960, 680);

    void buildCenter(wxGridBagSizer* sizer, game_state* gameState);
    void buildThisPlayer(wxGridBagSizer* sizer, game_state* gameState, player* me);
    void buildOtherPlayers(wxGridBagSizer* sizer, game_state* gameState, int myPosition);
    InputField* _trickEstimateField;
};

#endif //TRICKESTIMATIONPANEL_H
