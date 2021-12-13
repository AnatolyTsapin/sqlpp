#include <sqlpp/database.h>
#include <sqlpp/stmt/update.h>

#include <sstream>

using namespace std;

namespace sqlpp {
namespace stmt {

UpdateData::UpdateData(const string& tableName) : tableName(tableName) {}

UpdateData::UpdateData(const UpdateData& other)
    : tableName(other.tableName), binds(other.binds) {
  for (const auto& a : other.assignemts)
    assignemts.emplace_back(get<0>(a), get<1>(a)->clone());
  if (other.root) root = other.root->clone();
}

UpdateData::UpdateData(UpdateData&&) = default;

UpdateData& UpdateData::operator=(const UpdateData& other) {
  if (this != &other) {
    tableName = other.tableName;
    binds = other.binds;
    for (const auto& a : other.assignemts)
      assignemts.emplace_back(get<0>(a), get<1>(a)->clone());
    if (other.root) root = other.root->clone();
  }
  return *this;
}

UpdateData& UpdateData::operator=(UpdateData&&) = default;

void UpdateData::addAssignment(const string& column, const expr::Data& data) {
  addAssignment(column, expr::Data(data));
}

void UpdateData::addAssignment(const std::string& column, expr::Data&& data) {
  assignemts.emplace_back(column, move(data.root));
  binds.insert(binds.end(), make_move_iterator(data.binds.begin()),
               make_move_iterator(data.binds.end()));
}

void UpdateData::addCondition(const expr::Data& cond) {
  addCondition(expr::Data(cond));
}

void UpdateData::addCondition(expr::Data&& cond) {
  binds.insert(binds.end(), make_move_iterator(cond.binds.begin()),
               make_move_iterator(cond.binds.end()));
  root = move(cond.root);
}

void UpdateData::dump(ostream& stream) const {
  stream << "UPDATE " << tableName << " SET ";
  bool first = true;
  for (const auto& a : assignemts) {
    if (!first) stream << ", ";
    first = false;
    stream << get<0>(a) << " = ";
    get<1>(a)->dump(stream);
  }

  if (root) {
    stream << " WHERE ";
    root->dump(stream);
  }
}

Result UpdateData::execute(const Database& db) const {
  ostringstream ss;
  dump(ss);
  return db.execute(ss.str(), binds);
}

} /* namespace stmt */
} /* namespace sqlpp */
