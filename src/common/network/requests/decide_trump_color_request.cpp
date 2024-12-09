//
// Created by martinalavanya on 09.11.24.
//

#include "decide_trump_color_request.h"

#ifdef WIZARD_SERVER
#include "../../../server/game_instance_manager.h"
#include "../../../server/game_instance.h"
#endif

// Public constructor
decide_trump_color_request::decide_trump_color_request(std::string game_id, std::string player_id, int trump_color)
        : client_request( client_request::create_base_class_properties(RequestType::decide_trump_color, uuid_generator::generate_uuid_v4(), player_id, game_id) )
{
    _trump_color = trump_color;
}

// private constructor for deserialization
decide_trump_color_request::decide_trump_color_request(client_request::base_class_properties props, int trump_color) :
        client_request(props),
        _trump_color(trump_color)
{ }

decide_trump_color_request* decide_trump_color_request::from_json(const rapidjson::Value &json) {
    base_class_properties props = client_request::extract_base_class_properties(json);
    if (json.HasMember("trump_color") ) {
        return new decide_trump_color_request(props, json["trump_color"].GetInt());
    } else {
        throw WizardException("Could not find 'trump_color' in decide_trump_color_request");
    }
}

void decide_trump_color_request::write_into_json(rapidjson::Value &json,
                                        rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    client_request::write_into_json(json, allocator);
    json.AddMember("trump_color", rapidjson::Value(this->_trump_color),allocator);
}
