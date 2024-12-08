#include "TrickEstimationPanel.h"
#include "../uiElements/ImagePanel.h"
#include "../GameController.h"


TrickEstimationPanel::TrickEstimationPanel(wxWindow* parent): wxPanel(parent, wxID_ANY, wxDefaultPosition,
                                                                      wxSize(960, 680)){}
TrickEstimationPanel::~TrickEstimationPanel()
{
    delete _trickEstimateField;
    _trickEstimateField = nullptr;
}

void TrickEstimationPanel::buildGameState(game_state* gameState, player* me)
{
    this->DestroyChildren();

    // make new sizer
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    // child panel
    wxPanel *panel = new wxPanel(this, wxID_ANY);
    this->SetBackgroundColour(wxColour(100,100,100));

    this->SetMinSize(wxSize(960, 680));

    //create 3x3 Grid
    auto sizer = new wxGridBagSizer(4,3);
    std::vector<std::pair<wxGBPosition, wxGBSpan>> items  = {
        {{0,0}, {1,1}},
        {{0,1}, {1,1}},
        {{0,2}, {1,1}},
        {{1,0}, {1,1}},
        {{1,1}, {1,1}},
        {{1,2}, {1,1}},
        {{2,0}, {1,1}},
        {{2,1}, {1,1}},
        {{2,2}, {1,1}},
        {{3,0}, {1,3}}
    };

    // Add other players
    std::vector<player*> players = gameState->get_players();
    int myPosition = -1;

    // find our player in vector of players
    std::vector<player*>::iterator it = std::find_if(players.begin(), players.end(), [me](const player* x) {
       return x->get_id() == me->get_id();
    });

    if (it < players.end()) {
        me = *it;
        myPosition = it - players.begin();
    } else {
        GameController::showError("Game state error", "Could not find this player among players of server game.");
        return;
    }


    // specify minimum size of the panels
    int minWidth = TrickEstimationPanel::panelSize.GetWidth()/3;
    int minHeight = TrickEstimationPanel::panelSize.GetHeight()/4;

    // fill the gridsizer with panels and set the minimum size accordingly
    for (auto &item : items)
    {
        auto p = new wxPanel(panel, wxID_ANY, wxDefaultPosition);
        p->SetMinSize(wxSize(minWidth,minHeight));
        p->SetBackgroundColour(wxColour(102,0,51));
        sizer->Add(p, item.first, item.second, wxEXPAND,1);
    }


    // resize panels with window size
    // make all columns growable
    for (int col = 0; col < 3; ++col)
    {
        sizer->AddGrowableCol(col, 1);
    }
    // make all rows growable
    for (int row = 0; row < 3; ++row)
    {
        sizer->AddGrowableRow(row, 1);
    }

    // assign sizer to child panel
    panel->SetSizer(sizer);
    // add child panel to MainGamePanelWizard
    mainSizer->Add(panel, 1, wxEXPAND|wxALL,0);
    //assign sizer to MainGamewindow
    this->SetSizer(mainSizer);

    this->buildCenter(sizer, gameState);

    this->buildThisPlayer(sizer, gameState, me);

    this->buildOtherPlayers(sizer, gameState, myPosition);

    this->buildHand(sizer, gameState, me);

    this->buildTrumpColor(sizer, gameState);

    this->Layout();
}

void TrickEstimationPanel::buildTrumpColor(wxGridBagSizer *sizer, game_state *gameState) {
    wxGBSizerItem* trumpItem = sizer->FindItemAtPosition(wxGBPosition(2,0));
    wxPanel* trumpPanel = dynamic_cast<wxPanel*>(trumpItem->GetWindow());
    wxBoxSizer* trumpSizer_vert = new wxBoxSizer(wxVERTICAL);
    trumpPanel->SetSizer(trumpSizer_vert);

    wxStaticText* trumpText = new wxStaticText(trumpPanel, wxID_ANY, "TRUMP CARD",wxDefaultPosition, wxSize(120, 20), wxALIGN_CENTER);
    trumpText->SetForegroundColour(*wxWHITE);

    // increase font size of trump card text
    wxFont font = trumpText->GetFont(); // Get the current font of the wxStaticText
    font.SetPointSize(14);
    trumpText->SetFont(font);

    int trumpColor = gameState->get_trump_color();

    std::string cardImage = "assets/card_" + std::to_string(trumpColor) + ".png";
    ImagePanel* cardPanel = new ImagePanel(trumpPanel, cardImage, wxBITMAP_TYPE_ANY, wxDefaultPosition, TrickEstimationPanel::cardSize);


    trumpSizer_vert->Add(trumpText,0,wxALIGN_CENTER);
    trumpSizer_vert->Add(cardPanel, 0, wxALIGN_CENTER|wxALL, 5);
}

void TrickEstimationPanel::buildHand(wxGridBagSizer *sizer, game_state *gameState, player *me) {
    wxGBSizerItem* handItem = sizer->FindItemAtPosition(wxGBPosition(3,0));
    wxPanel* handPanel = dynamic_cast<wxPanel*>(handItem->GetWindow());

    // define two new sizers to be able to center the cards
    auto handPanelSizer_vert = new wxBoxSizer(wxVERTICAL);
    handPanel->SetSizer(handPanelSizer_vert);
    auto handPanelSizer_hor = new wxBoxSizer(wxHORIZONTAL);
    handPanelSizer_vert->Add(handPanelSizer_hor, 1, wxALIGN_CENTER);

    int numberOfCards = me->get_nof_cards();
    wxSize scaledCardSize = TrickEstimationPanel::cardSize;

    if (numberOfCards > 0) {

        if (numberOfCards * (TrickEstimationPanel::cardSize.GetWidth() + 8) >
            TrickEstimationPanel::panelSize.GetWidth()) {
            int scaledCardWidth = panelSize.GetWidth() / numberOfCards - 8;
            double cardAspectRatio = (double) cardSize.GetHeight() / (double) cardSize.GetWidth();
            int scaledCardHeight = (int) ((double) scaledCardWidth * cardAspectRatio);
            scaledCardSize = wxSize(scaledCardWidth, scaledCardHeight);
        }

        // Show all cards
        for (int i = 0; i < me->get_hand()->get_cards().size(); i++) {

            card *handCard = me->get_hand()->get_cards().at(i);

            std::string cardFile = "assets/card_" + std::to_string(handCard->get_value()) + "_" +
                                   std::to_string(handCard->get_color()) + ".png";

            ImagePanel *cardButton = new ImagePanel(handPanel, cardFile, wxBITMAP_TYPE_ANY, wxDefaultPosition,
                                                    scaledCardSize);

            if (gameState->get_current_player() == me && gameState->is_estimation_phase() == false) {
                cardButton->SetToolTip("Play card");
                cardButton->SetCursor(wxCursor(wxCURSOR_HAND));
                cardButton->Bind(wxEVT_LEFT_UP, [handCard](wxMouseEvent &event) {
                    GameController::playCard(handCard);
                });
            }
            handPanelSizer_hor->Add(cardButton, 0, wxALIGN_TOP | wxALL, 4);
        }

    }

}

void TrickEstimationPanel::buildCenter(wxGridBagSizer* sizer, game_state* gameState){
    wxGBSizerItem* centerItem = sizer->FindItemAtPosition(wxGBPosition(1,1));
    wxPanel* centerPanel = dynamic_cast<wxPanel*>(centerItem->GetWindow());

    // add sizer to center the text
    auto centerPanelSizer_vert = new wxBoxSizer(wxVERTICAL);
    centerPanel->SetSizer(centerPanelSizer_vert);
    auto centerPanelSizer_hor = new wxBoxSizer(wxHORIZONTAL);
    centerPanelSizer_vert->Add(centerPanelSizer_hor, 1, wxALIGN_CENTER);
    auto centerPanelSizer_vert2 = new wxBoxSizer(wxVERTICAL);
    centerPanelSizer_hor->Add(centerPanelSizer_vert2, 1, wxALIGN_CENTER);

    // add round number
    wxStaticText* roundNumber = new wxStaticText(centerPanel, wxID_ANY, "Round: " + std::to_string(gameState->get_round_number()+1) + "!",wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    roundNumber->SetForegroundColour(*wxWHITE);

    wxStaticText* trickSum = new wxStaticText(centerPanel, wxID_ANY, "Current sum: " + std::to_string(gameState->get_trick_estimate_sum()),wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    trickSum->SetForegroundColour(*wxWHITE);


    centerPanelSizer_vert2->Add(roundNumber, 0, wxALIGN_CENTER);
    centerPanelSizer_vert2->Add(trickSum, 0, wxALIGN_CENTER);

}

void TrickEstimationPanel::buildThisPlayer(wxGridBagSizer* sizer, game_state* gameState, player* me)
{
    wxGBSizerItem* meItem = sizer->FindItemAtPosition(wxGBPosition(2,1));
    wxPanel* mePanel = dynamic_cast<wxPanel*>(meItem->GetWindow());

    // define the sizers for alignment
    auto mePanelSizer_vert = new wxBoxSizer(wxVERTICAL);
    mePanel->SetSizer(mePanelSizer_vert);
    //auto mePanelSizer_hor = new wxBoxSizer(wxHORIZONTAL);
    //mePanelSizer_vert->Add(mePanelSizer_hor, 1, wxALIGN_CENTER);

    // add our name
    wxStaticText* playerName = new wxStaticText(mePanel, wxID_ANY, me->get_player_name(),wxDefaultPosition, wxSize(120, 20), wxALIGN_CENTER);
    playerName->SetForegroundColour(*wxWHITE);

    // increase font size of the player name
    wxFont font = playerName->GetFont(); // Get the current font of the wxStaticText
    font.SetPointSize(14);
    playerName->SetFont(font);

    mePanelSizer_vert->Add(playerName, 0, wxALIGN_CENTER);

    // if we have not submitted estimate yet
    if (me->get_nof_predicted() == -1)
    {
        if (gameState->get_current_player() == me)
        {
            // add input field for trick estimate
            wxStaticText* inputLabel = new wxStaticText(mePanel, wxID_ANY, "Trick Estimate:",wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
            inputLabel->SetForegroundColour(*wxWHITE);
            mePanelSizer_vert->Add(inputLabel, 0, wxALIGN_CENTER);


            this->_trickEstimateField = new InputField(mePanel, "", 0, "", 80);
            this->_trickEstimateField->SetLabelTextColour(wxColour(255, 255, 255)); // Set label text color to white
            mePanelSizer_vert->Add(_trickEstimateField, 0, wxALIGN_CENTER|wxALL, 10);

            // show button that allows our player to start the game
            wxButton* submitEstimateButton = new wxButton(mePanel, wxID_ANY, "Submit", wxDefaultPosition, wxSize(80, 32));
            submitEstimateButton->Bind(wxEVT_BUTTON, [](wxCommandEvent& event) {
                GameController::estimateTrick();
            });
            mePanelSizer_vert->Add(submitEstimateButton,0,wxALIGN_CENTER);
        }
        else
        {
            wxStaticText* playerName = new wxStaticText(mePanel, wxID_ANY, "waiting...",wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
            playerName->SetForegroundColour(*wxWHITE);
            mePanelSizer_vert->Add(playerName, 0, wxALIGN_CENTER);
        }
    }

    // display number of estimated tricks if already estimated
    else
    {
        // display the number of tricks
        wxStaticText* estimatedTricks = new wxStaticText(mePanel, wxID_ANY, std::to_string(me->get_nof_predicted()) + " Tricks",wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        estimatedTricks->SetForegroundColour(*wxWHITE);
        mePanelSizer_vert->Add(estimatedTricks, 0, wxALIGN_CENTER);
    }
    // mePanel->SetSizer(mePanelSizer_hor);
}

void TrickEstimationPanel::buildOtherPlayers(wxGridBagSizer* sizer, game_state* gameState, int myPosition)
{
    std::vector<player*> players = gameState->get_players();
    int numberOfPlayers = players.size();

    std::vector<wxGBPosition> otherPlayerPositions;

    if (numberOfPlayers == 3)
    {
        otherPlayerPositions = { wxGBPosition(0, 0),  wxGBPosition(0, 2)};
    }
    else if (numberOfPlayers == 4)
    {
        otherPlayerPositions = { wxGBPosition(1, 0),  wxGBPosition(0, 1),  wxGBPosition(1, 2)};
    }
    else if (numberOfPlayers == 5)
    {
        otherPlayerPositions = { wxGBPosition(1, 0), wxGBPosition(0, 0), wxGBPosition(0, 2), wxGBPosition(1, 2)};
    }
    else if (numberOfPlayers == 6)
    {
        otherPlayerPositions = { wxGBPosition(1, 0), wxGBPosition(0, 0), wxGBPosition(0, 1), wxGBPosition(0, 2), wxGBPosition(1, 2)};
    }

    for (int i = 0; i < otherPlayerPositions.size(); i++)
    {
        // define sizer for alignment at top center
        wxGBSizerItem* item = sizer->FindItemAtPosition(otherPlayerPositions[i]);
        wxPanel* panel = dynamic_cast<wxPanel*>(item->GetWindow());
        wxBoxSizer* playerSizer_vert = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(playerSizer_vert);

        player* otherPlayer = players.at((myPosition + i +1) % numberOfPlayers);

        // display name
        wxStaticText* playerNameText = new wxStaticText(panel, wxID_ANY, otherPlayer->get_player_name(),wxDefaultPosition, wxSize(panel->GetMinSize().GetWidth(), 25), wxALIGN_CENTER);
        playerNameText->SetForegroundColour(*wxWHITE);

        // increase font size of the player name
        wxFont font = playerNameText->GetFont(); // Get the current font of the wxStaticText
        font.SetPointSize(14);
        playerNameText->SetFont(font);

        playerSizer_vert->Add(playerNameText, 0, wxALIGN_CENTER);

        // display estimated tricks
        if (otherPlayer->get_nof_predicted() == -1)
        {
            wxStaticText* playerNameText = new wxStaticText(panel, wxID_ANY, "waiting...",wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
            playerNameText->SetForegroundColour(*wxWHITE);
            playerSizer_vert->Add(playerNameText, 0, wxALIGN_CENTER);
        }
        else
        {
            wxStaticText* playerNameText = new wxStaticText(panel, wxID_ANY, std::to_string(otherPlayer->get_nof_predicted()) + " Tricks",wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
            playerNameText->SetForegroundColour(*wxWHITE);
            playerSizer_vert->Add(playerNameText, 0, wxALIGN_CENTER);
        }

    }
}


wxString TrickEstimationPanel::getTrickEstimate()
{
    return this->_trickEstimateField->getValue();
}
