#pragma once

#include <iostream>

#include "players/player-interface.h"

class human_player_t : public player_interface_t {
public:
  human_player_t(std::istream &input_stream, std::ostream &output_stream)
      : input_stream(input_stream), output_stream(output_stream) {}

private:
  std::istream &input_stream;
  std::ostream &output_stream;
};
