#include "game/sea-battle.h"

#include <utility>

sea_battle_t::turn_t sea_battle_t::change_turn(turn_t current_turn) {
  return current_turn == FIRST_PLAYER ? SECOND_PLAYER : FIRST_PLAYER;
}

std::string sea_battle_t::get_player_name(turn_t turn) {
  return turn == FIRST_PLAYER ? "First" : "Second";
}

sea_battle_t::sea_battle_t(std::shared_ptr<player_interface_t> player1, field_t field1,
                           std::shared_ptr<player_interface_t> player2, field_t field2):
      _firstPlayer(std::move(player1)), _firstField(std::move(field1)),
      _secondPlayer(std::move(player2)), _secondField(std::move(field2)) {}

field_t makeEmptyField() {
  std::vector<std::string> emptyList;
  for (int i = 0; i < field_t::FIELD_SIZE; i++)
    emptyList.emplace_back(field_t::FIELD_SIZE, field_t::EMPTY_CELL);

  field_t emptyField = field_t(emptyList);
  return emptyField;
}

int getCountSHIP_CELL(field_t field) {
  int countSHIP_CELL = 0;
  for (int x = 0; x < field_t::FIELD_SIZE; x++)
    for (int y = 0; y < field_t::FIELD_SIZE; y++)
      if (field[x][y] == field_t::SHIP_CELL)
        countSHIP_CELL++;

  return countSHIP_CELL;
}

bool hasOtherSHIP_CELL(field_t &field, std::pair<int, int> coords) {
  const int MAX_SHIP_LEN = 3;
  int delta = 0;
  // scan directions, up to total of 4 (0,1,2,3)
  int scan_dir = 0;
  while (true) {
    if (delta > MAX_SHIP_LEN) {
      delta = 0;
      scan_dir++;
      if (scan_dir >= field_t::DIRECTIONS.size())
        // directions exhausted, no ship cells around
        return false;
      // start with new direction
    }

    // multiply by vector of direction
    int x = coords.first + (delta + 1) * field_t::DIRECTIONS[scan_dir].first;
    int y = coords.second + (delta + 1) * field_t::DIRECTIONS[scan_dir].second;
    delta++;
    if (!field_t::is_cell_valid(x, y))
      continue;

    auto cell = field[x][y];
    if (cell == field_t::EMPTY_CELL || cell == field_t::MISS_CELL) {
      // force skip to next direction
      delta = 10;
      continue;
    }

    // another ship cell is around, break the loop
    if (cell == field_t::SHIP_CELL)
      return true;
  }
}

void sea_battle_t::play() {
  turn_t currentTurn = turn_t::FIRST_PLAYER;
  std::array<field_t, 2> attacksField{};
  std::array<field_t, 2> playerField{};
  std::array<player_interface_t*, 2> players{};

  // construct empty fields for attack/miss markers
  attacksField[0] = makeEmptyField();
  attacksField[1] = makeEmptyField();
  // construct populated fields for game logic
  playerField[0] = _firstField;
  playerField[1] = _secondField;
  // construct players array
  players[0] = _firstPlayer.get();
  players[1] = _secondPlayer.get();
  // count SHIP_CELL
  std::array<int, 2> playersShipCells{ getCountSHIP_CELL(_firstField), getCountSHIP_CELL(_secondField) };
  std::array<int, 2> playersShipHitCells{ 0, 0 };

  // run game cycle
  while (true) {
    auto player = players[currentTurn];
    auto enemy = players[1 - currentTurn];
    // ask player to make a move
    auto coords = player->make_move(playerField[currentTurn], attacksField[currentTurn]);
    // check if given cell is within the game field
    if (!field_t::is_cell_valid(coords.first, coords.second)) {
      // invalid cell coordinates
      player->on_incorrect_move(coords.first, coords.second);
      continue; // continue, same player
    }

    // tile from the enemy field
    char tile = playerField[1 - currentTurn][coords.first][coords.second];

    if (tile == field_t::HIT_CELL || tile == field_t::MISS_CELL) {
      // in case we hit an already burning cell
      player->on_duplicate_move(coords.first, coords.second);
      continue; // continue, same player
    }
    else if (tile == field_t::SHIP_CELL) {
      // in case player hits an enemy cell
      if (hasOtherSHIP_CELL(playerField[1 - currentTurn], coords))
        player->on_hit(coords.first, coords.second);
      else
        player->on_kill(coords.first, coords.second);

      playerField[1 - currentTurn][coords.first][coords.second] = field_t::HIT_CELL;
      attacksField[currentTurn][coords.first][coords.second] = field_t::HIT_CELL;
      playersShipHitCells[1 - currentTurn] += 1;
      // check win/lose condition
      if (playersShipCells[1 - currentTurn] == playersShipHitCells[1 - currentTurn]) {
        player->on_win();
        enemy->on_lose();
        break;
      }

      continue; // continue, same player
    }
    else if (tile == field_t::EMPTY_CELL) {
      // in case current player missed
      player->on_miss(coords.first, coords.second);
      playerField[1 - currentTurn][coords.first][coords.second] = field_t::MISS_CELL;
      attacksField[currentTurn][coords.first][coords.second] = field_t::MISS_CELL;
      // pass
    }

    // switch player
    currentTurn = change_turn(currentTurn);
  }
}
