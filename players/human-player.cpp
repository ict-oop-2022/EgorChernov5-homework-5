#include "players/human-player.h"

std::pair<int, int> human_player_t::make_move(const field_t &my_field, const field_t &enemy_field) {
  output_stream << "Your field:" << std::endl;
  output_stream << my_field << std::endl;
  output_stream << "Enemy's field" << std::endl;
  output_stream << enemy_field << std::endl;
  output_stream << "Input you turn! [Example 1A or 10B or 6J]" << std::endl;

  std::pair<int, int> coords;
  std::string s;
  while (true) {
    std::getline(input_stream, s);
    std::regex reCheck("^\\d[0]?[A-J]$");
    if (std::regex_match(s, reCheck)) {
      if (std::stoi(s.substr(0, s.length() - 1)) == 0) {
        output_stream << "Incorrect move! [Correct example 1A or 10B or 6J]" << std::endl;
        continue;
      }
    }
    else {
      output_stream << "Incorrect move! [Correct example 1A or 10B or 6J]" << std::endl;
      continue;
    }

    coords = {std::stoi(s.substr(0, s.length() - 1)) - 1, int(s.at(s.size() - 1)) - A};
    break;
  }

  return coords;
}

void human_player_t::on_incorrect_move(int, int) {
  assert(false);
}

void human_player_t::on_duplicate_move(int x, int y) {
  output_stream << std::to_string(x + 1) + char(A + y) << " DUPLICATE! Repeat move!" << std::endl;
}

void human_player_t::on_miss(int x, int y) {
  output_stream << std::to_string(x + 1) + char(A + y) << " MISS! Enemy's turn!" << std::endl;
}

void human_player_t::on_hit(int x, int y) {
  output_stream << std::to_string(x + 1) + char(A + y) << " HIT! Your next move!" << std::endl;
}

void human_player_t::on_kill(int x, int y) {
  output_stream << std::to_string(x + 1) + char(A + y) << " KILL! Your next move!" << std::endl;
}

void human_player_t::on_win() {
  output_stream << "You WIN!" << std::endl;
}

void human_player_t::on_lose() {
  output_stream << "You LOSE!" << std::endl;
}
