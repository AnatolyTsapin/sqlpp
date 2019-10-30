#ifndef SRC_SQLPP_STMT_COMMON_H_
#define SRC_SQLPP_STMT_COMMON_H_

#include <sqlpp/table.h>
#include <sqlpp/result.h>

#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <tuple>

namespace sqlpp
{

class Database;

class Statement
{
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

inline std::ostream& operator<<(std::ostream& stream, const Statement& stmt)
{
    stmt.dump(stream);
    return stream;
}

namespace stmt
{

template<typename D>
class StatementD : public Statement
{
protected:
    StatementD(const StatementD&) = default;
    StatementD(StatementD&&) = default;

    template<typename... A>
    StatementD(A&&... data) :
        data(std::forward<A>(data)...)
    {}

public:
    ~StatementD() override = default;

    StatementD& operator=(const StatementD&) = default;
    StatementD& operator=(StatementD&&) = default;

    void dump(std::ostream& stream) const override
    {
        data.dump(stream);
    }
    Result execute(const Database& db) const override
    {
        return data.execute(db);
    }

protected:
    D data;
};

} /* namespace stmt */
} /* namespace sqlpp */

#endif /* SRC_SQLPP_STMT_COMMON_H_ */
