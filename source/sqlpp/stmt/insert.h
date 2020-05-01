#ifndef SRC_SQLPP_STMT_INSERT_H_
#define SRC_SQLPP_STMT_INSERT_H_

#include <sqlpp/stmt/common.h>

namespace sqlpp
{
namespace stmt
{

class InsertData
{
public:
    explicit InsertData(const std::string& tableName);

    InsertData(const InsertData&);
    InsertData(InsertData&&);

    InsertData& operator=(const InsertData&);
    InsertData& operator=(InsertData&&);

    void addValue(const std::string& name, Bind bind);
    void addValue(Bind bind);

    void dump(std::ostream& stream) const;
    Result execute(const Database& db) const;

private:
    std::string tableName;
    std::vector<std::string> names;
    std::vector<Bind> binds;
};

template<typename T>
class InsertRow;

template<typename T>
class Insert final : public StatementD<InsertData>
{
private:
    using StatementD::StatementD;
    explicit Insert(const std::string& tableName) :
        StatementD(tableName)
    {}

public:
    static Insert<T> make(const T& table)
    {
        return Insert<T>(table.getName());
    }

    ~Insert() override = default;

    template<typename V, typename... VV>
    InsertRow<T> values(V&& value, VV&&... values) const &
    {
        return InsertRow<T>(data, std::forward<V>(value), std::forward<VV>(values)...);
    }

    template<typename V, typename... VV>
    InsertRow<T> values(V&& value, VV&&... values) &&
    {
        return InsertRow<T>(std::move(data), std::forward<V>(value), std::forward<VV>(values)...);
    }
};

template<typename T>
class InsertRow final : public StatementD<InsertData>
{
private:
    using StatementD::StatementD;

    template<typename... V>
    explicit InsertRow(const InsertData& data, V&&... values) :
        StatementD(data)
    {
        init(std::forward<V>(values)...);
    }

    template<typename... V>
    explicit InsertRow(InsertData&& data, V&&... values) :
        StatementD(std::move(data))
    {
        init(std::forward<V>(values)...);
    }

    template<typename... V>
    void init(V&&... values)
    {
        static_assert(types::PackSize<V...> == T::COLUMN_COUNT,
            "Values count does not match to columns count in the table");
        addValues(std::forward<V>(values)...);
    }

    friend class Insert<T>;

public:
    ~InsertRow() override = default;

private:
    template<typename V, typename... VV>
    void addValues(V&& value, VV&&... values)
    {
        constexpr size_t N = T::COLUMN_COUNT - types::PackSize<V, VV...>;
        static_assert(std::is_same_v<DbType<V>, typename types::Get<N, typename T::Row>>,
            "Value type does not match to column's one");
        data.addValue(createBind(std::forward<V>(value)));
        if constexpr(types::PackSize<VV...>)
            addValues(std::forward<VV>(values)...);
    }
};

template<typename T>
class InsertValues final : public StatementD<InsertData>
{
private:
    using StatementD::StatementD;

    template<typename... V>
    explicit InsertValues(const std::string& tableName, const Value<T, V>&... values) :
        StatementD(tableName)
    {
        addValues(values...);
    }

public:
    template<typename V, typename... VV>
    static InsertValues<T> make(const Value<T, V>& value, const Value<T, VV>&... values)
    {
        return InsertValues<T>(value.getColumn().getTable().getName(), value, values...);
    }

    ~InsertValues() override = default;

private:
    template<typename V, typename... VV>
    void addValues(V&& value, VV&&... values)
    {
        data.addValue(value.getColumn().getName(), createBind(value.getValue()));
        if constexpr(types::PackSize<VV...>)
            addValues(std::forward<VV>(values)...);
    }
};

} /* namespace stmt */

template<typename T, typename... V>
inline stmt::Insert<Table<T, V...>> insertInto(const Table<T, V...>& table)
{
    return stmt::Insert<Table<T, V...>>::make(table);
}

template<typename V, typename... VV>
inline stmt::InsertValues<typename V::Table> insertValues(const V& value, const VV&... values)
{
    return stmt::InsertValues<typename V::Table>::make(value, values...);
}

} /* namespace sqlpp */

#endif /* SRC_SQLPP_STMT_INSERT_H_ */
