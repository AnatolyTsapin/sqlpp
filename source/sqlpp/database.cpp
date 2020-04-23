#include <sqlpp/database.h>

#include <sqlite3.h>

#include <stdexcept>
#include <memory>
#include <iostream>

using namespace std;

namespace sqlpp
{

static int dbCallback(void* handle, int count, char** values, char** names)
{
    cout << "Callback:" << endl;
    for(int i = 0; i < count; ++i)
        cout << names[i] << " = " << values[i] << endl;
    return 0;
}

Database::Database(const string& filename)
{
    sqlite3* temp = nullptr;
    sqlite3_open(filename.c_str(), &temp);
    db = SQLite(temp, [](sqlite3* db) { sqlite3_close(db); });
}

Result Database::execute(const string& sql) const
{
    Result res;
    char* errorMsg = nullptr;
    auto rc = sqlite3_exec(db.get(), sql.c_str(), dbCallback, &res, &errorMsg);
    if(rc != SQLITE_OK)
    {
        string err(errorMsg);
        sqlite3_free(errorMsg);
        throw runtime_error("SQLite error in statement \"" + sql + "\": " + err);
    }
    return res;
}

Result Database::execute(const string& sql, const vector<Bind>& values) const
{
    if(values.empty())
        return execute(sql);

    Result res;
    sqlite3_stmt* s = nullptr;
    auto rc = sqlite3_prepare_v2(db.get(), sql.c_str(), -1, &s, nullptr);
    if(rc != SQLITE_OK)
    {
        string err(sqlite3_errmsg(db.get()));
        throw runtime_error("SQLite error in statement \"" + sql + "\": " + err);
    }

    auto stmt = std::unique_ptr<sqlite3_stmt, function<void(sqlite3_stmt*)>>(s, [](sqlite3_stmt* s) { sqlite3_finalize(s); });

    int i = 1;
    for(auto&& v : values)
        v(stmt.get(), i++);

    rc = sqlite3_step(stmt.get());
    if(rc != SQLITE_DONE)
    {
        string err(sqlite3_errmsg(db.get()));
        throw runtime_error("SQLite error in statement \"" + sql + "\": " + err);
    }

    return res;
}

} /* namespace sqlpp */
