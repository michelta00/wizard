#ifndef WIZARDUI_GAMEWINDOW_H
#define WIZARDUI_GAMEWINDOW_H

#include "../panels/ConnectionPanel.h"


class GameWindow : public wxFrame
{
public:
    GameWindow(const wxString& title, const wxPoint& pos, const wxSize& size);

    void showPanel(wxPanel* panel);
    void setStatus(const std::string& message);

private:
    wxBoxSizer* _mainLayout;
    wxStatusBar* _statusBar;
    wxButton* _rulesButton;
    wxButton* _settingsButton;

    wxPanel* _currentPanel;

    void showRules(wxCommandEvent& event);
    void show_settings(wxCommandEvent& event);
};


#endif //WIZARDUI_GAMEWINDOW_H
