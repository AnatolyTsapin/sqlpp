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
    Database db(":memory:");
    MyTable mt;
    YaTable yt;

    auto cStmt = createTable(mt);
    cStmt.execute(db);
    createTableIfNotExists(yt).execute(db);

    auto insStmt = insertInto(mt).values(10, "Hi"s);
    insStmt.execute(db);

    auto insStmt2 = insertValues(yt.id <<= '0', yt.comment <<= "Just another table"s);
    insStmt2.execute(db);

    auto insStmt3 = insertInto(yt).values('1', "One more row"s, 1.0);
    insStmt3.execute(db);

    auto selStmt = select(yt).where(0L == yt.id && (~(yt.id + 1L) == -mt.id || yt.comment != "Test"s));
    cout << selStmt << endl;
    selStmt.execute(db);

    auto selStmt2 = select(yt);
    auto res = selStmt2.execute(db);
    for(size_t i = 0; i < res.count(); ++i)
        cout << "|" << res.name(i);
    cout << "|" << endl;
    while(res.hasData())
    {
        cout << "|" << res.as<Integer>(0).value_or(0);
        cout << "|" << res.as<Text>(1).value_or("NULL");
        cout << "|" << res.as<Real>(2).value_or(0.0);
        cout << "|" << endl;
        res.next();
    }

    return 0;
}
