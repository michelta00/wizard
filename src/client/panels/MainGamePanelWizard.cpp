#include "MainGamePanelWizard.h"
#include "../GameController.h"
#include "../uiElements/ImagePanel.h"
#include <wx/gbsizer.h>

MainGamePanelWizard::MainGamePanelWizard(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(960, 680)) {
    this->SetMinSize(wxSize(960, 680));
}

void MainGamePanelWizard::buildGameState(game_state* gameState, player* me)
{
    this->DestroyChildren();

    // make new sizer
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    // child panel
    wxPanel *panel = new wxPanel(this, wxID_ANY);
    this->SetBackgroundColour(wxColour(100,100,100));

    this->SetMinSize(wxSize(960, 680));

    //create Grid to partition game panel
    auto sizer = new wxGridBagSizer(4,5);
    std::vector<std::pair<wxGBPosition, wxGBSpan>> items  = {
        {{0,0}, {1,1}},
        {{0,1}, {1,1}},
        {{0,2}, {1,1}},
        {{0,3}, {1,1}},
        {{0,4}, {1,1}},
        {{1,0}, {1,1}},
        {{1,1}, {1,3}},
        {{1,4}, {1,1}},
        {{2,0}, {1,5}},
        {{3,0}, {1,1}},
        {{3,1}, {1,1}},
        {{3,2}, {1,1}},
        {{3,3}, {1,1}},
        {{3,4}, {1,1}},
        {{4,0}, {1,5}}
    };

    // set gaps between the panels to 0 explicitly
    //sizer->SetVGap(0);
    //sizer->SetHGap(0);

    // Add other players
    std::vector<player*> players = gameState->get_players();
    int numberOfPlayers = players.size();
    int myPosition = -1;

    // find our player in the list of players
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
    int minWidth = MainGamePanelWizard::panelSize.GetWidth()/5;
    int minHeight = MainGamePanelWizard::panelSize.GetHeight()/5;

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
    for (int col = 0; col < 5; ++col)
    {
        sizer->AddGrowableCol(col, 1);
    }
    // make all rows growable
    for (int row = 0; row < 4; ++row)
    {
        sizer->AddGrowableRow(row, 1);
    }



    // assign sizer to child panel
    panel->SetSizer(sizer);
    // add child panel to MainGamePanelWizard
    mainSizer->Add(panel, 1, wxEXPAND|wxALL,0);
    //assign sizer to MainGamewindow
    this->SetSizer(mainSizer);

    // show whose turn it is
    this->buildTurnIndicator(sizer, gameState, me);

    //show the played cards
    this->buildTrickPile(sizer, gameState, me);

    // show trump card
    this->buildTrumpCard(sizer, gameState);

    // show player
    this->buildThisPlayer(sizer, gameState, me);

    //show other Players
    this->buildOtherPlayers(sizer, gameState, me, myPosition);

    // update Layout
    this->Layout();
}

void MainGamePanelWizard::buildOtherPlayers(wxGridBagSizer* sizer, game_state* gameState, player* me, int myPosition)
{
    std::vector<player*> players = gameState->get_players();
    int numberOfPlayers = players.size();

    std::vector<wxGBPosition> otherPlayerPositions;

    if (numberOfPlayers == 2)
    {
        otherPlayerPositions = { wxGBPosition(0, 2)};
    }
    else if (numberOfPlayers == 3)
    {
        otherPlayerPositions = { wxGBPosition(0, 1),  wxGBPosition(0, 3)};
    }
    else if (numberOfPlayers == 4)
    {
        otherPlayerPositions = { wxGBPosition(0, 2),  wxGBPosition(1, 0),  wxGBPosition(1, 2)};
    }
    else if (numberOfPlayers == 5)
    {
        otherPlayerPositions = { wxGBPosition(0, 1), wxGBPosition(0, 3), wxGBPosition(1, 0), wxGBPosition(1, 2)};
    }
    else if (numberOfPlayers == 6)
    {
        otherPlayerPositions = { wxGBPosition(0, 1), wxGBPosition(0, 2), wxGBPosition(0, 3), wxGBPosition(1, 0), wxGBPosition(1, 2)};
    }

    for (int i = 0; i < otherPlayerPositions.size(); i++)
    {
        wxGBSizerItem* item = sizer->FindItemAtPosition(otherPlayerPositions[i]);
        wxPanel* panel = dynamic_cast<wxPanel*>(item->GetWindow());
        wxBoxSizer* playerSizer_vert = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(playerSizer_vert);

        player* otherPlayer = players.at((myPosition + i) % numberOfPlayers);

        if(!gameState->is_started())
        {
        wxStaticText* playerNameText = new wxStaticText(panel, wxID_ANY, otherPlayer->get_player_name(),wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        playerNameText->SetForegroundColour(*wxWHITE);

        wxStaticText* statusText = new wxStaticText(panel, wxID_ANY, "waiting...",wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        statusText->SetForegroundColour(*wxWHITE);

        playerSizer_vert->Add(playerNameText,0,wxALIGN_CENTER);
        playerSizer_vert->Add(statusText,0,wxALIGN_CENTER);
        }
        else
        {
        wxStaticText* playerNameText = new wxStaticText(panel, wxID_ANY, otherPlayer->get_player_name(),wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        playerNameText->SetForegroundColour(*wxWHITE);

        wxStaticText* statusText = new wxStaticText(panel, wxID_ANY, std::to_string(otherPlayer->get_score()) + " minus points",wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        statusText->SetForegroundColour(*wxWHITE);

        playerSizer_vert->Add(playerNameText,0,wxALIGN_CENTER);
        playerSizer_vert->Add(statusText,0,wxALIGN_CENTER);
        }

    }
}

void MainGamePanelWizard::buildTrumpCard(wxGridBagSizer* sizer, game_state* gameState)
{
    wxGBSizerItem* trumpItem = sizer->FindItemAtPosition(wxGBPosition(3,0));
    wxPanel* trumpPanel = dynamic_cast<wxPanel*>(trumpItem->GetWindow());
    wxBoxSizer* trumpSizer_vert = new wxBoxSizer(wxVERTICAL);
    trumpPanel->SetSizer(trumpSizer_vert);

    if(gameState->is_started())
    {
        wxStaticText* trumpText = new wxStaticText(trumpPanel, wxID_ANY, "TRUMP CARD",wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        trumpText->SetForegroundColour(*wxWHITE);

        const card* trumpCard = gameState->get_discard_pile()->get_top_card();

        std::string cardImage = "assets/wizard_" + std::to_string(trumpCard->get_value()) + ".png";
        ImagePanel* cardPanel = new ImagePanel(trumpPanel, cardImage, wxBITMAP_TYPE_ANY, wxDefaultPosition, MainGamePanelWizard::cardSize);


        trumpSizer_vert->Add(trumpText,0,wxALIGN_CENTER);
        trumpSizer_vert->Add(cardPanel, 0, wxALIGN_CENTER);
    }
}

void MainGamePanelWizard::buildTrickPile(wxGridBagSizer* sizer, game_state* gameState, player *me)
{
    wxGBSizerItem* trickItem = sizer->FindItemAtPosition(wxGBPosition(1,1));
    wxPanel* trickPanel = dynamic_cast<wxPanel*>(trickItem->GetWindow());
    trickPanel->SetBackgroundColour(wxColour(100,100,100));

    // define the sizers for alignment
    auto trickPanelSizer_vert = new wxBoxSizer(wxVERTICAL);
    trickPanel->SetSizer(trickPanelSizer_vert);
    auto trickPanelSizer_hor = new wxBoxSizer(wxHORIZONTAL);
    trickPanelSizer_vert->Add(trickPanelSizer_hor, 1, wxALIGN_CENTER);

    if(gameState->is_started())
    {
        /*
        const card* topCard = gameState->get_discard_pile()->get_top_card();
        const std::vector<card*>* trickCards = gameState->get_discard_pile()->get_cards();
        if(topCard != nullptr) {
            std::string cardImage = "assets/wizard_" + std::to_string(topCard->get_value()) + ".png";


            ImagePanel* discardPile = new ImagePanel(trickPanel, cardImage, wxBITMAP_TYPE_ANY, wxDefaultPosition, MainGamePanelWizard::cardSize);

            trickPanelSizer_hor->Add(discardPile, 0, wxALIGN_CENTER | wxALL);
        }
        */
        const std::vector<card*>* trickCards = gameState->get_discard_pile()->get_cards();
        if (trickCards!= nullptr)
        {
            for (const auto& card : *trickCards)
            {
                std::string cardImage = "assets/wizard_" + std::to_string(card->get_value()) + ".png";
                ImagePanel* cardPanel = new ImagePanel(trickPanel, cardImage, wxBITMAP_TYPE_ANY, wxDefaultPosition, MainGamePanelWizard::cardSize);
                trickPanelSizer_hor->Add(cardPanel, 0, wxALIGN_CENTER | wxALL, -15);
            }
        }

    }
}

void MainGamePanelWizard::buildTurnIndicator(wxGridBagSizer* sizer, game_state* gameState, player* me)
{
    wxGBSizerItem* turnItem = sizer->FindItemAtPosition(wxGBPosition(2,0));
    wxPanel* turnPanel = dynamic_cast<wxPanel*>(turnItem->GetWindow());
    turnPanel->SetMinSize(wxSize(10,20));

    // add sizer to center the text
    auto turnIndicatorPanelSizer_vert = new wxBoxSizer(wxVERTICAL);
    turnPanel->SetSizer(turnIndicatorPanelSizer_vert);
    auto turnIndicatorPanelSizer_hor = new wxBoxSizer(wxHORIZONTAL);
    turnIndicatorPanelSizer_vert->Add(turnIndicatorPanelSizer_hor, 1, wxALIGN_CENTER);

    if(gameState->is_started())
    {
        // display whose turn it is
        std::string turnIndicatorText;
        if (gameState->get_current_player() == me)
        {
            turnIndicatorText = "It is your turn!";
        }
        else
        {
            turnIndicatorText = "It is " + gameState->get_current_player()->get_player_name() + "'s turn!";
        }

        wxStaticText* turnText = new wxStaticText(turnPanel, wxID_ANY, turnIndicatorText,wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        turnText->SetForegroundColour(*wxWHITE);

        turnIndicatorPanelSizer_hor->Add(turnText, 0, wxALIGN_CENTER);
    }

}


void MainGamePanelWizard::buildThisPlayer(wxGridBagSizer* sizer, game_state* gameState, player* me)
{
    wxGBSizerItem* meItem = sizer->FindItemAtPosition(wxGBPosition(3,2));
    wxPanel* mePanel = dynamic_cast<wxPanel*>(meItem->GetWindow());
    mePanel->SetBackgroundColour(wxColour(0,0,0));

    // create sizer to align elements at bottom center
    wxBoxSizer* meSizer_hor = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* meSizer = new wxBoxSizer(wxVERTICAL);
    meSizer_hor->Add(meSizer, 1, wxALIGN_BOTTOM);

    // add player name to the panel
    wxStaticText* playerName = new wxStaticText(mePanel, wxID_ANY, me->get_player_name(),wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    playerName->SetForegroundColour(*wxWHITE);
    meSizer->Add(playerName, 0, wxALIGN_CENTER);

    if(!gameState->is_started())
    {
        // add status text (waiting)
        wxStaticText* playerScore = new wxStaticText(mePanel, wxID_ANY, "waiting for the game to start",wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        playerScore->SetForegroundColour(*wxWHITE);
        meSizer->Add(playerScore, 0, wxALIGN_CENTER);

        // show button that allows our player to start the game
        wxButton* startGameButton = new wxButton(mePanel, wxID_ANY, "Start Game!", wxDefaultPosition, wxSize(80, 32));
        startGameButton->Bind(wxEVT_BUTTON, [](wxCommandEvent& event) {
            GameController::startGame();
        });
        meSizer->Add(startGameButton,0,wxALIGN_CENTER);
    }
    else
    {
        // show minus points instead of status text
        wxStaticText* playerScore = new wxStaticText(mePanel, wxID_ANY, std::to_string(me->get_score()) + " minus points",wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        playerScore->SetForegroundColour(*wxWHITE);
        meSizer->Add(playerScore, 0, wxALIGN_CENTER);


        // add status (there are 3 different cases)
        // we have folded
        if (me->has_folded())
        {
            wxStaticText* playerStatus = new wxStaticText(mePanel, wxID_ANY, "Folded!",wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
            playerStatus->SetForegroundColour(*wxWHITE);
            meSizer->Add(playerStatus, 0, wxALIGN_CENTER);
        }
        // if we havent folded yet and it is our turn display fold button
        else if (gameState->get_current_player() == me)
        {
            wxButton *foldButton = new wxButton(mePanel, wxID_ANY, "Fold", wxDefaultPosition, wxSize(80, 32));
            foldButton->Bind(wxEVT_BUTTON, [](wxCommandEvent& event) {
                GameController::fold();
            });
            meSizer->Add(foldButton,0,wxALIGN_CENTER);
        }
        // it is not our turn
        else
        {
            wxStaticText* playerStatus = new wxStaticText(mePanel, wxID_ANY, "waiting...",wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
            playerStatus->SetForegroundColour(*wxWHITE);
            meSizer->Add(playerStatus, 0, wxALIGN_CENTER);
        }


        // display our hand
        int numberOfCards = me->get_nof_cards();

        if (numberOfCards > 0)
        {
            wxGBSizerItem* cardItem = sizer->FindItemAtPosition(wxGBPosition(4,0));
            wxPanel* cardPanel = dynamic_cast<wxPanel*>(cardItem->GetWindow());

            wxSize scaledCardSize = MainGamePanelWizard::cardSize;

            // define two new sizers to be able to center the cards
            auto cardPanelSizer_vert = new wxBoxSizer(wxVERTICAL);
            cardPanel->SetSizer(cardPanelSizer_vert);
            auto cardPanelSizer_hor = new wxBoxSizer(wxHORIZONTAL);
            cardPanelSizer_vert->Add(cardPanelSizer_hor, 1, wxALIGN_CENTER);

            if (numberOfCards * (MainGamePanelWizard::cardSize.GetWidth() + 8) > MainGamePanelWizard::panelSize.GetWidth())
            {
                int scaledCardWidth = panelSize.GetWidth() / numberOfCards - 8;
                double cardAspectRatio = (double) cardSize.GetHeight() / (double) cardSize.GetWidth();
                int scaledCardHeight = (int) ((double) scaledCardWidth * cardAspectRatio);
                scaledCardSize = wxSize(scaledCardWidth, scaledCardHeight);
            }

            // Show all cards
            for (int i = 0; i < me->get_hand()->get_cards().size(); i++) {

                card *handCard = me->get_hand()->get_cards().at(i);
                std::string cardFile = "assets/wizard_" + std::to_string(handCard->get_value()) + ".png";

                ImagePanel *cardButton = new ImagePanel(cardPanel, cardFile, wxBITMAP_TYPE_ANY, wxDefaultPosition, scaledCardSize);

                if (gameState->get_current_player() == me && !me->has_folded()) {
                    cardButton->SetToolTip("Play card");
                    cardButton->SetCursor(wxCursor(wxCURSOR_HAND));
                    cardButton->Bind(wxEVT_LEFT_UP, [handCard](wxMouseEvent& event) {
                        GameController::playCard(handCard);
                    });
                }
                cardPanelSizer_hor->Add(cardButton, 0, wxALIGN_TOP | wxALL, 4);
            }
        }

    }

    mePanel->SetSizer(meSizer_hor);
}
