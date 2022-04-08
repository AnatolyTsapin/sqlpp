#ifndef SQLPP_DATABASE_H_
#define SQLPP_DATABASE_H_

#include <string>

#include "result.h"
#include "types.h"

struct sqlite3;

namespace sqlpp {

class Database {
 public:
  explicit Database(const std::string& filename);
  Database(const Database&) = delete;
  Database(Database&& other);

  ~Database();

  Database& operator=(const Database&) = delete;
  Database& operator=(Database&& other);

  sqlite3* handle() const { return db; }

  Result execute(const std::string& sql,
                 const std::vector<Bind>& values = {}) const;

 private:
  sqlite3* db = nullptr;
};

}  // namespace sqlpp

#endif /* SQLPP_DATABASE_H_ */
