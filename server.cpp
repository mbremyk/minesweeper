#include "board.cpp"
#include "crow.h"

Board *board;

std::string json_to_string(Json::Value json);
Json::Value body_to_json(std::string body);
bool is_number(std::string s);

int main() {
  crow::SimpleApp app;

  CROW_ROUTE(app, "/")
  ([]() {
    auto page = crow::mustache::load_text("index.html");
    return page;
  });

  CROW_ROUTE(app, "/new")
      .methods(
          crow::HTTPMethod::PUT)([](crow::request &req, crow::response &res) {
        Json::Value request_body = body_to_json(req.body);
        int width =
            request_body["width"] ? atoi(request_body["width"].asCString()) : 0;
        int height = request_body["height"]
                         ? atoi(request_body["height"].asCString())
                         : 0;
        int size =
            request_body["size"] ? atoi(request_body["size"].asCString()) : 0;
        int mines =
            request_body["mines"] ? atoi(request_body["mines"].asCString()) : 0;
        if (width && height) {
          board = new Board(width, height);
        } else if (size) {
          board = new Board(size);
        } else {
          board = new Board();
        }
        if (!board) {
          res.code = crow::INTERNAL_SERVER_ERROR;
          res.end();
        } else {
          board->populate_mines(mines ? mines : INT_MAX);
          res.body = board->to_json_string();
          res.end();
        }
      });

  CROW_ROUTE(app, "/reveal")
      .methods(
          crow::HTTPMethod::PUT)([](crow::request &req, crow::response &res) {
        assert(board);

        Json::Value request_body = body_to_json(req.body);
        int x = request_body["x"] ? atoi(request_body["x"].asCString()) : -1;
        int y = request_body["y"] ? atoi(request_body["y"].asCString()) : -1;
        printf("%d %d\n", x, y);
        if (x < 0 || y < 0) {
          res.code = crow::BAD_REQUEST;
          res.end();
          return;
        }
        Json::Value response_body;
        response_body["revealed"] = board->reveal(x, y);
        response_body["value"] = board->at(x, y);
        response_body["x"] = x;
        response_body["y"] = y;
        Json::Value jboard = board->to_json(false);
        response_body["board"] = jboard["board"];
        response_body["width"] = jboard["width"];
        response_body["height"] = jboard["height"];
        res.body = json_to_string(response_body);
        res.end();
      });

  CROW_ROUTE(app, "/flag")
      .methods(
          crow::HTTPMethod::PUT)([](crow::request &req, crow::response &res) {
        assert(board);
        Json::Value request_body = body_to_json(req.body);
        int x = request_body["x"] ? atoi(request_body["x"].asCString()) : -1;
        int y = request_body["y"] ? atoi(request_body["y"].asCString()) : -1;
        if (x < 0 || y < 0) {
          res.code = crow::BAD_REQUEST;
          res.end();
          return;
        }
        Json::Value response_body;
        response_body["flagged"] = !board->flag(x, y);
        response_body["x"] = x;
        response_body["y"] = y;
        Json::Value jboard = board->to_json(false);
        response_body["board"] = jboard["board"];
        response_body["width"] = jboard["width"];
        response_body["height"] = jboard["height"];
        res.body = json_to_string(response_body);
        res.end();
      });

  CROW_ROUTE(app, "/print")
  ([]() {
    assert(board);
    printf("%s\n", board->to_string(true).c_str());
    return crow::status::OK;
  });

  app.port(8080).multithreaded().run();
}

std::string json_to_string(Json::Value json) {
  Json::StreamWriterBuilder builder;
  return Json::writeString(builder, json);
}

Json::Value body_to_json(std::string body) {
  Json::Value jbody;
  Json::Reader reader;
  reader.parse(body.c_str(), jbody);
  return jbody;
}

bool is_number(std::string s) {
  return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) {
                         return !std::isdigit(c);
                       }) == s.end();
}