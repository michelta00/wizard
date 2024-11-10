#ifndef WIZARD_PLAYER_H
#define WIZARD_PLAYER_H

#include <string>
#include "hand.h"
#include "../../serialization/uuid_generator.h"
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"

class player : public unique_serializable {
private:
    serializable_value<std::string>* _player_name;  // player's name chosen by the player
    serializable_value<int>* _nof_tricks;           // number of tricks won in current round
    serializable_value<int>* _nof_predicted;        // number of predicted tricks in current round
    serializable_value<int>* _score;                // score of the player (total game score)
    hand* _hand;

#ifdef WIZARD_SERVER
    std::string _game_id;       // ID of the game the player has joint
#endif

    /*
     * Deserialization constructor
     */
    player(std::string id,
           serializable_value<std::string>* name,
           serializable_value<int>* nof_tricks,
           serializable_value<int>* nof_predicted,
           serializable_value<int>* score,
           hand* hand);

public:
// constructors
    explicit player(std::string name);   // for client
    ~player();

#ifdef WIZARD_SERVER
    player(std::string id, std::string name);  // for server

    std::string get_game_id();
    void set_game_id(std::string game_id);
#endif

    // accessors
    int get_score() const noexcept;
    void set_score(int score);

    int get_nof_tricks() const noexcept;
    void set_nof_tricks(int nof_tricks);

    int get_nof_predicted() const noexcept;
    void set_nof_predicted(int nof_predicted);

    int get_nof_cards() const noexcept;
    const hand* get_hand() const noexcept;
    std::string get_player_name() const noexcept;

#ifdef WIZARD_SERVER
    // state update functions
    bool add_card(card* card, std::string& err);
    bool remove_card(std::string card_id, card*& card, std::string& err);

    void wrap_up_round(std::string& err);
    void setup_round(std::string& err);
#endif


    // serialization
    static player* from_json(const rapidjson::Value& json);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

};


#endif //WIZARD_PLAYER_H
