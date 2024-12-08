#include "ScoreBoardDialog.h"
#include <wx/grid.h>
#include <algorithm>


ScoreBoardDialog::ScoreBoardDialog(wxWindow* parent, const std::string& title, const std::string& message, game_state* gameState)
        : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxSize(400, 250), wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP){
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // put all player scores in a vector
    std::vector<player*> players = gameState->get_players();
    int numberOfPlayers = players.size();
    std::vector<std::vector<serializable_value<int>*>> tableData(numberOfPlayers);
    std::vector<std::string> playerNames(numberOfPlayers);

    // add playernames to vector
    for (int i = 0; i < numberOfPlayers; i++){
        playerNames.at(i) = players.at(i)->get_player_name();
    }

    int numRows = players.at(0)->get_scores().size()-1;
    int numCols = tableData.size(); // Assumes all vectors are the same length

    // avoid showing scores that are initialized with 0 but not actual acores yet
    if (numRows >= 1) {
        for (int i = 0; i < numberOfPlayers; i++) {
            auto scores = players.at(i)->get_scores();
            //tableData[i] = scores;
            tableData[i].insert(
                    tableData[i].end(),             // Insert starting at the end of tableData[i]
                    scores.begin()+1,                 // Start of scores
                    scores.end()                 // End of scores excluding the last element
            );
        }
    }

    // show scoreboard also if it is empty
    if (numRows >= 0 && numCols > 0) {
        // create the grid

        wxGrid* grid = new wxGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

        grid->CreateGrid(numRows, numCols);

        for (int col = 0; col < numCols; ++col) {
            grid->SetColLabelValue(col, playerNames[col]);
        }

        // Populate the grid with data
        for (int row = 0; row < numRows; ++row) {
            for (int col = 0; col < numCols; ++col) {
                grid->SetCellValue(row, col, wxString::Format("%d", tableData[col][row]->get_value()));
            }
        }

        sizer->Add(grid, 1, wxEXPAND | wxALL, 10);


    }
    // Add an OK button to close the dialog
    okButton = new wxButton(this, wxID_OK, "OK");
    sizer->Add(okButton, 0, wxALIGN_CENTER | wxALL, 10);

    SetSizerAndFit(sizer);
    Centre();  // Center the dialog on the screen
}