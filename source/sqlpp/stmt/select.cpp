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
    binds(other.binds)
{
    if(other.root)
        root = other.root->clone();
}

SelectData::SelectData(SelectData&&) = default;

SelectData& SelectData::operator=(const SelectData& other)
{
    if(this != &other)
    {
        tables = other.tables;
        columns = other.columns;
        binds = other.binds;
        if(other.root)
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

void SelectData::addCondition(const expr::Data& cond)
{
    addCondition(expr::Data(cond));
}

void SelectData::addCondition(expr::Data&& cond)
{
    tables.insert(make_move_iterator(cond.tables.begin()), make_move_iterator(cond.tables.end()));
    binds.insert(binds.end(), make_move_iterator(cond.binds.begin()), make_move_iterator(cond.binds.end()));
    root = move(cond.root);
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
    return db.execute(ss.str(), binds);
}

} /* namespace stmt */
} /* namespace sqlpp */
