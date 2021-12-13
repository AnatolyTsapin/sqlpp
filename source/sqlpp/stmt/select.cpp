#include <sqlpp/database.h>
#include <sqlpp/stmt/select.h>

#include <algorithm>
#include <sstream>

using namespace std;

namespace sqlpp {
namespace stmt {

SelectData::SelectData() {}

SelectData::SelectData(const SelectData& other)
    : columns(other.columns), tables(other.tables), binds(other.binds) {
  if (other.where) where = other.where->clone();
  for (const auto& g : other.groupBy) groupBy.emplace_back(g->clone());
  for (const auto& o : other.orderBy) orderBy.emplace_back(o->clone());
}

SelectData::SelectData(SelectData&&) = default;

SelectData& SelectData::operator=(const SelectData& other) {
  if (this != &other) {
    columns = other.columns;
    tables = other.tables;
    if (other.where)
      where = other.where->clone();
    else
      where.reset();
    groupBy.clear();
    for (const auto& g : other.groupBy) groupBy.emplace_back(g->clone());
    orderBy.clear();
    for (const auto& o : other.orderBy) orderBy.emplace_back(o->clone());
    binds = other.binds;
  }
  return *this;
}

SelectData& SelectData::operator=(SelectData&&) = default;

void SelectData::addColumn(const string& tableName, const string& columnName) {
  columns.push_back(tableName + "." + columnName);
  tables.insert(tableName);
}

void SelectData::addCondition(const expr::Data& cond) {
  addCondition(expr::Data(cond));
}

void SelectData::addCondition(expr::Data&& cond) {
  tables.insert(make_move_iterator(cond.tables.begin()),
                make_move_iterator(cond.tables.end()));
  where = move(cond.root);
  binds.insert(binds.end(), make_move_iterator(cond.binds.begin()),
               make_move_iterator(cond.binds.end()));
}

void SelectData::addGroupBy(const expr::Data& group) {
  addGroupBy(expr::Data(group));
}

void SelectData::addGroupBy(expr::Data&& group) {
  groupBy.emplace_back(std::move(group.root));
  binds.insert(binds.end(), make_move_iterator(group.binds.begin()),
               make_move_iterator(group.binds.end()));
}

void SelectData::addOrderBy(const expr::Data& order) {
  addOrderBy(expr::Data(order));
}

void SelectData::addOrderBy(expr::Data&& order) {
  orderBy.emplace_back(std::move(order.root));
  binds.insert(binds.end(), make_move_iterator(order.binds.begin()),
               make_move_iterator(order.binds.end()));
}

void SelectData::addLimit(size_t l) { limit = l; }

void SelectData::dump(ostream& stream) const {
  stream << "SELECT ";
  bool first = true;
  for (const auto& c : columns) {
    if (!first) stream << ", ";
    first = false;
    stream << c;
  }
  stream << " FROM ";
  first = true;
  for (auto&& t : tables) {
    if (!first) stream << ", ";
    first = false;
    stream << t;
  }

  if (where) {
    stream << " WHERE ";
    where->dump(stream);
  }

  if (!groupBy.empty()) {
    stream << " GROUP BY ";
    first = true;
    for (const auto& g : groupBy) {
      if (!first) stream << ", ";
      first = false;
      g->dump(stream);
    }
  }

  if (!orderBy.empty()) {
    stream << " ORDER BY ";
    first = true;
    for (const auto& o : orderBy) {
      if (!first) stream << ", ";
      first = false;
      o->dump(stream);
    }
  }

  if (limit) stream << " LIMIT " << *limit;
}

Result SelectData::execute(const Database& db) const {
  ostringstream ss;
  dump(ss);
  return db.execute(ss.str(), binds);
}

} /* namespace stmt */
} /* namespace sqlpp */
