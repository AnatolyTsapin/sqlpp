#ifndef SRC_SQLPP_STMT_SELECT_H_
#define SRC_SQLPP_STMT_SELECT_H_

#include <sqlpp/stmt/common.h>
#include <sqlpp/condition.h>

#include <unordered_set>

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

    void addCondition(const condition::Data& cond);
    void addCondition(condition::Data&& cond);

    void dump(std::ostream& stream) const;
    Result execute(const Database& db) const;

private:
    std::unordered_set<std::string> tables;
    std::vector<std::string> columns;
    std::vector<Bind> binds;
    condition::Node::Ptr root;
};

template<typename T, typename C>
class SelectWhere;

template<typename C, typename... CC>
class Select final : public StatementD<SelectData>
{
private:
    using StatementD::StatementD;

    explicit Select(const C& c, const CC&... cc)
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

    template<typename T>
    SelectWhere<Tables, T> where(const Condition<T>& condition) const &
    {
        return SelectWhere<Tables, T>(data, condition);
    }

    template<typename T>
    SelectWhere<Tables, T> where(const Condition<T>& condition) &&
    {
        return SelectWhere<Tables, T>(std::move(data), condition);
    }

    template<typename T>
    SelectWhere<Tables, T> where(Condition<T>&& condition) const &
    {
        return SelectWhere<Tables, T>(data, std::move(condition));
    }

    template<typename T>
    SelectWhere<Tables, T> where(Condition<T>&& condition) &&
    {
        return SelectWhere<Tables, T>(std::move(data), std::move(condition));
    }

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

template<typename T, typename C>
class SelectWhere final : public StatementD<SelectData>
{
private:
    using StatementD::StatementD;

    SelectWhere(const SelectData& data, const Condition<C>& condition) :
        StatementD(data)
    {
        init(condition);
    }

    SelectWhere(SelectData&& data, const Condition<C>& condition) :
        StatementD(std::move(data))
    {
        init(condition);
    }

    SelectWhere(const SelectData& data, Condition<C>&& condition) :
        StatementD(data)
    {
        init(std::move(condition));
    }

    SelectWhere(SelectData&& data, Condition<C>&& condition) :
        StatementD(std::move(data))
    {
        init(std::move(condition));
    }

    void init(const Condition<C>& condition)
    {
        data.addCondition(condition.data);
    }

    template<typename U, typename... UU>
    friend class Select;

public:
    ~SelectWhere() override = default;
};

} /* namespace stmt */

template<typename C, typename... CC>
inline stmt::Select<C, CC...> select(const C& column, const CC&... columns)
{
    return stmt::Select<C, CC...>::make(column, columns...);
}

} /* namespace sqlpp */

#endif /* SRC_SQLPP_STMT_SELECT_H_ */
