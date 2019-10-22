#include "types.h"

#include <sqlite3.h>

namespace sqlpp
{

void bind(sqlite3_stmt* stmt, int idx, const Integer& value)
{
    sqlite3_bind_int64(stmt, idx, value);
}

void bind(sqlite3_stmt* stmt, int idx, const Real& value)
{
    sqlite3_bind_double(stmt, idx, value);
}

void bind(sqlite3_stmt* stmt, int idx, const Text& value)
{
    sqlite3_bind_text(stmt, idx, value.c_str(), -1, SQLITE_TRANSIENT);
}

void bind(sqlite3_stmt* stmt, int idx, const Blob& value)
{
    sqlite3_bind_blob(stmt, idx, value.data(), value.size(), SQLITE_TRANSIENT);
}

} /* namespace sqlpp */
