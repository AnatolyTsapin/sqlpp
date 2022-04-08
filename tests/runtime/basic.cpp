#include <sqlpp.h>

#include <iostream>

using namespace sqlpp;
using namespace std::string_literals;

class MyTable final : public Table<MyTable, int, std::string> {
 public:
  MyTable() : Table("MyTable", {"id", "text"}) {}

  Column<0> id = column<0>();
  Column<1> text = column<1>();
};

class YaTable final : public Table<YaTable, char, std::string, double> {
 public:
  YaTable() : Table("YaTable", {"id", "comment", "value"}) {}
  ~YaTable() override = default;

  Column<0> id = column<0>();
  Column<1> comment = column<1>();
  Column<2> value = column<2>();
};

int main(int argc, char* argv[]) try {
  Database db(":memory:");
  MyTable mt;
  YaTable yt;

  auto cStmt = createTable(mt);
  cStmt.execute(db);
  createTableIfNotExists(yt).execute(db);

  auto insStmt = insertInto(mt).values(10, "Hi"s);
  insStmt.execute(db);
  insStmt.execute(db);

  auto insStmt2 =
      insertValues(yt.id <<= '0', yt.comment <<= "Just another table"s);
  insStmt2.execute(db);

  auto insStmt3 = insertInto(yt).values('1', "One more row"s, 1.0);
  insStmt3.execute(db);

  auto upStmt =
      update(yt.value = yt.value + 1.0, yt.id = yt.id + '0').where(yt.id > 0);
  std::cout << upStmt << std::endl;
  upStmt.execute(db);

  auto c = yt.id == 0;

  auto selStmt = select(yt).where(
      0 == yt.id && c ||
      yt.value == 1.2 && (~(yt.id + 1) == -mt.id || yt.comment != "Test"s));
  std::cout << selStmt << std::endl;
  selStmt.execute(db);

  auto selStmt2 = select(yt, mt.id);
  auto res = selStmt2.executeT(db);
  for (size_t i = 0; i < res.count(); ++i) std::cout << "|" << res.name(i);
  std::cout << "|" << std::endl;
  while (res.hasData()) {
    std::cout << "|" << res.get<0>().value_or('c');
    std::cout << "|" << res.get<1>().value_or("NULL");
    std::cout << "|" << res.get<2>().value_or(3.14);
    std::cout << "|" << std::endl;
    res.next();
  }

  auto selStmt3 = select(mt)
                      .where(yt.id == 0)
                      .groupBy(mt.id - 1, mt.text)
                      .orderBy(mt.id, yt.comment)
                      .limit(10);
  std::cout << selStmt3 << std::endl;

  return 0;
} catch (const std::exception& e) {
  std::cerr << "Error: " << e.what() << std::endl;
  return 1;
} catch (...) {
  std::cerr << "Unknown error" << std::endl;
  return 2;
}
