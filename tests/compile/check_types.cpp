#include <sqlpp.h>

using namespace std;

class TestTable final : public sqlpp::Table<TestTable, int, string, double>
{
public:
    TestTable() : Table("Test", {"id", "comment", "value"})
    {}

    Column<0> id = column<0>();
    Column<1> comment = column<1>();
    Column<2> value = column<2>();
};

class AnotherTable final : public sqlpp::Table<AnotherTable, int, string>
{
public:
    AnotherTable() : Table("Another", {"id", "value"})
    {}

    Column<0> id = column<0>();
    Column<1> value = column<1>();
};

int main(int argc, char* argv[])
{
    sqlpp::Database db(":memory:");

    TestTable test;
    AnotherTable another;

    sqlpp::createTable(test).execute(db);

#ifdef CHECK_INSERT_TYPE
    auto stmt = sqlpp::insertInto(test).values("1"s, 1.0, 1);
#endif

#ifdef CHECK_INSERT_COUNT
    auto stmt = sqlpp::insertInto(test).values(1, "1"s, 1.0, 1);
#endif

#ifdef CHECK_INSERT_TWO_TABLES
    auto stmt = sqlpp::insertValues(test.id <<= 1, another.id <<= 2);
#endif

#ifdef CHECK_SELECT_CONDITION
    auto stmt = sqlpp::select(test).where(test.id == 0L && test.comment > test.value);
#endif

    return 0;
}
