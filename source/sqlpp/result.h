#ifndef SRC_SQLPP_RESULT_H_
#define SRC_SQLPP_RESULT_H_

#include <sqlpp/types.h>

#include <string>
#include <optional>

struct sqlite3_stmt;

namespace sqlpp
{

class Result
{
public:
    explicit Result(sqlite3_stmt* stmt);
    Result(const Result&) = delete;
    Result(Result&& other);

    ~Result();
    Result& operator=(const Result&) = delete;
    Result& operator=(Result&& other);

    sqlite3_stmt* handle()
    {
        return stmt;
    }

    operator bool() const;

    void next();

    bool hasData() const;

    size_t count();
    std::string name(size_t i);

    template<typename R>
    std::optional<R> as(size_t i);

private:
    sqlite3_stmt* stmt = nullptr;
    int status = -1;
};

extern template std::optional<Integer> Result::as(size_t i);
extern template std::optional<Real> Result::as(size_t i);
extern template std::optional<Text> Result::as(size_t i);
extern template std::optional<Blob> Result::as(size_t i);

} /* namespace sqlpp */

#endif /* SRC_SQLPP_RESULT_H_ */
