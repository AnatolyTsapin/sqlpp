#include <iostream>

#include <sqlpp.h>

using namespace std;
using namespace sqlpp;

//class MyTable : public Table<MyTable, int, string>
//{
//public:
//    MyTable() :
//        Table("MyTable", {"id", "text"}),
//        id(*this, "id"),
//        text(*this, "text")
//    {}
//    ~MyTable() override = default;
//
//    Column<0> id;
//    Column<1> text;
//};

struct Pack
{
    int id = 0;
    char value = '0';
};

namespace sqlpp
{
template<>
struct Converter<Pack>
{
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

class YaTable : public Table<char, string, double>
{
public:
    YaTable() :
        Table("YaTable", {"id", "comment", "value"}),
        id(*this, "id"),
        comment(*this, "comment"),
        value(*this, "value")
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
    Table<int, string> mt("MyTable", {"id", "text"});
    YaTable yt;

    createTable(mt).execute(db);
    createTableIfNotExists(yt).execute(db);

    auto insStmt = insertInto(mt).values(10, "Hi"s);
    insStmt.execute(db);

    auto insStmt2 = insertValues(yt.id << 0, yt.comment << "Just another table"s);
    insStmt2.execute(db);

    insertInto(yt).execute(db);

//    auto stmt = select(mt, mt.id).where();
//    stmt.dump(cout);

    return 0;
}
