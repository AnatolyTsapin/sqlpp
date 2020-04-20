#include <iostream>

#include <sqlpp.h>

using namespace std;
using namespace sqlpp;

struct Pack
{
    int id = 0;
    char value = '0';
};

struct NotPack
{
    std::string val;
};

namespace sqlpp
{
template<>
struct Converter<Pack>
{
    using DbType = Blob;
    static Blob toDb(const Pack& value)
    {
        return Blob();
    }
    static Pack fromDb(const Blob& blob)
    {
        return Pack();
    }
};
}

class MyTable final : public Table<MyTable, int, string>
{
public:
    MyTable() : Table("MyTable", {"id", "text"})
    {}

    Column<0> id = column<0>();
    Column<1> text = column<1>();
};

class YaTable final : public Table<YaTable, char, string, double>
{
public:
    YaTable() :
        Table("YaTable", {"id", "comment", "value"})
    {}
    ~YaTable() override = default;

    Column<0> id = column<0>();
    Column<1> comment = column<1>();
    Column<2> value = column<2>();
};

int main(int argc, char* argv[])
{
//    sqlite3* db = nullptr;
//    sqlite3_open("my.db", &db);
//    sqlite3_exec(db, "create table if not exists tbl (id integer, txt text)", nullptr, nullptr, nullptr);
//    sqlite3_exec(db, "insert into tbl values (1, \"Hi!\")", nullptr, nullptr, nullptr);
//    sqlite3_close(db);

    Database db("test.db");
    MyTable mt;
    YaTable yt;

    auto cStmt = createTable(mt);
    cStmt.execute(db);
    createTableIfNotExists(yt).execute(db);

    auto insStmt = insertInto(mt).values(10, "Hi"s);
    insStmt.execute(db);

    auto insStmt2 = insertValues(yt.id <<= '0', yt.comment <<= "Just another table"s);
    insStmt2.execute(db);

    auto insStmt3 = insertInto(yt);
    insStmt3.execute(db);

    auto selStmt = select(yt, mt.id).where('0' == yt.id && yt.id == mt.id || yt.comment != "Test"s);
    cout << selStmt << endl;

    return 0;
}
