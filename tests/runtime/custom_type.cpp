#include <iostream>
#include <cstring>

#include <sqlpp.h>

using namespace std;
using namespace sqlpp;

struct MyType
{
    int iValue = 0;
    std::string sValue;
};

bool operator==(const MyType& l, const MyType& r)
{
    return l.iValue == r.iValue && l.sValue == r.sValue;
}

ostream& operator<<(ostream& s, const MyType& v)
{
    s << "{ " << v.iValue << ", " << v.sValue << " }";
    return s;
}

namespace sqlpp
{

template<>
struct Converter<MyType>
{
    using DbType = Blob;
    static Blob toDb(const MyType& value)
    {
        size_t size = sizeof(int) + value.sValue.size();
        Blob res(size);
        memcpy(res.data(), &value.iValue, sizeof(int));
        memcpy(res.data() + sizeof(int), value.sValue.data(), value.sValue.size());
        return res;
    }
    static MyType fromDb(const Blob& value)
    {
        MyType res;
        memcpy(&res.iValue, value.data(), sizeof(int));
        size_t len = value.size() - sizeof(int);
        res.sValue.resize(len);
        memcpy(res.sValue.data(), value.data() + sizeof(int), len);
        return res;
    }
};

} // namespace sqlpp

class MyTable final : public Table<MyTable, int, MyType>
{
public:
    MyTable() : Table("MyTable", {"id", "data"})
    {}

    Column<0> id = column<0>();
    Column<1> data = column<1>();
};

int main(int argc, char* argv[])
try
{
    Database db(":memory:");
    MyTable mt;

    auto cStmt = createTable(mt);
    cStmt.execute(db);

    MyType in[] = {
        { 1, "Test" },
        { 2, "Some text" }
    };
    insertInto(mt).values(0, in[0]).execute(db);

    MyType in2 { 2, "Some text" };
    insertValues(mt.id <<= 1, mt.data <<= in[1]).execute(db);

    auto selStmt = select(mt);
    auto res = selStmt.executeT(db);
    for(size_t i = 0; i < res.count(); ++i)
        cout << "|" << res.name(i);
    cout << "|" << endl;

    while(res.hasData())
    {
        cout << "|" << res.get<0>().value();
        cout << "|" << res.get<1>().value();
        cout << "|" << endl;
        if(!(in[res.get<0>().value()] == res.get<1>().value()))
            throw runtime_error("Comparison error");
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
