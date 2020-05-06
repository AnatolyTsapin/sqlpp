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
    sqlpp::createTableIfNotExists(another).execute(db);

#ifdef CHECK_INSERT_TYPE_PASS
    auto stmt = sqlpp::insertInto(test).values(0, "0"s, 0.0);
#endif

#ifdef CHECK_INSERT_TYPE_FAIL
    auto stmt = sqlpp::insertInto(test).values("1"s, 1.0, 1);
#endif

#ifdef CHECK_INSERT_COUNT_FAIL
    auto stmt = sqlpp::insertInto(test).values(1, "1"s, 1.0, 1);
#endif

#ifdef CHECK_INSERT_VALUES_PASS
    auto stmt = sqlpp::insertValues(test.id <<= 0, test.comment <<= "0"s);
#endif

#ifdef CHECK_INSERT_VALUES_TWICE_FAIL
    auto stmt = sqlpp::insertValues(test.id <<= 0, test.comment <<= "0"s, test.id <<= 1);
#endif

#ifdef CHECK_INSERT_TWO_TABLES_FAIL
    auto stmt = sqlpp::insertValues(test.id <<= 1, another.id <<= 2);
#endif

#ifdef CHECK_SELECT_CONDITION_PASS
    auto stmt = sqlpp::select(test.id).where(test.comment = another.value || test.value < 0 && another.id > 0);
#endif

#ifdef CHECK_SELECT_CONDITION_FAIL
    auto stmt = sqlpp::select(test).where(test.id == 0L && test.comment > test.value);
#endif

    return 0;
}
