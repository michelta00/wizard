//
// Created by hannah on 11/9/24.
//

#ifndef ERRORDIALOG_H
#define ERRORDIALOG_H

#include <wx/wx.h>
#include <wx/statbmp.h>

class ErrorDialog : public wxDialog{
    public: ErrorDialog(wxWindow* parent, const std::string& title, const std::string& message, const wxBitmap& image);

    private:
        wxStaticBitmap* imageCtrl;
        wxStaticText* messageCtrl;
        wxButton* okButton;

};

#endif //ERRORDIALOG_H
