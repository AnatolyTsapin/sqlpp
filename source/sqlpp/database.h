#ifndef SQLPP_DATABASE_H_
#define SQLPP_DATABASE_H_

#include <sqlpp/types.h>
#include <sqlpp/result.h>

#include <string>
#include <memory>
#include <functional>

extern "C"
typedef struct sqlite3 sqlite3;

namespace sqlpp
{

class Database
{
public:
    explicit Database(const std::string& filename);

    sqlite3* handle() const
    {
        return db.get();
    }

    Result execute(const std::string& sql) const;
    Result execute(const std::string& sql, const std::vector<Bind>& values) const;

private:
    using SQLite = std::unique_ptr<sqlite3, std::function<void(sqlite3*)>>;
    SQLite db;
};

} /* namespace sqlpp */

#endif /* SQLPP_DATABASE_H_ */
