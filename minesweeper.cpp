#include "board.cpp"
#include <iostream>
#include <random>
#include <string>

int size;
int mines;

int main() {
  std::cin >> size >> mines;
  Board board(size);
  board.populate_mines(mines);
  printf("%s\n", board.to_string().c_str());
  std::string input;
  std::cin >> input;
  while (input.compare("quit")) {
    int first = input[0] - 'A';
    int last = std::stoi(input.substr(1, input.length())) - 1;
    board.reveal(last, first);

    printf("%s\n", board.to_string().c_str());
    std::cin >> input;
  }
}