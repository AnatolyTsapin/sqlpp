#ifndef SQLPP_TABLE_H_
#define SQLPP_TABLE_H_

#include <sqlpp/column.h>
#include <sqlpp/types.h>

#include <array>
#include <string>

namespace sqlpp {

template <typename T, typename... V>
class Table {
 public:
  using TableType = T;
  using ValueType = types::MakeList<V...>;

  using Row = types::MakeList<DbType<V>...>;

  template <size_t N>
  using Field = types::Get<N, Row>;

  template <size_t N>
  using Column = sqlpp::Column<TableType, types::Get<N, ValueType>, N>;

  static constexpr size_t COLUMN_COUNT = types::PackSize<V...>;

  Table(std::string name, std::array<std::string, COLUMN_COUNT> columnNames)
      : name(std::move(name)), columnNames(std::move(columnNames)) {}
  virtual ~Table() = default;

  const std::string& getName() const { return name; }

  template <size_t N>
  Column<N> column() const {
    return Column<N>(static_cast<const TableType&>(*this), columnNames[N]);
  }

  const std::string& getColumnName(size_t i) const { return columnNames[i]; }

 private:
  const std::string name;
  const std::array<std::string, COLUMN_COUNT> columnNames;
};

} /* namespace sqlpp */

#endif /* SQLPP_TABLE_H_ */
