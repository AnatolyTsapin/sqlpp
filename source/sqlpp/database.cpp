#include "database.h"

#include <sqlite3.h>

#include <stdexcept>

namespace sqlpp {

Database::Database(const std::string& filename) {
  auto rc = sqlite3_open(filename.c_str(), &db);
  if (rc != SQLITE_OK) {
    std::string err(sqlite3_errmsg(db));
    throw std::runtime_error("Cannot open database \"" + filename +
                             "\": " + err);
  }
}

Database::Database(Database&& other) { std::swap(db, other.db); }

Database::~Database() {
  if (db) sqlite3_close(db);
}

Database& Database::operator=(Database&& other) {
  if (this != &other) {
    std::swap(db, other.db);
    if (other.db) {
      sqlite3_close(other.db);
      other.db = nullptr;
    }
  }
  return *this;
}

Result Database::execute(const std::string& sql,
                         const std::vector<Bind>& values) const {
  sqlite3_stmt* stmt = nullptr;
  auto rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK) {
    std::string err(sqlite3_errmsg(db));
    throw std::runtime_error("SQLite error in statement \"" + sql +
                             "\": " + err);
  }

  Result res(stmt);

  int i = 1;
  for (auto&& v : values) v(res.handle(), i++);

  res.next();

  return res;
}

}  // namespace sqlpp
