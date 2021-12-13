#include <sqlpp/database.h>
#include <sqlpp/stmt/create.h>

#include <sstream>

using namespace std;

namespace sqlpp {
namespace stmt {

CreateTableData::CreateTableData(string const& tableName, bool ifNotExists)
    : tableName(tableName), ifNotExists(ifNotExists) {}

CreateTableData::CreateTableData(CreateTableData const&) = default;
CreateTableData::CreateTableData(CreateTableData&&) = default;
CreateTableData& CreateTableData::operator=(CreateTableData const&) = default;
CreateTableData& CreateTableData::operator=(CreateTableData&&) = default;

void CreateTableData::addColumnDesc(const string& name, const string& type) {
  columnDesc.emplace_back(name, type);
}

void CreateTableData::dump(ostream& stream) const {
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
  ostringstream ss;
  dump(ss);
  return db.execute(ss.str());
}

CreateTableData::ColumnDesc::ColumnDesc(const string& name, const string& type)
    : name(name), type(type) {}

} /* namespace stmt */
} /* namespace sqlpp */
