#include <sqlpp/database.h>

#include <sqlite3.h>

#include <stdexcept>

using namespace std;

namespace sqlpp
{

Database::Database(const string& filename)
{
    auto rc = sqlite3_open(filename.c_str(), &db);
    if(rc != SQLITE_OK)
    {
        string err(sqlite3_errmsg(db));
        throw runtime_error("Cannot open database \"" + filename + "\": " + err);
    }
}

Database::Database(Database&& other)
{
    swap(db, other.db);
}

Database::~Database()
{
    if(db)
        sqlite3_close(db);
}

Database& Database::operator=(Database&& other)
{
    if(this != &other)
    {
        swap(db, other.db);
        if(other.db)
        {
            sqlite3_close(other.db);
            other.db = nullptr;
        }
    }
    return *this;
}

Result Database::execute(const string& sql, const vector<Bind>& values) const
{
    sqlite3_stmt* stmt = nullptr;
    auto rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if(rc != SQLITE_OK)
    {
        string err(sqlite3_errmsg(db));
        throw runtime_error("SQLite error in statement \"" + sql + "\": " + err);
    }

    Result res(stmt);

    int i = 1;
    for(auto&& v : values)
        v(res.handle(), i++);

    res.next();

    return res;
}

} /* namespace sqlpp */
