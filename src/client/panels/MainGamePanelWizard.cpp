#include "MainGamePanelWizard.h"
#include "../GameController.h"
#include "../uiElements/ImagePanel.h"
#include <wx/gbsizer.h>
#include <wx/grid.h>
#include "../messageBoxes/ScoreBoardDialog.h"



// font for buttons
wxFont magicalFont = wxFont(wxFontInfo(20).FaceName("Magic School One"));

//font for general text
wxFont regularFont = wxFont(wxFontInfo(12).FaceName("Junicode")); //requires yy:20

//font for player names
wxFont regularFontBig = wxFont(wxFontInfo(16).FaceName("Junicode")); //requires yy:35, xx:150

//font for round announcement
wxFont magicalFontGigantic = wxFont(wxFontInfo(70).FaceName("Magic School One"));


MainGamePanelWizard::MainGamePanelWizard(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(960, 680)) {
    this->SetMinSize(wxSize(1000, 680));
}


void MainGamePanelWizard::buildGameState(game_state* gameState, player* me)
{
    this->DestroyChildren();

    // make new sizer
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    // child panel
    wxPanel *panel = new wxPanel(this, wxID_ANY);
    this->SetBackgroundColour(wxColour(102,0,51));

    /*
    // access the main window to add a button
    wxFrame* parentFrame = dynamic_cast<wxFrame*>(this->GetParent());
    if (parentFrame && parentFrame->GetStatusBar()) {
        wxStatusBar *statusBar = parentFrame->GetStatusBar();

        // Retrieve the existing sizer
        wxSizer *statusSizer = statusBar->GetSizer();
        if (statusSizer) {
            wxButton* scoreBoardButton = new wxButton(statusBar, wxID_ANY, "ScoreBoard");
            statusSizer->Add(scoreBoardButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
            // Refresh the layout
            statusBar->Layout();
        }
    }

    // put all player scores in a vector
    std::vector<player*> players = gameState->get_players();
    int numberOfPlayers = players.size();
    std::vector<std::vector<int>> tableData(numberOfPlayers);

    for (int i = 0; i < numberOfPlayers; i++){
        tableData.at(i) = players.at(i)->get_scores();
    }
    */



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
    int minWidth = MainGamePanelWizard::panelSize.GetWidth()/5 - 10;
    int minHeight = MainGamePanelWizard::panelSize.GetHeight()/5 - 10;

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

    // show button to display score board
    this->buildScoreLeaveButtons(sizer, gameState);

    // update Layout
    this->Layout();
}

void MainGamePanelWizard::buildScoreLeaveButtons(wxGridBagSizer *sizer, game_state* gameState) {
    wxGBSizerItem* item = sizer->FindItemAtPosition(wxGBPosition(3,3));
    wxPanel* panel = dynamic_cast<wxPanel*>(item->GetWindow());

    wxBoxSizer *sizer_vert = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(sizer_vert);
    auto sizer_hor = new wxBoxSizer(wxHORIZONTAL);
    sizer_vert->Add(sizer_hor, 1, wxALIGN_LEFT);

    if (gameState->is_started())
    {
        wxButton *scoreBoardButton = new wxButton(panel, wxID_ANY, "ScoreBoard");
        scoreBoardButton->SetMinSize(wxSize(110, 43)); //90 , 35
        sizer_vert->Add(scoreBoardButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

        scoreBoardButton->SetFont(magicalFont);
        scoreBoardButton->SetForegroundColour(wxColour(225, 225, 225)); // Set button text color
        scoreBoardButton->SetBackgroundColour(wxColour(50, 0, 51)); //same shade of purple as start game button and estimation panel

        scoreBoardButton->Bind(wxEVT_BUTTON, [gameState](wxCommandEvent &event) {
            ScoreBoardDialog scoreBoard(nullptr, "ScoreBoard", "Here will be the scoreboard", gameState);
            scoreBoard.ShowModal();
        });
    }
        wxButton *leaveGameButton = new wxButton(panel, wxID_ANY, "Leave Game");
        leaveGameButton->SetMinSize(wxSize(110, 43));
        sizer_vert->Add(leaveGameButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

        leaveGameButton->SetFont(magicalFont);
        leaveGameButton->SetBackgroundColour(wxColour(50,0,51));  // Set background color to blue
        leaveGameButton->SetForegroundColour(*wxWHITE);

        leaveGameButton->Bind(wxEVT_BUTTON, [gameState](wxCommandEvent &event) {
            GameController::leaveGame();
        });
}

/*
void MainGamePanelWizard::buildLeaveGameButton(wxGridBagSizer *sizer, game_state* gameState) {
    wxGBSizerItem* item = sizer->FindItemAtPosition(wxGBPosition(3,3));
    wxPanel* panel = dynamic_cast<wxPanel*>(item->GetWindow());

    if (gameState->is_started()) {
        wxSizer* sizer_vert = panel->GetSizer();
        // wxBoxSizer *sizer_vert = new wxBoxSizer(wxVERTICAL);
        // panel->SetSizer(sizer_vert);

        wxButton *leaveGameButton = new wxButton(panel, wxID_ANY, "Leave Game");
        sizer_vert->Add(leaveGameButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);

        leaveGameButton->Bind(wxEVT_BUTTON, [gameState](wxCommandEvent &event) {
            GameController::leaveGame();
        });
    }

}
*/
void MainGamePanelWizard::buildOtherPlayers(wxGridBagSizer* sizer, game_state* gameState, player* me, int myPosition)
{
    std::vector<player*> players = gameState->get_players();
    int numberOfPlayers = players.size();

    std::vector<wxGBPosition> otherPlayerPositions;

    if (numberOfPlayers == 2) {
        otherPlayerPositions = { wxGBPosition(0, 2)};
    }
    else if (numberOfPlayers == 3)
    {
        otherPlayerPositions = { wxGBPosition(0, 1),  wxGBPosition(0, 3)};
    }
    else if (numberOfPlayers == 4)
    {
        otherPlayerPositions = { wxGBPosition(0, 2),  wxGBPosition(1, 0),  wxGBPosition(1, 4)};
    }
    else if (numberOfPlayers == 5)
    {
        otherPlayerPositions = { wxGBPosition(0, 1), wxGBPosition(0, 3), wxGBPosition(1, 0), wxGBPosition(1, 4)};
    }
    else if (numberOfPlayers == 6)
    {
        otherPlayerPositions = { wxGBPosition(0, 1), wxGBPosition(0, 2), wxGBPosition(0, 3), wxGBPosition(1, 0), wxGBPosition(1, 4)};
    }

    for (int i = 0; i < otherPlayerPositions.size(); i++)
    {
        // sizer to put text at center top
        wxGBSizerItem* item = sizer->FindItemAtPosition(otherPlayerPositions[i]);
        wxPanel* panel = dynamic_cast<wxPanel*>(item->GetWindow());
        wxBoxSizer* playerSizer_vert = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(playerSizer_vert);

        panel->SetBackgroundColour(wxColour(120,0,51));

        // get other player
        player* otherPlayer = players.at((myPosition + i + 1) % numberOfPlayers);

        // Lobby: display names
        if(!gameState->is_started())
        {
        wxStaticText* playerNameText = new wxStaticText(panel, wxID_ANY, otherPlayer->get_player_name(),wxDefaultPosition, wxSize(150, 35), wxALIGN_CENTER);
        playerNameText->SetForegroundColour(*wxWHITE);
        playerNameText->SetFont(regularFontBig);


        wxStaticText* statusText = new wxStaticText(panel, wxID_ANY, "waiting...",wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        statusText->SetForegroundColour(*wxWHITE);
        statusText->SetFont(regularFont);
        playerSizer_vert->Add(playerNameText,0,wxALIGN_CENTER|wxTOP, 5);
        playerSizer_vert->Add(statusText,0,wxALIGN_CENTER);
        }
        // game started: display names, predicted and scored tricks
        else
        {
            // 15 characters for player name
        wxStaticText* playerNameText = new wxStaticText(panel, wxID_ANY, otherPlayer->get_player_name(),wxDefaultPosition, wxSize(150, 35), wxALIGN_CENTER);
        playerNameText->SetForegroundColour(*wxWHITE);
        playerNameText->SetFont(regularFontBig);

        wxStaticText* trickText = new wxStaticText(panel, wxID_ANY, std::to_string(otherPlayer->get_nof_tricks()) + "/" +  std::to_string(otherPlayer->get_nof_predicted()) + " Tricks",wxDefaultPosition, wxSize(100, 20), wxALIGN_CENTER);
        trickText->SetForegroundColour(*wxWHITE);
        trickText->SetFont(regularFont);
        playerSizer_vert->Add(playerNameText,0,wxALIGN_CENTER|wxTOP,5);
        playerSizer_vert->Add(trickText,0,wxALIGN_CENTER);
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
        trumpText->SetFont(regularFont);
        int trumpColor = gameState->get_trump_color();
        int trumpCardValue = gameState->get_trump_card_value();

        std::string cardImage = "assets/card_" + std::to_string(trumpCardValue) + "_" + std::to_string(trumpColor)+".png";
        std::cout << cardImage << std::endl;
        ImagePanel* cardPanel = new ImagePanel(trumpPanel, cardImage, wxBITMAP_TYPE_ANY, wxDefaultPosition, MainGamePanelWizard::cardSize);


        trumpSizer_vert->Add(trumpText,0,wxALIGN_CENTER);
        trumpSizer_vert->Add(cardPanel, 0, wxALIGN_CENTER);
    }
}

void MainGamePanelWizard::buildTrickPile(wxGridBagSizer* sizer, game_state* gameState, player *me)
{
    wxGBSizerItem* trickItem = sizer->FindItemAtPosition(wxGBPosition(1,1));
    wxPanel* trickPanel = dynamic_cast<wxPanel*>(trickItem->GetWindow());

    // define the sizers for alignment
    auto trickPanelSizer_vert = new wxBoxSizer(wxVERTICAL);
    trickPanel->SetSizer(trickPanelSizer_vert);
    auto trickPanelSizer_hor = new wxBoxSizer(wxHORIZONTAL);
    trickPanelSizer_vert->Add(trickPanelSizer_hor, 1, wxALIGN_CENTER);

    if(gameState->is_started())
    {
        trickPanel->SetBackgroundColour(wxColour(100,100,100));
        const std::vector<std::pair<card*, player*>> trickCards = gameState->get_trick()->get_cards_and_players();
            for (const auto& it : trickCards)
            {
                card* card = it.first;
                std::string cardImage = "assets/card_" + std::to_string(card->get_value()) + "_" + std::to_string(card->get_color()) + ".png";
                ImagePanel* cardPanel = new ImagePanel(trickPanel, cardImage, wxBITMAP_TYPE_ANY, wxDefaultPosition, MainGamePanelWizard::cardSize);
                trickPanelSizer_hor->Add(cardPanel, 0, wxALIGN_CENTER | wxALL, -10);
            }
    }
    // if game has not started yet display picture in the lobby
    else{
        std::string wizardLogoImage = "assets/Wizard_round.png";
        ImagePanel* wizardLogo = new ImagePanel(trickPanel, wizardLogoImage, wxBITMAP_TYPE_ANY, wxDefaultPosition, wxSize(130,116));
        trickPanelSizer_hor->Add(wizardLogo, 0, wxALIGN_CENTER );
    }
}
/*
// TODO: round announcement shown at the beginning of every new round
void MainGamePanelWizard::showRoundOverlay(int roundNumber) { //obtain round number with std::to_string(gameState->get_round_number()+1)
    // Create a panel to act as the overlay
    wxPanel* overlayPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, this->GetSize()); //create panel the size of the main game panel
    overlayPanel->SetBackgroundColour(wxColour(0, 0, 0, 70)); // make current game panel opaque

    // Create the text
    wxString roundText = wxString::Format("ROUND %d", roundNumber);
    wxStaticText* roundLabel = new wxStaticText(overlayPanel, wxID_ANY, roundText, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    roundLabel->SetFont(magicalFontGigantic);
    roundLabel->SetForegroundColour(*wxWHITE);

    // center text
    wxBoxSizer* overlaySizer = new wxBoxSizer(wxVERTICAL);
    overlaySizer->AddStretchSpacer(1);
    overlaySizer->Add(roundLabel, 0, wxALIGN_CENTER_HORIZONTAL);
    overlaySizer->AddStretchSpacer(1);

    overlayPanel->SetSizer(overlaySizer);

    // Show the overlay
    overlayPanel->Raise();
    overlayPanel->Show();

    // Set a timer to remove the overlay after 1 second
    wxTimer* timer = new wxTimer(this, wxID_ANY);
    timer->Bind(wxEVT_TIMER, [this, overlayPanel](wxTimerEvent& event) {
        overlayPanel->Destroy(); // Remove the overlay panel
    });
    timer->Start(1000, wxTIMER_ONE_SHOT); // 1-second delay
}
*/

void MainGamePanelWizard::buildTurnIndicator(wxGridBagSizer* sizer, game_state* gameState, player* me)
{
    wxGBSizerItem* turnItem = sizer->FindItemAtPosition(wxGBPosition(2,0));
    wxPanel* turnPanel = dynamic_cast<wxPanel*>(turnItem->GetWindow());
    turnPanel->SetMinSize(wxSize(50,25)); //make turn display panel a bit more wide

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

        wxStaticText* turnText = new wxStaticText(turnPanel, wxID_ANY, turnIndicatorText,wxDefaultPosition, wxSize(270, 50), wxALIGN_CENTER);
            // width 12 + 15 player name --> 27 characters
        turnText->SetForegroundColour(*wxWHITE);
        turnText->SetFont(regularFontBig);
        turnIndicatorPanelSizer_hor->Add(turnText, 0, wxALIGN_CENTER);
    }

}


void MainGamePanelWizard::buildThisPlayer(wxGridBagSizer* sizer, game_state* gameState, player* me)
{
    wxGBSizerItem* meItem = sizer->FindItemAtPosition(wxGBPosition(3,2));
    wxPanel* mePanel = dynamic_cast<wxPanel*>(meItem->GetWindow());
    mePanel->SetBackgroundColour(wxColour(120,0,51));

    // create sizer to align elements at bottom center
    wxBoxSizer* meSizer_hor = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* meSizer = new wxBoxSizer(wxVERTICAL);
    meSizer_hor->Add(meSizer, 1, wxALIGN_BOTTOM);

    // add player name to the panel
    wxStaticText* playerName = new wxStaticText(mePanel, wxID_ANY, me->get_player_name(),wxDefaultPosition, wxSize(mePanel->GetMinSize().GetWidth(), 35), wxALIGN_CENTER);
    playerName->SetForegroundColour(*wxWHITE);
    playerName->SetFont(regularFontBig);


    meSizer->Add(playerName, 0, wxALIGN_CENTER);

    if(!gameState->is_started())
    {
        // add status text (waiting)
        wxStaticText* playerScore = new wxStaticText(mePanel, wxID_ANY, "Waiting for the game to start",wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        //playerScore->SetFont(regularFont);
        playerScore->SetForegroundColour(*wxWHITE);
        playerScore->SetFont(regularFont);
        meSizer->Add(playerScore, 0, wxALIGN_CENTER|wxALL,5);

        // show button that allows our player to start the game
        wxButton* startGameButton = new wxButton(mePanel, wxID_ANY, "Start Game!", wxDefaultPosition, wxSize(110, 43)); //110 pixels wide, 43 high
        startGameButton->SetFont(magicalFont);

        startGameButton->Bind(wxEVT_BUTTON, [](wxCommandEvent& event) {
            GameController::startGame();
        });

        // change color of the button as soon as there are 3 players
        std::vector<player*> players = gameState->get_players();
        int numberOfPlayers = players.size();

        if (numberOfPlayers >= 3) {
            //make button same as connect button once enough players
            startGameButton->SetForegroundColour(wxColour(225, 225, 225)); // Set button text color
            startGameButton->SetBackgroundColour(wxColour(50, 0, 51));    //make button same purple as estimation panel once clickable
        }

        meSizer->Add(startGameButton,0,wxALIGN_CENTER|wxALL, 5);
    }
    else
    {
        // show estimated and scored tricks instead of status text
        wxStaticText* playerScore = new wxStaticText(mePanel, wxID_ANY, std::to_string(me->get_nof_tricks()) + "/" +  std::to_string(me->get_nof_predicted()) + " Tricks",wxDefaultPosition, wxSize(100, 20), wxALIGN_CENTER);
        playerScore->SetForegroundColour(*wxWHITE);
        playerScore->SetFont(regularFont);
        meSizer->Add(playerScore, 0, wxALIGN_CENTER);

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

                std::string cardFile = "assets/card_" + std::to_string(handCard->get_value()) + "_" + std::to_string(handCard->get_color())  + ".png";

                ImagePanel *cardButton = new ImagePanel(cardPanel, cardFile, wxBITMAP_TYPE_ANY, wxDefaultPosition, scaledCardSize);

                if (gameState->get_current_player() == me && gameState->is_estimation_phase() == false) {
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