#ifndef SRC_SQLPP_STMT_SELECT_H_
#define SRC_SQLPP_STMT_SELECT_H_

#include <sqlpp/stmt/common.h>

namespace sqlpp
{
namespace stmt
{

class SelectData
{
public:
    SelectData();

    SelectData(const SelectData&);
    SelectData(SelectData&&);

    SelectData& operator=(const SelectData&);
    SelectData& operator=(SelectData&&);

    void addColumn(const std::string& tableName, const std::string& columnName);

    void dump(std::ostream& stream) const;
    Result execute(const Database& db) const;

private:
    std::vector<std::string> tables;
    std::vector<std::string> columns;
};

template<typename C, typename... CC>
class Select final : public StatementD<SelectData>
{
private:
    using StatementD::StatementD;

    Select(const C& c, const CC&... cc)
    {
        selectColumns(c, cc...);
    }

public:
    using Tables = types::MakeList<TableType<C>, TableType<CC>...>;

    static Select<C, CC...> make(const C& c, const CC&... cc)
    {
        return Select<C, CC...>(c, cc...);
    }

    ~Select() override = default;

//    SelectWhere where()
//    {
//        return SelectWhere(std::string(request));
//    }

private:
    template<typename U, typename... UU>
    void selectColumns(const U& c, const UU&... cc)
    {
        selectColumn(c);
        if constexpr(types::PackSize<UU...>)
            selectColumns(cc...);
    }

    template<typename T, typename... V>
    void selectColumn(const Table<T, V...>& table)
    {
        data.addColumn(table.getName(), "*");
    }

    template<typename T, typename V>
    void selectColumn(const Column<T, V>& column)
    {
        data.addColumn(column.getTable().getName(), column.getName());
    }
};

} /* namespace stmt */

template<typename C, typename... CC>
inline stmt::Select<C, CC...> select(const C& column, const CC&... columns)
{
    return stmt::Select<C, CC...>::make(column, columns...);
}

} /* namespace sqlpp */

#endif /* SRC_SQLPP_STMT_SELECT_H_ */
