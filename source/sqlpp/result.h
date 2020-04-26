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

    std::optional<Integer> asInteger(size_t i);
    std::optional<Real> asReal(size_t i);
    std::optional<Text> asText(size_t i);
    std::optional<Blob> asBlob(size_t i);

    template<typename R>
    std::optional<R> as(size_t i);

private:
    sqlite3_stmt* stmt = nullptr;
    int status = -1;
};

template<>
inline std::optional<Integer> Result::as<Integer>(size_t i)
{
    return asInteger(i);
}

template<>
inline std::optional<Real> Result::as<Real>(size_t i)
{
    return asReal(i);
}

template<>
inline std::optional<Text> Result::as<Text>(size_t i)
{
    return asText(i);
}

template<>
inline std::optional<Blob> Result::as<Blob>(size_t i)
{
    return asBlob(i);
}

} /* namespace sqlpp */

#endif /* SRC_SQLPP_RESULT_H_ */
