#ifndef TEST_CONNECTIONPANEL_H
#define TEST_CONNECTIONPANEL_H

#include <wx/wx.h>
#include "../uiElements/InputField.h"

/**
 * @brief A custom wxPanel for handling server connection input and UI.
* This panel provides fields for entering the server address, port, and player name.
 * It also includes a background image and a "Connect" button.
 */
class ConnectionPanel : public wxPanel {

public:
    /**
     * @brief Constructs Connection Panel
     * @param parent Pointer to the parent wxWindow that contains this window
     */
    ConnectionPanel(wxWindow* parent);

    /**
     * @brief Retrieves the server address entered by the user (default is set to 127.0.0.1).
     * @return The server address as a wxString.
     */
    wxString getServerAddress();
    /**
     * @brief Retrieves the server port entered by the user (default is set to 50505).
     * @return The server port as a wxString.
     */
    wxString getServerPort();
    /**
    * @brief Retrieves the player name entered by the user.
    * The player name is entered in an input field and is used for gameplay identification.
    * @return The player name as a wxString.
    */
    wxString getPlayerName();

private:
    /**
     * @brief Handles the paint event to draw the background image.
     * @param event The wxPaintEvent triggered by the paint action.
     */
    void OnPaint(wxPaintEvent& event);
    /**
      * @brief Handles the resize event to adjust layout and widget sizes.
      * @param event The wxSizeEvent triggered by the resize action.
      */
    void OnSize(wxSizeEvent& event);

    wxBitmap _backgroundImage; ///< Bitmap for the background image of the panel
    InputField* _serverAddressField; ///< Input field for enterng the server address
    InputField* _serverPortField; ///< Input field for entering the server port
    InputField* _playerNameField; ///< Input field for entering the player name

};


#endif //TEST_CONNECTIONPANEL_H
