#include <sqlpp/types.h>

#include <sqlite3.h>

#include <stdexcept>

using namespace std;

namespace sqlpp
{

void bind(sqlite3_stmt* stmt, int idx, const Integer& value)
{
    if(sqlite3_bind_int64(stmt, idx, value) != SQLITE_OK)
        throw runtime_error("Cannot bind integer parameter #" + to_string(idx));
}

void bind(sqlite3_stmt* stmt, int idx, const Real& value)
{
    if(sqlite3_bind_double(stmt, idx, value) != SQLITE_OK)
        throw runtime_error("Cannot bind real parameter #" + to_string(idx));
}

void bind(sqlite3_stmt* stmt, int idx, const Text& value)
{
    if(sqlite3_bind_text(stmt, idx, value.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
        throw runtime_error("Cannot bind text parameter #" + to_string(idx));
}

void bind(sqlite3_stmt* stmt, int idx, const Blob& value)
{
    if(sqlite3_bind_blob(stmt, idx, value.data(), value.size(), SQLITE_TRANSIENT) != SQLITE_OK)
        throw runtime_error("Cannot bind BLOB parameter #" + to_string(idx));
}

} /* namespace sqlpp */
