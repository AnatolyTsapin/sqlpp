#ifndef SRC_SQLPP_STMT_SELECT_H_
#define SRC_SQLPP_STMT_SELECT_H_

#include <sqlpp/stmt/common.h>
#include <sqlpp/expr/condition.h>

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

    void addCondition(const expr::Data& cond);
    void addCondition(expr::Data&& cond);

    void addGroupBy(const expr::Data& group);
    void addGroupBy(expr::Data&& group);

    void addOrderBy(const expr::Data& group);
    void addOrderBy(expr::Data&& group);

    void addLimit(size_t limit);

    void dump(std::ostream& stream) const;
    Result execute(const Database& db) const;

private:
    std::vector<std::string> columns;
    std::unordered_set<std::string> tables;
    expr::Node::Ptr where;
    std::vector<expr::Node::Ptr> groupBy;
    std::vector<expr::Node::Ptr> orderBy;
    std::optional<size_t> limit;
    std::vector<Bind> binds;
};

template<typename T, typename V>
class SelectLimit : public StatementD<SelectData>
{
public:
    using Tables = T;
    using Values = V;

    using StatementD<SelectData>::StatementD;

    ~SelectLimit() override = default;

    TypedResult<Values> executeT(const Database& db) const
    {
        return TypedResult<Values>(execute(db));
    }
};

template<typename T, typename V>
class SelectOrderBy : public SelectLimit<T, V>
{
public:
    using SelectLimit<T, V>::SelectLimit;

    ~SelectOrderBy() override = default;

    SelectLimit<T, V> limit(size_t l) const &
    {
        return addLimit(SelectData(this->data), l);
    }

    SelectLimit<T, V> limit(size_t l) &&
    {
        return addLimit(std::move(this->data), l);
    }

private:
    static SelectLimit<T, V> addLimit(SelectData&& data, size_t l)
    {
        data.addLimit(l);
        return SelectLimit<T, V>(std::move(data));
    }
};

template<typename T, typename V>
class SelectGroupBy : public SelectOrderBy<T, V>
{
    template<typename... E>
    using SelectOrderByType = SelectOrderBy<types::Merge<T, expr::ExprTables<expr::AllExpr, E...>>, V>;
public:
    using SelectOrderBy<T, V>::SelectOrderBy;

    template<typename E, typename... EE>
    SelectOrderByType<E, EE...> orderBy(E&& expression, EE&&... expressions) const &
    {
        return addGroupBy(SelectData(this->data), std::forward<E>(expression), std::forward<EE>(expressions)...);
    }

    template<typename E, typename... EE>
    SelectOrderByType<E, EE...> orderBy(E&& expression, EE&&... expressions) &&
    {
        return addOrderBy(std::move(this->data), std::forward<E>(expression), std::forward<EE>(expressions)...);
    }

private:
    template<typename E, typename... EE>
    static SelectOrderByType<E, EE...> addOrderBy(SelectData&& data, E&& expression, EE&&... expressions)
    {
        addOrder(data, std::forward<E>(expression));
        if constexpr(types::PackSize<EE...> == 0)
            return SelectOrderByType<E, EE...>(std::move(data));
        else
            return addOrderBy(std::move(data), std::forward<EE>(expressions)...);
    }

    template<typename E>
    static void addOrder(SelectData& data, E&& expression)
    {
        data.addOrderBy(std::forward<E>(expression).data);
    }
};

template<typename T, typename V>
class SelectWhere : public SelectGroupBy<T, V>
{
    template<typename... E>
    using SelectGroupByType = SelectGroupBy<types::Merge<T, expr::ExprTables<expr::AllExpr, E...>>, V>;
public:
    using SelectGroupBy<T, V>::SelectGroupBy;

    ~SelectWhere() override = default;

    template<typename E, typename... EE>
    SelectGroupByType<E, EE...> groupBy(E&& expression, EE&&... expressions) const &
    {
        return addGroupBy(SelectData(this->data), std::forward<E>(expression), std::forward<EE>(expressions)...);
    }

    template<typename E, typename... EE>
    SelectGroupByType<E, EE...> groupBy(E&& expression, EE&&... expressions) &&
    {
        return addGroupBy(std::move(this->data), std::forward<E>(expression), std::forward<EE>(expressions)...);
    }

private:
    template<typename E, typename... EE>
    static SelectGroupByType<E, EE...> addGroupBy(SelectData&& data, E&& expression, EE&&... expressions)
    {
        addGroup(data, std::forward<E>(expression));
        if constexpr(types::PackSize<EE...> == 0)
            return SelectGroupByType<E, EE...>(std::move(data));
        else
            return addGroupBy(std::move(data), std::forward<EE>(expressions)...);
    }

    template<typename E>
    static void addGroup(SelectData& data, E&& expression)
    {
        data.addGroupBy(std::forward<E>(expression).data);
    }
};

template<typename T, typename V>
class Select : public SelectWhere<T, V>
{
    template<typename C>
    using SelectWhereType = SelectWhere<types::Merge<T, expr::ExprTables<expr::BoolExpr, C>>, V>;
public:
    using SelectWhere<T, V>::SelectWhere;

    ~Select() override = default;

    template<typename R, typename... RR>
    static Select<T, V> make(R&& result, RR&&... results)
    {
        Select<T, V> ret;
        ret.addResults(std::forward<R>(result), std::forward<RR>(results)...);
        return ret;
    }

    template<typename C>
    SelectWhereType<C> where(C&& condition) const &
    {
        return addWhere(SelectData(this->data), std::forward<C>(condition));
    }

    template<typename C>
    SelectWhereType<C> where(C&& condition) &&
    {
        return addWhere(std::move(this->data), std::forward<C>(condition));
    }

private:
    template<typename R, typename... RR>
    void addResults(R&& result, RR&&... results)
    {
        addResult(std::forward<R>(result));
        if constexpr(types::PackSize<RR...>)
            addResults(std::forward<RR>(results)...);
    }

    template<typename B, typename... U>
    void addResult(const Table<B, U...>& table)
    {
        this->data.addColumn(table.getName(), "*");
    }

    template<typename B, typename U, size_t I>
    void addResult(const Column<B, U, I>& column)
    {
        this->data.addColumn(column.getTable().getName(), column.getName());
    }

    template<typename C>
    static SelectWhereType<C> addWhere(SelectData&& data, C&& condition)
    {
        data.addCondition(std::forward<C>(condition).data);
        return SelectWhereType<C>(std::move(data));
    }
};

} /* namespace stmt */

template<typename R, typename... RR>
inline auto select(R&& result, RR&&... results)
{
    using Tables = types::MakeSet<TableType<std::remove_cvref_t<R>>, TableType<std::remove_cvref_t<RR>>...>;
    using Values = types::Concat<ValueType<std::remove_cvref_t<R>>, ValueType<std::remove_cvref_t<RR>>...>;
    return stmt::Select<Tables, Values>::make(std::forward<R>(result), std::forward<RR>(results)...);
}

} /* namespace sqlpp */

#endif /* SRC_SQLPP_STMT_SELECT_H_ */
