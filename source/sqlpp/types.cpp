#include "types.h"

#include <sqlite3.h>

#include <stdexcept>

namespace sqlpp {

void bind(sqlite3_stmt* stmt, int idx, const Integer& value) {
  if (sqlite3_bind_int64(stmt, idx, value) != SQLITE_OK)
    throw std::runtime_error("Cannot bind integer parameter #" +
                             std::to_string(idx));
}

void bind(sqlite3_stmt* stmt, int idx, const Real& value) {
  if (sqlite3_bind_double(stmt, idx, value) != SQLITE_OK)
    throw std::runtime_error("Cannot bind real parameter #" +
                             std::to_string(idx));
}

void bind(sqlite3_stmt* stmt, int idx, const Text& value) {
  if (sqlite3_bind_text(stmt, idx, value.c_str(), -1, SQLITE_TRANSIENT) !=
      SQLITE_OK)
    throw std::runtime_error("Cannot bind text parameter #" +
                             std::to_string(idx));
}

void bind(sqlite3_stmt* stmt, int idx, const Blob& value) {
  if (sqlite3_bind_blob(stmt, idx, value.data(), value.size(),
                        SQLITE_TRANSIENT) != SQLITE_OK)
    throw std::runtime_error("Cannot bind BLOB parameter #" +
                             std::to_string(idx));
}

}  // namespace sqlpp
