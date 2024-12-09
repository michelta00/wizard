//
// Created by martinalavanya on 09.11.24.
//

#include "estimate_tricks_request.h"

#ifdef WIZARD_SERVER
#include "../../../server/game_instance_manager.h"
#include "../../../server/game_instance.h"
#endif

// Public constructor
estimate_tricks_request::estimate_tricks_request(std::string game_id, std::string player_id, int estimate_tricks)
        : client_request( client_request::create_base_class_properties(RequestType::estimate_tricks, uuid_generator::generate_uuid_v4(), player_id, game_id) )
{
    _estimate_tricks = estimate_tricks;
}

// private constructor for deserialization
estimate_tricks_request::estimate_tricks_request(client_request::base_class_properties props, int estimate_tricks) :
        client_request(props),
        _estimate_tricks(estimate_tricks)
{ }

estimate_tricks_request* estimate_tricks_request::from_json(const rapidjson::Value &json) {
    base_class_properties props = client_request::extract_base_class_properties(json);
    if (json.HasMember("estimate_tricks") ) {
        return new estimate_tricks_request(props, json["estimate_tricks"].GetInt());
    } else {
        throw WizardException("Could not find 'estimate_tricks' in estimate_tricks_request");
    }
}

void estimate_tricks_request::write_into_json(rapidjson::Value &json,
                                        rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    client_request::write_into_json(json, allocator);
    json.AddMember("estimate_tricks", rapidjson::Value(this->_estimate_tricks),allocator);
}

