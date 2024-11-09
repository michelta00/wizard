//
// Created by hannah on 11/9/24.
//

#include "ErrorDialog.h"

ErrorDialog::ErrorDialog(wxWindow* parent, const std::string& title, const std::string& message, const wxBitmap& image)
    : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxSize(400, 250), wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP) {

    // Set custom background color (for example, red for error dialogs)
    SetBackgroundColour(*wxBLACK);

    // Create a vertical sizer for layout
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Display the custom image (if it’s valid)
    if (image.IsOk()) {
        wxImage resizedImage = image.ConvertToImage();
        resizedImage.Rescale(100, 100);

        imageCtrl = new wxStaticBitmap(this, wxID_ANY, wxBitmap(resizedImage));
        sizer->Add(imageCtrl, 0, wxALIGN_CENTER | wxALL, 20);
    }

    // Display the error message
    messageCtrl = new wxStaticText(this, wxID_ANY, message);
    messageCtrl->SetForegroundColour(*wxWHITE);  // Set text color to white for better contrast
    sizer->Add(messageCtrl, 0, wxALIGN_CENTER | wxALL, 10);

    // Add an OK button to close the dialog
    okButton = new wxButton(this, wxID_OK, "OK");
    sizer->Add(okButton, 0, wxALIGN_CENTER | wxALL, 10);

    SetSizerAndFit(sizer);
    Centre();  // Center the dialog on the screen
}