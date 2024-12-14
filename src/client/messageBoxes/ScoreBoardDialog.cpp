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


    std::vector<int> currentScores(numberOfPlayers);
    for (int i = 0; i < numberOfPlayers; i++)
    {
        currentScores[i] = players.at(i)->get_scores().back()->get_value();
    }

    // determine current winners
    // Find the maximum value
    int maxValue = *std::max_element(currentScores.begin(), currentScores.end());
    int minValue = *std::min_element(currentScores.begin(), currentScores.end());

    // Find all indices with the maximum value
    std::vector<int> maxIndices;
    for (int i = 0; i < currentScores.size(); ++i) {
        if (currentScores[i] == maxValue) {
            maxIndices.push_back(i);
        }
    }
    // Find all indices with the maximum value
    std::vector<int> minIndices;
    for (int i = 0; i < currentScores.size(); ++i)
    {
        if (currentScores[i] == minValue) {
            minIndices.push_back(i);
        }
    }

    int numRows = players.at(0)->get_scores().size()-1;
    int numCols = tableData.size(); // Assumes all vectors are the same length

    // avoid showing scores that are initialized with 0 but not actual scores yet
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

        grid->SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);

        for (int col = 0; col < numCols; ++col) {
            grid->SetColLabelValue(col, playerNames[col]);
        }

        // Populate the grid with data
        for (int row = 0; row < numRows; ++row) {
            for (int col = 0; col < numCols; ++col) {
                grid->SetCellValue(row, col, wxString::Format("%d", tableData[col][row]->get_value()));
                if (maxIndices.size() != numberOfPlayers) // do not mark anything if all players have the same score
                {
                    for (auto i: maxIndices) // mark all leaders
                    {
                        if (col == i && row == numRows-1)
                        {
                            grid->SetCellBackgroundColour(row, col, wxColour(0, 100, 0));
                            grid->SetCellTextColour(row,col,*wxWHITE);
                        }
                    }
                    for (auto i: minIndices) // mark all losers
                    {
                        if (col == i && row == numRows-1)
                        {
                            grid->SetCellBackgroundColour(row, col, wxColour(102,0,51));
                            grid->SetCellTextColour(row,col,*wxWHITE);
                        }
                    }
                }
            }
        }
        // Automatically size columns and rows to fit content
        grid->AutoSizeColumns();
        grid->AutoSizeRows();

        sizer->Add(grid, 1, wxEXPAND | wxALL, 10);


    }
    // Add an OK button to close the dialog
    okButton = new wxButton(this, wxID_OK, "OK");
    sizer->Add(okButton, 0, wxALIGN_CENTER | wxALL, 10);

    SetSizerAndFit(sizer);
    Centre();  // Center the dialog on the screen
}