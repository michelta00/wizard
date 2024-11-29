//
// Created by hannah on 11/11/24.
//

#ifndef SCOREDIALOG_H
#define SCOREDIALOG_H

#include <wx/wx.h>
#include <wx/timer.h>
#include <string>

class ScoreDialog : public wxDialog{
public:
    ScoreDialog(wxWindow* parent, const std::string& title, const std::string& message);
    ~ScoreDialog() = default;
private:
    wxTimer _closeTimer;

    void OnTimerClose(wxTimerEvent& event);

    wxDECLARE_EVENT_TABLE();  // For handling wxWidgets events
};



#endif
