//
// Created by martinalavanya on 09.11.24.
//

#ifndef ESTIMATE_TRICKS_REQUEST_H
#define ESTIMATE_TRICKS_REQUEST_H
#include <string>
#include "client_request.h"
#include "../../../../rapidjson/include/rapidjson/document.h"



class estimate_tricks_request : public client_request {

private:
    int _estimate_tricks;

    /*
     * Private constructor for deserialization
     */
    estimate_tricks_request(base_class_properties, int val);

public:

    [[nodiscard]] int get_trick_estimate() const { return this->_estimate_tricks; }

    estimate_tricks_request(std::string game_id, std::string player_id, int estimate_tricks = 1);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static estimate_tricks_request* from_json(const rapidjson::Value& json);
};



#endif //ESTIMATE_TRICKS_REQUEST_H