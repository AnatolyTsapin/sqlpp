#include "result.h"

#include <sqlite3.h>

#include <stdexcept>

namespace sqlpp {

static void initValue(Integer& val, sqlite3_stmt* stmt, size_t i) {
  val = sqlite3_column_int64(stmt, i);
}

static void initValue(Real& val, sqlite3_stmt* stmt, size_t i) {
  val = sqlite3_column_double(stmt, i);
}

static void initValue(Text& val, sqlite3_stmt* stmt, size_t i) {
  val = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
}

static void initValue(Blob& val, sqlite3_stmt* stmt, size_t i) {
  size_t size = sqlite3_column_bytes(stmt, i);
  if (size == 0) return;
  auto ptr = static_cast<const std::byte*>(sqlite3_column_blob(stmt, i));
  val.assign(ptr, ptr + size);
}

Result::Result(sqlite3_stmt* stmt) : stmt(stmt) {}

Result::Result(Result&& other) {
  std::swap(stmt, other.stmt);
  std::swap(status, other.status);
}

Result::~Result() {
  if (stmt) sqlite3_finalize(stmt);
}

Result& Result::operator=(Result&& other) {
  if (this != &other) {
    std::swap(stmt, other.stmt);
    std::swap(status, other.status);
    if (other.stmt) {
      sqlite3_finalize(other.stmt);
      other.stmt = nullptr;
      other.status = NO_STATUS;
    }
  }
  return *this;
}

Result::operator bool() const {
  return status == SQLITE_DONE || status == SQLITE_ROW;
}

void Result::next() { status = sqlite3_step(stmt); }

bool Result::hasData() const { return status == SQLITE_ROW; }

size_t Result::count() { return sqlite3_column_count(stmt); }

std::string Result::name(size_t i) {
  if (i >= count()) throw std::out_of_range("Incorrect column index");
  return sqlite3_column_name(stmt, i);
}

template <typename R>
std::optional<R> Result::as(size_t i) {
  if (i >= count()) throw std::out_of_range("Incorrect column index");

  std::optional<R> res;
  if (sqlite3_column_type(stmt, i) != SQLITE_NULL) {
    res.emplace();
    initValue(res.value(), stmt, i);
  }
  return res;
}

template std::optional<Integer> Result::as(size_t i);
template std::optional<Real> Result::as(size_t i);
template std::optional<Text> Result::as(size_t i);
template std::optional<Blob> Result::as(size_t i);

}  // namespace sqlpp
