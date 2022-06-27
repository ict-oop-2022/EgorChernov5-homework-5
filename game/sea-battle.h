#pragma once

#include <memory>

#include "game/field.h"
#include "players/player-interface.h"

class sea_battle_t {
public:
  enum turn_t { FIRST_PLAYER = 0, SECOND_PLAYER = 1 };

  sea_battle_t(std::shared_ptr<player_interface_t> player1, field_t field1, std::shared_ptr<player_interface_t> player2,
               field_t field2);

  static turn_t change_turn(turn_t current_turn);
  static std::string get_player_name(turn_t turn);

  void play();

  ~sea_battle_t() = default;
private:
  std::shared_ptr<player_interface_t> _firstPlayer;
  field_t _firstField;
  std::shared_ptr<player_interface_t> _secondPlayer;
  field_t _secondField;
};
