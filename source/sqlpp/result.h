#ifndef SRC_SQLPP_RESULT_H_
#define SRC_SQLPP_RESULT_H_

#include <sqlpp/types.h>

#include <optional>
#include <string>

struct sqlite3_stmt;

namespace sqlpp {

class Result {
 public:
  explicit Result(sqlite3_stmt* stmt);
  Result(const Result&) = delete;
  Result(Result&& other);

  virtual ~Result();
  Result& operator=(const Result&) = delete;
  Result& operator=(Result&& other);

  sqlite3_stmt* handle() { return stmt; }

  operator bool() const;

  void next();

  bool hasData() const;

  size_t count();
  std::string name(size_t i);

  template <typename R>
  std::optional<R> as(size_t i);

 private:
  static constexpr int NO_STATUS = -1;

  sqlite3_stmt* stmt = nullptr;
  int status = NO_STATUS;
};

extern template std::optional<Integer> Result::as(size_t i);
extern template std::optional<Real> Result::as(size_t i);
extern template std::optional<Text> Result::as(size_t i);
extern template std::optional<Blob> Result::as(size_t i);

template <typename T>
class TypedResult : public Result {
 public:
  using TypesList = T;

  using Result::Result;
  explicit TypedResult(Result&& other) : Result(std::move(other)) {}
  ~TypedResult() override = default;

  template <size_t N>
  auto get() {
    using Type = types::Get<N, TypesList>;
    std::optional<Type> res;

    auto value = as<DbType<Type>>(N);
    if (value) res = fromDb<Type>(*value);

    return res;
  }
};

}  // namespace sqlpp
#endif /* SRC_SQLPP_RESULT_H_ */
