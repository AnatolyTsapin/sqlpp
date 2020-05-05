#include <iostream>

#include <sqlpp.h>

using namespace std;
using namespace sqlpp;

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
try
{
    Database db(":memory:");
    MyTable mt;
    YaTable yt;

    auto cStmt = createTable(mt);
    cStmt.execute(db);
    createTableIfNotExists(yt).execute(db);

    auto insStmt = insertInto(mt).values(10, "Hi"s);
    insStmt.execute(db);
    insStmt.execute(db);

    auto insStmt2 = insertValues(yt.id <<= '0', yt.comment <<= "Just another table"s);
    insStmt2.execute(db);

    auto insStmt3 = insertInto(yt).values('1', "One more row"s, 1.0);
    insStmt3.execute(db);

    auto selStmt = select(yt).where(0 == yt.id || yt.value == 1.2 && (~(yt.id + 1) == -mt.id || yt.comment != "Test"s));
    cout << selStmt << endl;
    selStmt.execute(db);

    auto selStmt2 = select(yt, mt.id);
    auto res = selStmt2.executeT(db);
    for(size_t i = 0; i < res.count(); ++i)
        cout << "|" << res.name(i);
    cout << "|" << endl;
    while(res.hasData())
    {
        cout << "|" << res.get<0>().value_or('c');
        cout << "|" << res.get<1>().value_or("NULL");
        cout << "|" << res.get<2>().value_or(3.14);
        cout << "|" << endl;
        res.next();
    }

    return 0;
}
catch(const exception& e)
{
    cerr << "Error: " << e.what() << endl;
    return 1;
}
catch(...)
{
    cerr << "Unknown error" << endl;
    return 2;
}
