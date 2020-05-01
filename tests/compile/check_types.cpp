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

int main(int argc, char* argv[])
{
    sqlpp::Database db(":memory:");

    TestTable test;

    sqlpp::createTable(test).execute(db);

#ifdef CHECK_INSERT_TYPE
    auto stmt = sqlpp::insertInto(test).values("1"s, 1.0, 1);
#endif

#ifdef CHECK_INSERT_COUNT
    auto stmt = sqlpp::insertInto(test).values(1, "1"s, 1.0, 1);
#endif

#ifdef CHECK_SELECT_CONDITION
    auto stmt = sqlpp::select(test).where(test.id == 0 && test.comment > test.value);
#endif

    return 0;
}
