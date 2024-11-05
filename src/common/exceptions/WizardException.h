//
// Created by Manuel on 09.03.2021.
//

#ifndef WIZARD_WIZARDEXCEPTION_H
#define WIZARD_WIZARDEXCEPTION_H

#include <string>

class WizardException : public std::exception {
private:
    std::string _msg;
public:
    explicit WizardException(const std::string& message) : _msg(message) { };

    const char* what() const noexcept override {
        return _msg.c_str();
    }
};

#endif //WIZARD_WIZARDEXCEPTION_H
