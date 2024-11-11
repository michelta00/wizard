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

    // Set background
    wxColor lightBlue = wxColor(102, 0, 51);
    this->SetBackgroundColour(lightBlue);

    // Set the minimum size of the window. The user won't be able to resize the window to a size smaller than this
    this->SetMinSize(wxSize(1000, 720));

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

