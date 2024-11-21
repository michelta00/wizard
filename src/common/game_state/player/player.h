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
    std::vector<serializable_value<int>*> _scores;  // scores of the player (total game score, current and past ones)
    hand* _hand;

#ifdef WIZARD_SERVER
    std::string _game_id;       // ID of the game the player has joint
#endif

    /*
     * Deserialization constructor
     */
    player(const std::string& id,
           serializable_value<std::string>* name,
           serializable_value<int>* nof_tricks,
           serializable_value<int>* nof_predicted,
           const std::vector<serializable_value<int>*>& scores,
           hand* hand);

public:
// constructors
    explicit player(const std::string& name);   // for client
    ~player();

#ifdef WIZARD_SERVER
    player(const std::string& id, const std::string& name);  // for server

    std::string get_game_id();
    void set_game_id(const std::string& game_id);
#endif

    // accessors
    [[nodiscard]] std::vector<serializable_value<int>*> get_scores() const noexcept;
    void set_scores(int score);

    [[nodiscard]] int get_nof_tricks() const noexcept;
    void set_nof_tricks(int nof_tricks) const;

    [[nodiscard]] int get_nof_predicted() const noexcept;
    void set_nof_predicted(int nof_predicted) const;

    [[nodiscard]] unsigned int get_nof_cards() const noexcept;
    [[nodiscard]] hand* get_hand() const noexcept;
    [[nodiscard]] std::string get_player_name() const noexcept;

#ifdef WIZARD_SERVER
    // state update functions
    bool add_card(card* card, std::string& err);
    bool remove_card(const std::string& card_id, card*& card, std::string& err);

    void wrap_up_round();
    void setup_round();
#endif


    // serialization
    static player* from_json(const rapidjson::Value& json);
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

};


#endif //WIZARD_PLAYER_H
