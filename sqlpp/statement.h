#ifndef SQLPP_STATEMENT_H_
#define SQLPP_STATEMENT_H_

#include "table.h"
#include "result.h"

#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <tuple>

namespace sqlpp
{

class Database;

class Statement
{
protected:
    Statement();
    Statement(const Statement&);
    Statement(Statement&&);

public:
    virtual ~Statement();

    Statement& operator=(const Statement&);
    Statement& operator=(Statement&&);

    virtual void dump(std::ostream& stream) const = 0;
    virtual Result execute(const Database& db) const = 0;
};

inline std::ostream& operator<<(std::ostream& stream, const Statement& stmt)
{
    stmt.dump(stream);
    return stream;
}

namespace stmt
{

template<typename D>
class StatementD : public Statement
{
protected:
    StatementD(const StatementD&) = default;
    StatementD(StatementD&&) = default;

    template<typename... A>
    StatementD(A&&... data) :
        data(std::forward<A>(data)...)
    {}

public:
    ~StatementD() override = default;

    StatementD& operator=(const StatementD&) = default;
    StatementD& operator=(StatementD&&) = default;

    void dump(std::ostream& stream) const override
    {
        data.dump(stream);
    }
    Result execute(const Database& db) const override
    {
        return data.execute(db);
    }

protected:
    D data;
};

class CreateTableData
{
public:
    CreateTableData(std::string const &tableName, bool ifNotExists);

    CreateTableData(const CreateTableData&);
    CreateTableData(CreateTableData&&);

    CreateTableData& operator=(const CreateTableData&);
    CreateTableData& operator=(CreateTableData&&);

    void addColumnDesc(const std::string& name, const std::string& type);

    void dump(std::ostream& stream) const;
    Result execute(const Database& db) const;

private:
    struct ColumnDesc
    {
        std::string name;
        std::string type;

        ColumnDesc(const std::string& name, const std::string& type);
    };

    std::string tableName;
    bool ifNotExists;
    std::vector<ColumnDesc> columnDesc;
};

template<typename... V>
class CreateTable final : public StatementD<CreateTableData>
{
private:
    using StatementD::StatementD;
    CreateTable(const Table<V...>& table, bool ifNotExists) :
        StatementD(table.getName(), ifNotExists)
    {
        insertColumns(table);
    }

public:
    static CreateTable<V...> make(const Table<V...>& table, bool ifNotExists)
    {
        return CreateTable<V...>(table, ifNotExists);
    }

    ~CreateTable() override = default;

private:
    template<size_t N = 0>
    void insertColumns(const Table<V...>& table)
    {
        data.addColumnDesc(
            table.getColumnName(N),
            TypeName<std::tuple_element_t<N, typename Table<V...>::Row>>::get());
        if constexpr(N + 1 < Table<V...>::COLUMN_COUNT)
            insertColumns<N + 1>(table);
    }
};

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
    InsertRow(const InsertData& data, V&&... values) :
        StatementD(data)
    {
        static_assert(std::tuple_size_v<std::tuple<V...>> == T::COLUMN_COUNT,
            "Values count does not match to columns count in the table");
        addValues(std::forward<V>(values)...);
    }

    template<typename... V>
    InsertRow(InsertData&& data, V&&... values) :
        StatementD(std::move(data))
    {
        static_assert(std::tuple_size_v<std::tuple<V...>> == T::COLUMN_COUNT,
            "Values count does not match to columns count in the table");
        addValues(std::forward<V>(values)...);
    }

    friend class Insert<T>;

public:
    ~InsertRow() override = default;

private:
    template<typename U>
    using ValType = typename Converter<U>::DbType;
    template<size_t N>
    using ColType = std::tuple_element_t<N, typename T::Row>;

    template<typename V, typename... VV>
    void addValues(V&& value, VV&&... values)
    {
        constexpr size_t N = T::COLUMN_COUNT - std::tuple_size_v<std::tuple<V, VV...>>;
        static_assert(std::is_same_v<ValType<V>, ColType<N>>,
            "Value type does not match to column's one");
        data.addValue(createBind(std::forward<V>(value)));
        if constexpr(std::tuple_size_v<std::tuple<VV...>>)
            addValues(std::forward<VV>(values)...);
    }
};

template<typename T>
class InsertValues final : public StatementD<InsertData>
{
private:
    using StatementD::StatementD;

    template<typename... V>
    InsertValues(const std::string& tableName, const Value<T, V>&... values) :
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
        if constexpr(std::tuple_size_v<std::tuple<VV...>>)
            addValues(std::forward<VV>(values)...);
    }
};

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
        if constexpr(std::tuple_size_v<std::tuple<UU...>>)
            selectColumns(cc...);
    }

    template<typename... V>
    void selectColumn(const Table<V...>& table)
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

template<typename... V>
inline stmt::CreateTable<V...> createTable(const Table<V...>& table)
{
    return stmt::CreateTable<V...>::make(table, false);
}

template<typename... V>
inline stmt::CreateTable<V...> createTableIfNotExists(const Table<V...>& table)
{
    return stmt::CreateTable<V...>::make(table, true);
}

template<typename... V>
inline stmt::Insert<Table<V...>> insertInto(const Table<V...>& table)
{
    return stmt::Insert<Table<V...>>::make(table);
}

template<typename V, typename... VV>
inline stmt::InsertValues<typename V::Table> insertValues(const V& value, const VV&... values)
{
    return stmt::InsertValues<typename V::Table>::make(value, values...);
}

template<typename C, typename... CC>
inline stmt::Select<C, CC...> select(const C& column, const CC&... columns)
{
    return stmt::Select<C, CC...>::make(column, columns...);
}

} /* namespace sqlpp */

#endif /* SQLPP_STATEMENT_H_ */
