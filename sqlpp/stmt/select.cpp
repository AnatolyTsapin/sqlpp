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

SelectData::SelectData(const SelectData&) = default;
SelectData::SelectData(SelectData&&) = default;
SelectData& SelectData::operator=(const SelectData&) = default;
SelectData& SelectData::operator=(SelectData&&) = default;

void SelectData::addColumn(const string& tableName, const string& columnName)
{
    if(find(begin(tables), end(tables), tableName) == end(tables))
        tables.push_back(tableName);
    columns.push_back(tableName + "." + columnName);
}

void SelectData::addCondition(const condition::Data& cond)
{
    condition = cond;
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

    if(condition)
    {
        stream << " WHERE ";
        condition.dump(stream);
    }
}

Result SelectData::execute(const Database& db) const
{
    return Result();
}

} /* namespace stmt */
} /* namespace sqlpp */
