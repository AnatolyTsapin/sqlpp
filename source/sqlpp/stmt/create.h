#ifndef SRC_SQLPP_STMT_CREATE_H_
#define SRC_SQLPP_STMT_CREATE_H_

#include <sqlpp/stmt/common.h>

namespace sqlpp {
namespace stmt {

class CreateTableData {
 public:
  CreateTableData(std::string const& tableName, bool ifNotExists);

  CreateTableData(const CreateTableData&);
  CreateTableData(CreateTableData&&);

  CreateTableData& operator=(const CreateTableData&);
  CreateTableData& operator=(CreateTableData&&);

  void addColumnDesc(const std::string& name, const std::string& type);

  void dump(std::ostream& stream) const;
  Result execute(const Database& db) const;

 private:
  struct ColumnDesc {
    std::string name;
    std::string type;

    ColumnDesc(const std::string& name, const std::string& type);
  };

  std::string tableName;
  bool ifNotExists;
  std::vector<ColumnDesc> columnDesc;
};

template <typename T, typename... V>
class CreateTable final : public StatementD<CreateTableData> {
 private:
  using StatementD::StatementD;
  CreateTable(const Table<T, V...>& table, bool ifNotExists)
      : StatementD(table.getName(), ifNotExists) {
    insertColumns(table);
  }

 public:
  static CreateTable<T, V...> make(const Table<T, V...>& table,
                                   bool ifNotExists) {
    return CreateTable<T, V...>(table, ifNotExists);
  }

  ~CreateTable() override = default;

 private:
  template <size_t N = 0>
  void insertColumns(const Table<T, V...>& table) {
    data.addColumnDesc(
        table.getColumnName(N),
        TypeName<types::Get<N, typename Table<T, V...>::Row>>::get());
    if constexpr (N + 1 < Table<T, V...>::COLUMN_COUNT)
      insertColumns<N + 1>(table);
  }
};

} /* namespace stmt */

template <typename T, typename... V>
inline stmt::CreateTable<T, V...> createTable(const Table<T, V...>& table) {
  return stmt::CreateTable<T, V...>::make(table, false);
}

template <typename T, typename... V>
inline stmt::CreateTable<T, V...> createTableIfNotExists(
    const Table<T, V...>& table) {
  return stmt::CreateTable<T, V...>::make(table, true);
}

} /* namespace sqlpp */

#endif /* SRC_SQLPP_STMT_CREATE_H_ */
