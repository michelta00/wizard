//
// Created by hannah on 12/8/24.
//

#ifndef WIZARD_SCOREBOARDDIALOG_H
#define WIZARD_SCOREBOARDDIALOG_H

#include <wx/wx.h>
#include "../../common/game_state/game_state.h"

class ScoreBoardDialog: public wxDialog {
public: ScoreBoardDialog(wxWindow* parent, const std::string& title, const std::string& message, game_state* gameState);

private:
    wxButton* okButton;

};


#endif //WIZARD_SCOREBOARDDIALOG_H

