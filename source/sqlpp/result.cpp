#include <sqlpp/result.h>

#include <sqlite3.h>

#include <stdexcept>

using namespace std;

namespace sqlpp
{

Result::Result(sqlite3_stmt* stmt) :
    stmt(stmt)
{}

Result::Result(Result&& other)
{
    swap(stmt, other.stmt);
}

Result::~Result()
{
    if(stmt)
        sqlite3_finalize(stmt);
}

Result& Result::operator=(Result&& other)
{
    if(this != &other)
    {
        swap(stmt, other.stmt);
        if(other.stmt)
        {
            sqlite3_finalize(other.stmt);
            other.stmt = nullptr;
        }
    }
    return *this;
}

Result::operator bool() const
{
    return status == SQLITE_DONE || status == SQLITE_ROW;
}

void Result::next()
{
    status = sqlite3_step(stmt);
}

bool Result::hasData() const
{
    return status == SQLITE_ROW;
}

size_t Result::count()
{
    return sqlite3_column_count(stmt);
}

string Result::name(size_t i)
{
    if(i >= count())
        throw out_of_range("Incorrect column index");
    return sqlite3_column_name(stmt, i);
}

optional<Integer> Result::asInteger(size_t i)
{
    if(i >= count())
        throw out_of_range("Incorrect column index");

    optional<Integer> res;
    if(sqlite3_column_type(stmt, i) != SQLITE_NULL)
        res = sqlite3_column_int64(stmt, i);
    return res;
}
std::optional<Real> Result::asReal(size_t i)
{
    if(i >= count())
        throw out_of_range("Incorrect column index");

    optional<Real> res;
    if(sqlite3_column_type(stmt, i) != SQLITE_NULL)
        res = sqlite3_column_double(stmt, i);
    return res;
}

std::optional<Text> Result::asText(size_t i)
{
    if(i >= count())
        throw out_of_range("Incorrect column index");

    optional<Text> res;
    if(sqlite3_column_type(stmt, i) != SQLITE_NULL)
        res = Text(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
    return res;
}

std::optional<Blob> Result::asBlob(size_t i)
{
    if(i >= count())
        throw out_of_range("Incorrect column index");

    optional<Blob> res;
    if(sqlite3_column_type(stmt, i) != SQLITE_NULL)
    {
        size_t size = sqlite3_column_bytes(stmt, i);
        if(size == 0)
        {
            res = Blob();
        }
        else
        {
            auto ptr = static_cast<const std::byte*>(sqlite3_column_blob(stmt, i));
            res = Blob(ptr, ptr + size);
        }
    }
    return res;
}

} /* namespace sqlpp */
