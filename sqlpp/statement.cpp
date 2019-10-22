#include "statement.h"
#include "database.h"

#include <sstream>

#include <sqlite3.h>

using namespace std;

namespace sqlpp
{

Statement::Statement() = default;
Statement::Statement(Statement&&) = default;
Statement::Statement(const Statement&) = default;
Statement::~Statement() = default;
Statement& Statement::operator=(const Statement&) = default;
Statement& Statement::operator=(Statement&&) = default;

namespace stmt
{

CreateTableData::CreateTableData(string const &tableName, bool ifNotExists) :
    tableName(tableName), ifNotExists(ifNotExists)
{}

CreateTableData::CreateTableData(CreateTableData const&) = default;
CreateTableData::CreateTableData(CreateTableData&&) = default;
CreateTableData& CreateTableData::operator=(CreateTableData const&) = default;
CreateTableData& CreateTableData::operator=(CreateTableData&&) = default;

void CreateTableData::addColumnDesc(const string& name, const string& type)
{
    columnDesc.emplace_back(name, type);
}

void CreateTableData::dump(ostream& stream) const
{
    stream << "CREATE TABLE ";
    if(ifNotExists)
        stream << "IF NOT EXISTS ";
    stream << tableName << " (";
    bool first = true;
    for(auto&& c : columnDesc)
    {
        if(!first)
            stream << ", ";
        first = false;
        stream << c.name << " " << c.type;
    }
    stream << ")";
}

Result CreateTableData::execute(const Database& db) const
{
    ostringstream ss;
    dump(ss);
    return db.execute(ss.str());
}

CreateTableData::ColumnDesc::ColumnDesc(const string& name, const string& type) :
    name(name), type(type)
{}

InsertData::InsertData(const string& tableName) :
    tableName(tableName)
{}

InsertData::InsertData(const InsertData&) = default;
InsertData::InsertData(InsertData&&) = default;
InsertData& InsertData::operator=(const InsertData&) = default;
InsertData& InsertData::operator=(InsertData&&) = default;

void InsertData::addValue(const string& name, Bind bind)
{
    names.emplace_back(name);
    binds.emplace_back(move(bind));
}

void InsertData::addValue(Bind bind)
{
    binds.emplace_back(move(bind));
}

void InsertData::dump(ostream& stream) const
{
    stream << "INSERT INTO " << tableName;
    if(binds.empty())
    {
        stream << " DEFAULT VALUES";
    }
    else if(names.empty())
    {
        stream << " VALUES (";
        for(size_t i = 0; i < binds.size(); ++i)
        {
            if(i != 0)
                stream << ", ";
            stream << "?";
        }
        stream << ")";
    }
    else
    {
        stream << " (";
        bool first = true;
        stringstream valStream;
        for(auto&& n : names)
        {
            if(!first)
            {
                stream << ", ";
                valStream << ", ";
            }
            first = false;
            stream << n;
            valStream << "?";
        }
        stream << ") VALUES (" << valStream.str() << ")";
    }
}

Result InsertData::execute(const Database& db) const
{
    ostringstream ss;
    dump(ss);
    return db.execute(ss.str(), binds);
}

} /* namespace stmt */

} /* namespace sqlpp */
