//
// Created by hannah on 11/10/24.
//

#ifndef TRICKESTIMATIONPANEL_H
#define TRICKESTIMATIONPANEL_H

#include "../../common/game_state/game_state.h"
#include <wx/wx.h>
#include <wx/gbsizer.h>

class TrickEstimationPanel : public wxPanel{

public:
    TrickEstimationPanel(wxWindow* parent);

    void buildGameState(game_state* gameState, player* me);
};

#endif //TRICKESTIMATIONPANEL_H
