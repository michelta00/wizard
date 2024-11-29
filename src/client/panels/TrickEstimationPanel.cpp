//
// Created by hannah on 11/10/24.
//

#include "TrickEstimationPanel.h"


TrickEstimationPanel::TrickEstimationPanel(wxWindow* parent): wxPanel(parent, wxID_ANY, wxDefaultPosition,
                                                                      wxSize(960, 680)){}

void TrickEstimationPanel::buildGameState(game_state* gameState, player* me)
{
    this->DestroyChildren();

    auto sizer = new wxGridBagSizer(1,5);
    std::vector<std::pair<wxGBPosition, wxGBSpan>> items  = {
        {{0,0}, {1,1}},
        {{0,1}, {1,1}},
        {{0,2}, {1,1}},
        {{0,3}, {1,1}},
        {{0,4}, {1,1}}
    };

    for (auto &item : items)
    {
        auto p = new wxPanel(this, wxID_ANY, wxDefaultPosition, sizer->GetEmptyCellSize());
        p->SetBackgroundColour(wxColour(0,100,100));

        sizer->Add(p, item.first, item.second, wxEXPAND);
    }
    // Make all columns growable
    for (int col = 0; col < 5; ++col)
    {
        sizer->AddGrowableCol(col, 1); // Second parameter is the grow priority
    }
    sizer->AddGrowableRow(0, 10);
    this->SetSizer(sizer);
}