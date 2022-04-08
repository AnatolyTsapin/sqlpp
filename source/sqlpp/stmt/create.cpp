#include "create.h"

#include <sstream>

#include "../database.h"

namespace sqlpp::stmt {

CreateTableData::CreateTableData(std::string const& tableName, bool ifNotExists)
    : tableName(tableName), ifNotExists(ifNotExists) {}

CreateTableData::CreateTableData(CreateTableData const&) = default;
CreateTableData::CreateTableData(CreateTableData&&) = default;
CreateTableData& CreateTableData::operator=(CreateTableData const&) = default;
CreateTableData& CreateTableData::operator=(CreateTableData&&) = default;

void CreateTableData::addColumnDesc(const std::string& name,
                                    const std::string& type) {
  columnDesc.emplace_back(name, type);
}

void CreateTableData::dump(std::ostream& stream) const {
  stream << "CREATE TABLE ";
  if (ifNotExists) stream << "IF NOT EXISTS ";
  stream << tableName << " (";
  bool first = true;
  for (auto&& c : columnDesc) {
    if (!first) stream << ", ";
    first = false;
    stream << c.name << " " << c.type;
  }
  stream << ")";
}

Result CreateTableData::execute(const Database& db) const {
  std::ostringstream ss;
  dump(ss);
  return db.execute(ss.str());
}

CreateTableData::ColumnDesc::ColumnDesc(const std::string& name,
                                        const std::string& type)
    : name(name), type(type) {}

}  // namespace sqlpp::stmt
