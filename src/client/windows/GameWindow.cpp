#include "GameWindow.h"


GameWindow::GameWindow(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(nullptr, wxID_ANY, title, pos, size)
{
    // Set up layout that will contain and center all content
    this->_mainLayout = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* outerLayout = new wxBoxSizer(wxHORIZONTAL);
    outerLayout->Add(this->_mainLayout, 1, wxEXPAND | wxALL, 20);
    this->SetSizerAndFit(outerLayout);
    this->_currentPanel = nullptr;

    // Set up status bar
    this->_statusBar = this->CreateStatusBar(1);

    // Add "Rules" button directly to the status bar
    _rulesButton = new wxButton(_statusBar, wxID_ANY, "Rules");
    //_settingsButton = new wxButton(_statusBar, wxID_ANY, "Settings");

    // Set minimum height of status bar based on the button's height with padding
    _statusBar->SetMinHeight(_rulesButton->GetSize().GetHeight() + 10);

    // Create a horizontal sizer for the status bar and add the button to it
    wxBoxSizer* statusSizer = new wxBoxSizer(wxHORIZONTAL);

    // Add stretchable space to fill space in statusbar
    statusSizer->AddStretchSpacer(1);

    //statusSizer->Add(_settingsButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    statusSizer->Add(_rulesButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    _statusBar->SetSizer(statusSizer);
    _statusBar->Layout();

    // Bind the button click event to showRules method
    _rulesButton->Bind(wxEVT_BUTTON, &GameWindow::showRules, this);

    // Set background
    wxColor lightBlue = wxColor(102, 0, 51);
    this->SetBackgroundColour(lightBlue);

    this->SetMinSize(wxSize(1200, 850));

}


void GameWindow::showPanel(wxPanel* panel) {

    // if we are already showing the panel, we don't need to do anything
    if(this->_currentPanel == panel) {
        return;
    }

    // remove previous panel
    if(this->_currentPanel != nullptr) {
        this->_mainLayout->Detach(this->_currentPanel);
        this->_currentPanel->Show(false);
        this->_currentPanel = nullptr;
    }

    // add new panel
    this->_mainLayout->Add(panel, 1, wxEXPAND | wxALL, 20); // Verwenden von wxEXPAND
    panel->Show(true);
    this->_currentPanel = panel;

    // update layout
    this->_mainLayout->Layout();

    // update window size
    this->Fit();
}


void GameWindow::setStatus(const std::string& message) {
    this->_statusBar->SetStatusText(message, 0);
}

void GameWindow::showRules(wxCommandEvent& event) {
    wxString pdfPath = "../assets/Rules.pdf";

    if (wxFileExists(pdfPath)) {
        if (!wxLaunchDefaultApplication(pdfPath)) {
            wxMessageBox("Das PDF konnte nicht geöffnet werden.", "Fehler", wxICON_ERROR);
        }
    } else {
        wxMessageBox("Die Datei 'Rules.pdf' wurde nicht gefunden!", "Fehler", wxICON_ERROR);
    }
}
