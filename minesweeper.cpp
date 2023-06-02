#include "board.cpp"
#include <iostream>

enum flag_ops { NONE, SET, UNSET };

int main() {
  int width, height, mines;
  flag_ops flag_op;
  std::cout << "Width: ";
  std::cin >> width;
  std::cout << "Height: ";
  std::cin >> height;
  std::cout << "Mines: ";
  std::cin >> mines;
  Board board(width, height);
  board.populate_mines(mines);
  std::cout << board.to_string() << std::endl;
  std::string input;
  std::cin >> input;
  while (input.compare("quit")) {
    if (input[0] == '+') {
      flag_op = SET;
    } else if (input[0] == '-') {
      flag_op = UNSET;
    } else {
      flag_op = NONE;
    }
    int first = input[flag_op ? 1 : 0] - 'A';
    int last = std::stoi(input.substr(flag_op ? 2 : 1, input.length())) - 1;
    switch (flag_op) {
    case SET:
      board.set_flag(first, last);
      break;
    case UNSET:
      board.unset_flag(first, last);
      break;
    case NONE:
    default:
      int rev = board.reveal(first, last);
      printf("%d\n", rev);
      if (rev < 0) {
        std::cout << "Boom you ded" << std::endl;
        std::cout << board.to_string(true) << std::endl;
        std::cout << "Width: ";
        std::cin >> width;
        std::cout << "Height: ";
        std::cin >> height;
        std::cout << "Mines: ";
        std::cin >> mines;
        board.init(width, height);
        board.populate_mines(mines);
      }
      break;
    }

    std::cout << board.to_string() << std::endl;
    std::cin >> input;
  }
}