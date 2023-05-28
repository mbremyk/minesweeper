#include <algorithm>
#include <ctime>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#define TOP_LEFT_CORNER "┏"
#define TOP_RIGHT_CORNER "┓"
#define HORIZONTAL_LINE "━"
#define VERTICAL_LINE "┃"
#define BOTTOM_LEFT_CORNER "┗"
#define BOTTOM_RIGHT_CORNER "┛"
#define MAX_SIZE 62

class Board {
public:
  const int default_size = 5;

private:
  int m_size = default_size;
  std::vector<std::vector<int>> m_board;
  std::vector<std::vector<bool>> m_revealed;

public:
  Board() { init(); }

  Board(int size) {
    set_size(size);
    init();
  }

  bool set_size(int size) {
    m_size = std::min(size, MAX_SIZE);
    return false;
  }

  int size() { return m_size; }

  bool init() {
    m_board.clear();
    m_revealed.clear();
    for (auto i = 0; i < m_size; ++i) {
      auto &board_row = m_board.emplace_back();
      auto &revealed_row = m_revealed.emplace_back();
      for (auto j = 0; j < m_size; ++j) {
        board_row.push_back(0);
        revealed_row.push_back(false);
      }
    }

    return false;
  }

  bool init(int size) { return set_size(size) || init(); }

  bool populate_mines(int amount) {
    std::vector<int> temp(m_size * m_size);
    std::iota(temp.begin(), temp.end(), 0);
    std::shuffle(temp.begin(), temp.end(),
                 std::default_random_engine(std::time(nullptr)));
    int i = 0;
    while (i < amount && temp.size() > 0) {
      auto pos = temp.back();
      temp.pop_back();
      m_board[pos / m_size][pos % m_size] = -1;
      ++i;
    }

    for (auto i = 0; i < m_size; ++i) {
      for (auto j = 0; j < m_size; ++j) {
        if (m_board[i][j] < 0)
          continue;
        if (i && j && m_board[i - 1][j - 1] < 0)
          ++m_board[i][j];
        if (i && m_board[i - 1][j] < 0)
          ++m_board[i][j];
        if (i && j < m_size - 1 && m_board[i - 1][j + 1] < 0)
          ++m_board[i][j];
        if (j && m_board[i][j - 1] < 0)
          ++m_board[i][j];
        if (j < m_size - 1 && m_board[i][j + 1] < 0)
          ++m_board[i][j];
        if (i < m_size - 1 && j && m_board[i + 1][j - 1] < 0)
          ++m_board[i][j];
        if (i < m_size - 1 && m_board[i + 1][j] < 0)
          ++m_board[i][j];
        if (i < m_size - 1 && j < m_size - 1 && m_board[i + 1][j + 1] < 0)
          ++m_board[i][j];
      }
    }

    return false;
  }

  int reveal(int x, int y, bool recursed = false) {
    if (m_revealed[x][y]) {
      return 0;
    }
    m_revealed[x][y] = true;
    if (!recursed && m_board[x][y] < 0) {
      return -1;
    }
    if (m_board[x][y] > 0) {
      return 1;
    }

    int g = 0;
    if (x && y)
      reveal(x - 1, y - 1, true);
    if (x)
      reveal(x - 1, y, true);
    if (x && y < m_size - 1)
      reveal(x - 1, y + 1, true);
    if (y)
      reveal(x, y - 1, true);
    if (y < m_size - 1)
      reveal(x, y + 1, true);
    if (x < m_size - 1 && y)
      reveal(x + 1, y - 1, true);
    if (x < m_size - 1)
      reveal(x + 1, y, true);
    if (x < m_size - 1 && y < m_size - 1)
      reveal(x + 1, y + 1, true);
    return false;
  }

  std::string to_string(bool revealed = false) {
    int indent = std::floor(std::log10(m_size));
    int top_indent = indent;
    std::string o;
    o += "     ";
    for (auto i = 0; i < top_indent; ++i) {
      o += " ";
    }
    for (auto i = 0; i < m_size; ++i) {
      o += 'A' + i;
      o += "  ";
    }
    o += '\n';
    o += "   ";
    for (auto i = 0; i < top_indent; ++i) {
      o += " ";
    }
    o += TOP_LEFT_CORNER;
    for (auto i = 0; i < m_size; ++i) {
      o += HORIZONTAL_LINE;
      o += HORIZONTAL_LINE;
      o += HORIZONTAL_LINE;
    }
    o += TOP_RIGHT_CORNER;
    o += '\n';
    for (auto i = 0; i < m_size; ++i) {
      auto &row = m_board.at(i);
      o += " ";
      indent = top_indent - std::floor(std::log10(i + 1));
      for (auto j = 0; j < indent; ++j) {
        o += " ";
      }
      o += std::to_string(i + 1);
      o += " ";
      o += VERTICAL_LINE;
      for (auto j = 0; j < m_size; ++j) {
        auto val = row.at(j);
        o += revealed || m_revealed[i][j] ? (val < 0 ? "" : " ") : " ";
        o += revealed || m_revealed[i][j] ? std::to_string(val) : "_";
        o += " ";
      }
      o += VERTICAL_LINE;
      o += '\n';
    }
    o += "   ";
    for (auto i = 0; i < top_indent; ++i) {
      o += " ";
    }
    o += BOTTOM_LEFT_CORNER;
    for (auto i = 0; i < m_size; ++i) {
      o += HORIZONTAL_LINE;
      o += HORIZONTAL_LINE;
      o += HORIZONTAL_LINE;
    }
    o += BOTTOM_RIGHT_CORNER;
    return o;
  }
};