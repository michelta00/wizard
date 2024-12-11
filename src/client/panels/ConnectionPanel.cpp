#include "ConnectionPanel.h"

#include <wx/image.h>
#include "../uiElements/ImagePanel.h"
#include "../GameController.h"
#include <wx/dcbuffer.h>

ConnectionPanel::ConnectionPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {

    // font for buttons
    wxFont magicalFont = wxFont(wxFontInfo(20).FaceName("Magic School One"));
    //font for regular text
    wxFont regularFont = wxFont(wxFontInfo(12).FaceName("Junicode"));
    // Load the background image
    _backgroundImage.LoadFile("./assets/Wizard_big.png", wxBITMAP_TYPE_ANY);

    // Set background paint style and bind paint/resize events
    this->SetBackgroundStyle(wxBG_STYLE_PAINT);
    this->Bind(wxEVT_PAINT, &ConnectionPanel::OnPaint, this);
    this->Bind(wxEVT_SIZE, &ConnectionPanel::OnSize, this);

    // Main vertical layout for input fields and button
    wxBoxSizer* verticalLayout = new wxBoxSizer(wxVERTICAL);

    int fieldSpacing = 12; // Space between input fields and button

    // Default values for server host and port
    wxString default_server_host = "127.0.0.1";
    int default_port = 50505;

    // Server Address Input Field
    this->_serverAddressField = new InputField(this, "Server address:", 100, default_server_host, 240); //field width is the width of the white text field
    //label width is the width of the label text
    this->_serverAddressField->SetFont(regularFont); //set font
    this->_serverAddressField->SetLabelTextColour(wxColour(255, 255, 255)); // Set label text color to white
    this->_serverAddressField->SetMinSize(wxSize(-1, 27));
    verticalLayout->Add(this->_serverAddressField, 0, wxALL | wxEXPAND, fieldSpacing);



    // Server Port Input Field
    this->_serverPortField = new InputField(this, "Server port:", 100, wxString::Format("%i", default_port), 240);
    this->_serverPortField->SetFont(regularFont);
    this->_serverPortField->SetLabelTextColour(wxColour(255, 255, 255)); // Set label text color to white
    this->_serverPortField->SetMinSize(wxSize(-1, 27));
    verticalLayout->Add(this->_serverPortField, 0, wxALL | wxEXPAND, fieldSpacing);

    // Player Name Input Field
    this->_playerNameField = new InputField(this, "Player name:", 100, "", 240);
    this->_playerNameField->SetFont(regularFont);
    this->_playerNameField->SetLabelTextColour(wxColour(255, 255, 255)); // Set label text color to white
    this->_playerNameField->SetMinSize(wxSize(-1, 27));
    verticalLayout->Add(this->_playerNameField, 0, wxALL | wxEXPAND, fieldSpacing);

    //TODO: fix spacing between the three lines

    // Connect Button with custom style
    wxButton* connectButton = new wxButton(this, wxID_ANY, "Connect", wxDefaultPosition, wxSize(80, 37)); //changed size to make margins between text and button smaller
    connectButton->SetFont(magicalFont);
    connectButton->SetForegroundColour(wxColour(225, 225, 225)); // Set button text color
    connectButton->SetBackgroundColour(wxColour(102, 0, 51));    // Set button background color
    connectButton->SetWindowStyleFlag(wxBORDER_SIMPLE);          // Set border style
    connectButton->Bind(wxEVT_BUTTON, [](wxCommandEvent& event) {
        GameController::connectToServer(); // Call the connect method when clicked
    });
    verticalLayout->Add(connectButton, 0, wxALIGN_CENTER | wxALL, fieldSpacing);

    // Create a centered layout to ensure everything is vertically and horizontally centered
    wxBoxSizer* centeredLayout = new wxBoxSizer(wxVERTICAL);
    centeredLayout->AddStretchSpacer(1); // Add flexible space above the vertical layout
    centeredLayout->Add(verticalLayout, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
    centeredLayout->AddStretchSpacer(1); // Add flexible space below the vertical layout

    // Set the centered layout as the sizer for this panel
    this->SetSizer(centeredLayout);
}

void ConnectionPanel::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    wxSize panelSize = this->GetClientSize();

    if (_backgroundImage.IsOk()) {
        // Scale the background image to match the panel size
        wxImage image = _backgroundImage.ConvertToImage();
        wxImage scaledImage = image.Scale(panelSize.GetWidth(), panelSize.GetHeight(), wxIMAGE_QUALITY_HIGH);
        wxBitmap scaledBitmap(scaledImage);

        // Draw the scaled background image
        dc.DrawBitmap(scaledBitmap, 0, 0, true);
    } else {
        // Fallback: Draw a light blue background if the image is not loaded
        dc.SetBrush(wxBrush(wxColour(200, 200, 255)));
        dc.DrawRectangle(wxPoint(0, 0), panelSize);
    }
}

void ConnectionPanel::OnSize(wxSizeEvent& event) {
    wxSize panelSize = this->GetClientSize();

    // Update the layout to adjust widget positions and sizes
    this->Layout();
    event.Skip();   // Pass the resize event to allow further processing
}

// Retrieve the server address entered by the user
wxString ConnectionPanel::getServerAddress() {
    return this->_serverAddressField->getValue();
}

// Retrieve the server port entered by the user
wxString ConnectionPanel::getServerPort() {
    return this->_serverPortField->getValue();
}

// Retrieve the player name entered by the user
wxString ConnectionPanel::getPlayerName() {
    return this->_playerNameField->getValue();
}