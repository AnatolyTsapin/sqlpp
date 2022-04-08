#include "insert.h"

#include <sstream>

#include "../database.h"

namespace sqlpp::stmt {

InsertData::InsertData(const std::string& tableName) : tableName(tableName) {}

InsertData::InsertData(const InsertData&) = default;
InsertData::InsertData(InsertData&&) = default;
InsertData& InsertData::operator=(const InsertData&) = default;
InsertData& InsertData::operator=(InsertData&&) = default;

void InsertData::addValue(const std::string& name, Bind bind) {
  names.emplace_back(name);
  binds.emplace_back(move(bind));
}

void InsertData::addValue(Bind bind) { binds.emplace_back(move(bind)); }

void InsertData::dump(std::ostream& stream) const {
  stream << "INSERT INTO " << tableName;
  if (binds.empty()) {
    stream << " DEFAULT VALUES";
  } else if (names.empty()) {
    stream << " VALUES (";
    for (size_t i = 0; i < binds.size(); ++i) {
      if (i != 0) stream << ", ";
      stream << "?";
    }
    stream << ")";
  } else {
    stream << " (";
    bool first = true;
    std::stringstream valStream;
    for (auto&& n : names) {
      if (!first) {
        stream << ", ";
        valStream << ", ";
      }
      first = false;
      stream << n;
      valStream << "?";
    }
    stream << ") VALUES (" << valStream.str() << ")";
  }
}

Result InsertData::execute(const Database& db) const {
  std::ostringstream ss;
  dump(ss);
  return db.execute(ss.str(), binds);
}

}  // namespace sqlpp::stmt