#include "InputField.h"


InputField::InputField(wxWindow* parent, const wxString& labelText, int labelWidth, const wxString& fieldValue, int fieldWidth)
    : wxPanel(parent, wxID_ANY) {

    wxBoxSizer* horizontalLayout = new wxBoxSizer(wxHORIZONTAL);

    this->_label = new wxStaticText(
            this, // parent element
            wxID_ANY, // element id
            labelText, // text that's displayed as label
            wxDefaultPosition, // position
            wxSize(labelWidth, 40)
     );
    horizontalLayout->Add(this->_label, 0, wxALIGN_CENTER);

    this->_field = new wxTextCtrl(
            this, // parent element
            wxID_ANY, // element id
            fieldValue, // default value
            wxDefaultPosition, // position
            wxSize(fieldWidth, 40)
    );
    horizontalLayout->Add(this->_field, 0, wxALIGN_CENTER);

    this->SetSizerAndFit(horizontalLayout);
}
void InputField::SetLabelTextColour(const wxColour& colour) {
    _label->SetForegroundColour(colour);
    _label->Refresh();
}


wxString InputField::getValue() {
    return this->_field->GetValue();

}
