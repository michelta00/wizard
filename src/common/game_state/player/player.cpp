#include "player.h"
#include "../../exceptions/WizardException.h"
#include "../../serialization/vector_utils.h"

// constructor for client
player::player(const std::string& name) : unique_serializable() {
    this->_player_name = new serializable_value<std::string>(name);
    this->_nof_predicted = new serializable_value<int>(-1);
    this->_nof_tricks = new serializable_value<int>(0);
    this->_scores = std::vector<serializable_value<int>*>(0);
    this->_hand = new hand();

}

// deserialization constructor
player::player(const std::string& id, serializable_value<std::string>* name,
               serializable_value<int>* nof_tricks,
               serializable_value<int>* nof_predicted,
               const std::vector<serializable_value<int>*>& scores, hand *hand) :
        unique_serializable(id),
        _player_name(name),
        _nof_tricks(nof_tricks),
        _nof_predicted(nof_predicted),
        _scores(scores),
        _hand(hand)
{ }

player::~player() {
    if (_player_name != nullptr) {
        delete _hand;
        delete _player_name;
        delete _nof_predicted;
        delete _nof_tricks;


        _hand = nullptr;
        _player_name = nullptr;
        _nof_predicted = nullptr;
        _nof_tricks = nullptr;
    }
}

#ifdef WIZARD_SERVER
// constructor for server
player::player(const std::string& id, const std::string& name) :
        unique_serializable(id)
{
    this->_player_name = new serializable_value<std::string>(name);
    this->_scores = std::vector<serializable_value<int>*>(1, new serializable_value<int>(0));
    this->_hand = new hand();
    this->_nof_predicted = new serializable_value<int>(-1);
    this->_nof_tricks = new serializable_value<int>(0);
}

std::string player::get_game_id()
{
    return _game_id;
}

void player::set_game_id(const std::string& game_id)
{
    _game_id = game_id;
}
#endif


// getter and setter for scores
std::vector<serializable_value<int>*> player::get_scores() const noexcept
{
    return _scores;
}

void player::set_scores(const int score)
{
    _scores.push_back(new serializable_value<int>(score));
}


// getter and setter for number of won tricks
int player::get_nof_tricks() const noexcept
{
    return _nof_tricks->get_value();
}

void player::set_nof_tricks(const int nof_tricks)
{
    _nof_tricks->set_value(nof_tricks);
}


// getter and setter for number of predicted tricks
int player::get_nof_predicted() const noexcept
{
    return _nof_predicted->get_value();
}

void player::set_nof_predicted(const int nof_predicted)
{
    _nof_predicted->set_value(nof_predicted);
}


// other getters
std::string player::get_player_name() const noexcept
{
    return this->_player_name->get_value();
}

hand* player::get_hand() const noexcept
{
    return this->_hand;
}

unsigned int player::get_nof_cards() const noexcept
{
    return _hand->get_nof_cards();
}


#ifdef WIZARD_SERVER
void player::setup_round()
{
    _nof_predicted->set_value(-1);
    _nof_tricks->set_value(0);
    //delete _hand;
    //_hand = new hand();
    // TODO: check if the set up of new hand is necessary
}

void player::wrap_up_round() {
    int new_score = _scores.back()->get_value();
    if (_nof_predicted->get_value() == _nof_tricks->get_value())
    {
        new_score += 20 + (10 * _nof_predicted->get_value());
    }
    else
    {
        new_score -= std::abs(_nof_predicted->get_value() - _nof_tricks->get_value()) * 10;
    }
    _scores.push_back(new serializable_value<int>(new_score));
}

bool player::add_card(card *card, std::string &err) {
    return _hand->add_card(card, err);
}


#endif


void player::write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const {
    unique_serializable::write_into_json(json, allocator);

    rapidjson::Value id_val(_id.c_str(), allocator);
    json.AddMember("id", id_val, allocator);

    rapidjson::Value name_val(rapidjson::kObjectType);
    _player_name->write_into_json(name_val, allocator);
    json.AddMember("player_name", name_val, allocator);

    rapidjson::Value nof_tricks_val(rapidjson::kObjectType);
    _nof_tricks->write_into_json(nof_tricks_val, allocator);
    json.AddMember("nof_tricks", nof_tricks_val, allocator);

    rapidjson::Value nof_predicted_val(rapidjson::kObjectType);
    _nof_predicted->write_into_json(nof_predicted_val, allocator);
    json.AddMember("nof_predicted", nof_predicted_val, allocator);

    json.AddMember("scores", vector_utils::serialize_vector(_scores, allocator), allocator);

    rapidjson::Value hand_val(rapidjson::kObjectType);
    _hand->write_into_json(hand_val, allocator);
    json.AddMember("hand", hand_val, allocator);
}

player* player::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id")
        && json.HasMember("nof_predicted")
        && json.HasMember("nof_tricks")
        && json.HasMember("player_name")
        && json.HasMember("scores")
        && json.HasMember("hand"))
    {
        std::vector<serializable_value<int>*> deserialized_scores;
        for (auto &serialized_score : json["scores"].GetArray()) {
            deserialized_scores.push_back(serializable_value<int>::from_json(serialized_score.GetObject()));
        }
        return new player(
                json["id"].GetString(),
                serializable_value<std::string>::from_json(json["player_name"].GetObject()),
                serializable_value<int>::from_json(json["nof_tricks"].GetObject()),
                serializable_value<int>::from_json(json["nof_predicted"].GetObject()),
                deserialized_scores,
                hand::from_json(json["hand"].GetObject()));
    } else {
        throw WizardException("Failed to deserialize player from json. Required json entries were missing.");
    }
}
