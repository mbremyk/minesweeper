#include <algorithm>
#include <ctime>
#ifndef NO_JSON
#include <jsoncpp/json/json.h>
#endif
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
  int m_width = default_size;
  int m_height = default_size;
  std::vector<std::vector<int>> m_board;

  enum state { HIDDEN, FLAGGED, REVEALED };

  std::vector<std::vector<state>> m_state;

public:
  Board() { init(); }

  Board(int size) {
    set_size(size);
    init();
  }

  Board(int width, int height) {
    set_size(width, height);
    init();
  }

  bool set_size(int size) {
    m_width = std::min(size, MAX_SIZE);
    m_height = std::min(size, MAX_SIZE);
    return false;
  }

  bool set_size(int width, int height) {
    m_width = std::min(width, MAX_SIZE);
    m_height = std::min(height, MAX_SIZE);
    return false;
  }

  int width() const { return m_width; }

  int height() const { return m_height; }

  bool init() {
    m_board.clear();
    m_state.clear();
    for (auto i = 0; i < m_height; ++i) {
      auto &board_row = m_board.emplace_back();
      auto &revealed_row = m_state.emplace_back();
      for (auto j = 0; j < m_width; ++j) {
        board_row.push_back(0);
        revealed_row.push_back(HIDDEN);
      }
    }

    return false;
  }

  bool init(int size) { return set_size(size) || init(); }
  bool init(int width, int height) { return set_size(width, height) || init(); }

  bool populate_mines(int amount) {
    std::vector<int> temp(m_width * m_height);
    std::iota(temp.begin(), temp.end(), 0);
    std::shuffle(temp.begin(), temp.end(),
                 std::default_random_engine(std::time(nullptr)));
    amount = std::min(amount, (m_width - 1) * (m_height - 1));
    int i = 0;
    while (i < amount && temp.size() > 0) {
      auto pos = temp.back();
      temp.pop_back();
      m_board[pos / m_width][pos % m_width] = -1;
      ++i;
    }

    for (auto y = 0; y < m_height; ++y) {
      for (auto x = 0; x < m_width; ++x) {
        if (m_board[y][x] < 0)
          continue;
        if (y && x && m_board[y - 1][x - 1] < 0)
          ++m_board[y][x];
        if (y && m_board[y - 1][x] < 0)
          ++m_board[y][x];
        if (y && x < m_width - 1 && m_board[y - 1][x + 1] < 0)
          ++m_board[y][x];
        if (x && m_board[y][x - 1] < 0)
          ++m_board[y][x];
        if (x < m_width - 1 && m_board[y][x + 1] < 0)
          ++m_board[y][x];
        if (y < m_height - 1 && x && m_board[y + 1][x - 1] < 0)
          ++m_board[y][x];
        if (y < m_height - 1 && m_board[y + 1][x] < 0)
          ++m_board[y][x];
        if (y < m_height - 1 && x < m_width - 1 && m_board[y + 1][x + 1] < 0)
          ++m_board[y][x];
      }
    }

    return false;
  }

  int reveal(int x, int y, bool recursed = false) {
    if (m_state[y][x]) {
      return 0;
    }
    m_state[y][x] = REVEALED;
    if (!recursed && m_board[y][x] < 0) {
      return -1;
    }
    if (m_board[y][x] > 0) {
      return 1;
    }

    int g = 0;
    if (x && y)
      g += reveal(x - 1, y - 1, true);
    if (x)
      g += reveal(x - 1, y, true);
    if (x && y < m_height - 1)
      g += reveal(x - 1, y + 1, true);
    if (y)
      g += reveal(x, y - 1, true);
    if (y < m_height - 1)
      g += reveal(x, y + 1, true);
    if (x < m_width - 1 && y)
      g += reveal(x + 1, y - 1, true);
    if (x < m_width - 1)
      g += reveal(x + 1, y, true);
    if (x < m_width - 1 && y < m_height - 1)
      g += reveal(x + 1, y + 1, true);
    return g;
  }

  bool set_flag(int x, int y) {
    if (m_state[y][x] == HIDDEN) {
      m_state[y][x] = FLAGGED;
      return false;
    }
    return true;
  }

  bool unset_flag(int x, int y) {
    if (m_state[y][x] == FLAGGED) {
      m_state[y][x] = HIDDEN;
      return false;
    }
    return true;
  }

  bool flag(int x, int y) {
    if (m_state[y][x] != REVEALED) {
      m_state[y][x] = (state)!m_state[y][x];
      return false;
    }
    return true;
  }

  int at(int x, int y, bool revealed = false) {
    if (revealed || m_state[y][x] == REVEALED) {
      return m_board[y][x];
    }
    return -2;
  }

  bool check_win() {
    for (auto y = 0; y < m_height; ++y) {
      for (auto x = 0; x < m_width; ++x) {
        if (m_board[y][x] < 0 && m_state[y][x] != FLAGGED)
          return false;
        if (m_board[y][x] >= 0 && m_state[y][x] == FLAGGED)
          return false;
      }
    }
    return true;
  }

  std::string to_string(bool revealed = false) {
    int indent = std::floor(std::log10(m_height));
    int top_indent = indent;
    std::string o;
    o += "     ";
    for (auto i = 0; i < top_indent; ++i) {
      o += " ";
    }
    for (auto i = 0; i < m_width; ++i) {
      o += 'A' + i;
      o += "  ";
    }
    o += '\n';
    o += "   ";
    for (auto i = 0; i < top_indent; ++i) {
      o += " ";
    }
    o += TOP_LEFT_CORNER;
    for (auto i = 0; i < m_width; ++i) {
      o += HORIZONTAL_LINE;
      o += HORIZONTAL_LINE;
      o += HORIZONTAL_LINE;
    }
    o += TOP_RIGHT_CORNER;
    o += '\n';
    for (auto i = 0; i < m_height; ++i) {
      auto &row = m_board.at(i);
      o += " ";
      indent = top_indent - std::floor(std::log10(i + 1));
      for (auto j = 0; j < indent; ++j) {
        o += " ";
      }
      o += std::to_string(i + 1);
      o += " ";
      o += VERTICAL_LINE;
      for (auto j = 0; j < m_width; ++j) {
        auto val = row.at(j);

        if (revealed || (m_state[i][j] == REVEALED)) {
          o += val < 0 ? "" : " ";
          o += std::to_string(val);
          o += " ";
        } else if (m_state[i][j] == FLAGGED) {
          o += " 🚩";
        } else {
          o += " _ ";
        }
      }
      o += VERTICAL_LINE;
      o += '\n';
    }
    o += "   ";
    for (auto i = 0; i < top_indent; ++i) {
      o += " ";
    }
    o += BOTTOM_LEFT_CORNER;
    for (auto i = 0; i < m_width; ++i) {
      o += HORIZONTAL_LINE;
      o += HORIZONTAL_LINE;
      o += HORIZONTAL_LINE;
    }
    o += BOTTOM_RIGHT_CORNER;
    return o;
  }

#ifndef NO_JSON

  Json::Value to_json(bool revealed = false) {
    Json::Value out;
    out["width"] = m_width;
    out["height"] = m_height;

    for (auto i = 0; i < m_height; ++i) {
      Json::Value row;
      for (auto j = 0; j < m_width; ++j) {
        if (revealed || (m_state[i][j] == REVEALED)) {
          row[j] = m_board[i][j];
        } else if (m_state[i][j] == FLAGGED) {
          row[j] = "🚩";
        } else {
          row[j] = "";
        }
      }
      out["board"][i] = row;
    }

    return out;
  }

  std::string to_json_string(bool revealed = false) {
    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, to_json(revealed));
  }
};

#endif
