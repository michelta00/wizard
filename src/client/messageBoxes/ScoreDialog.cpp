//
// Created by hannah on 11/11/24.
//

#include "ScoreDialog.h"
#include <wx/sound.h>

wxBEGIN_EVENT_TABLE(ScoreDialog, wxDialog)
    EVT_TIMER(wxID_ANY, ScoreDialog::OnTimerClose)
wxEND_EVENT_TABLE()

ScoreDialog::ScoreDialog(wxWindow* parent, const std::string& title, const std::string& message)
            :wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP)
{
    // Set up the dialog layout
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* messageText = new wxStaticText(this, wxID_ANY, message, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    sizer->Add(messageText, 1, wxALL | wxEXPAND, 10);
    SetSizerAndFit(sizer);

    // Initialize the timer to close the dialog after 5 seconds
    _closeTimer.SetOwner(this, wxID_ANY);
    _closeTimer.Start(5000, wxTIMER_ONE_SHOT);  // 5 seconds
}

void ScoreDialog::OnTimerClose(wxTimerEvent& event) {
    Close();  // Close the dialog when the timer expires
}