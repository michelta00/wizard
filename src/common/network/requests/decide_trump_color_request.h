//
// Created by Manuel on 29.01.2021.
//

#ifndef WIZARD_DECIDE_TRUMP_COLOR_REQUEST_H
#define WIZARD_DECIDE_TRUMP_COLOR_REQUEST_H

#include <string>
#include "client_request.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

class decide_trump_color_request : public client_request {

private:
    int _trump_color;

    /*
     * Private constructor for deserialization
     */
    decide_trump_color_request(base_class_properties, int val);

public:

    [[nodiscard]] int get_trump_color() const { return this->_trump_color; }

    decide_trump_color_request(std::string game_id, std::string player_id, int trump_color = 1); // why default value of 1??
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static decide_trump_color_request* from_json(const rapidjson::Value& json);
};


#endif //WIZARD_DECIDE_TRUMP_COLOR_REQUEST_H