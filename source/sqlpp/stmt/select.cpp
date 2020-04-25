#include <sqlpp/stmt/select.h>
#include <sqlpp/database.h>

#include <sstream>
#include <algorithm>

using namespace std;

namespace sqlpp
{
namespace stmt
{

SelectData::SelectData()
{}

SelectData::SelectData(const SelectData& other) :
    tables(other.tables),
    columns(other.columns),
    binds(other.binds),
    root(other.root->clone())
{}

SelectData::SelectData(SelectData&&) = default;

SelectData& SelectData::operator=(const SelectData& other)
{
    if(this != &other)
    {
        tables = other.tables;
        columns = other.columns;
        binds = other.binds;
        root = other.root->clone();
    }
    return *this;
}

SelectData& SelectData::operator=(SelectData&&) = default;

void SelectData::addColumn(const string& tableName, const string& columnName)
{
    tables.insert(tableName);
    columns.push_back(tableName + "." + columnName);
}

void SelectData::addCondition(const condition::Data& cond)
{
    tables.insert(cond.tables.begin(), cond.tables.end());
    binds.insert(binds.end(), cond.binds.begin(), cond.binds.end());
    root = cond.root->clone();
}

void SelectData::dump(ostream& stream) const
{
    stream << "SELECT ";
    bool first = true;
    for(auto&& c : columns)
    {
        if(!first)
            stream << ", ";
        first = false;
        stream << c;
    }
    stream << " FROM ";
    first = true;
    for(auto&& t : tables)
    {
        if(!first)
            stream << ", ";
        first = false;
        stream << t;
    }

    if(root)
    {
        stream << " WHERE ";
        root->dump(stream);
    }
}

Result SelectData::execute(const Database& db) const
{
    ostringstream ss;
    dump(ss);
    return binds.empty() ? db.execute(ss.str()) : db.execute(ss.str(), binds);
}

} /* namespace stmt */
} /* namespace sqlpp */
