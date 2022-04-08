#ifndef SRC_SQLPP_STMT_COMMON_H_
#define SRC_SQLPP_STMT_COMMON_H_

#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "../result.h"
#include "../table.h"

namespace sqlpp {

class Database;

class Statement {
 protected:
  Statement();
  Statement(const Statement&);
  Statement(Statement&&);

 public:
  virtual ~Statement();

  Statement& operator=(const Statement&);
  Statement& operator=(Statement&&);

  virtual void dump(std::ostream& stream) const = 0;
  virtual Result execute(const Database& db) const = 0;
};

inline std::ostream& operator<<(std::ostream& stream, const Statement& stmt) {
  stmt.dump(stream);
  return stream;
}

namespace stmt {

template <typename D>
class StatementD : public Statement {
 public:
  StatementD(const StatementD&) = default;
  StatementD(StatementD&&) = default;

  template <typename... A>
  explicit StatementD(A&&... data) : data(std::forward<A>(data)...) {}

  ~StatementD() override = default;

  StatementD& operator=(const StatementD&) = default;
  StatementD& operator=(StatementD&&) = default;

  void dump(std::ostream& stream) const override { data.dump(stream); }
  Result execute(const Database& db) const override { return data.execute(db); }

 protected:
  D data;
};

}  // namespace stmt
}  // namespace sqlpp

#endif /* SRC_SQLPP_STMT_COMMON_H_ */
