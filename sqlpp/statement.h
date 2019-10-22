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
    virtual ~Statement();

    Statement& operator=(const Statement&);
    Statement& operator=(Statement&&);

public:
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

class CreateTableData
{
public:
    CreateTableData(std::string const &tableName, bool ifNotExists);

    CreateTableData(CreateTableData const&);
    CreateTableData(CreateTableData&&);

    CreateTableData& operator=(CreateTableData const&);
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
class CreateTable : public Statement
{
protected:
    using Statement::Statement;
    CreateTable(const std::string& name, bool ifNotExists) :
        data(name, ifNotExists)
    {}

public:
    static CreateTable<V...> make(const Table<V...>& table, bool ifNotExists)
    {
        CreateTable<V...> ret(table.getName(), ifNotExists);
        ret.insertColumns(table);
        return ret;
    }

    ~CreateTable() override = default;

    void dump(std::ostream& stream) const override
    {
        data.dump(stream);
    }
    Result execute(const Database& db) const override
    {
        return data.execute(db);
    }

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

private:
    CreateTableData data;
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
class Insert : public Statement
{
protected:
    using Statement::Statement;
    explicit Insert(const std::string& tableName) :
        data(tableName)
    {}

public:
    static Insert<T> make(const T& table)
    {
        return Insert<T>(table.getName());
    }

    ~Insert() override = default;

    void dump(std::ostream& stream) const override
    {
        data.dump(stream);
    }
    Result execute(const Database& db) const override
    {
        return data.execute(db);
    }

    template<typename V, typename... VV>
    InsertRow<T> values(V&& value, VV&&... values) const
    {
        return InsertRow<T>(data, std::forward<V>(value), std::forward<VV>(values)...);
    }

private:
    InsertData data;
};

template<typename T>
class InsertRow : public Statement
{
protected:
    using Statement::Statement;

    template<typename... V>
    InsertRow(const InsertData& data, V&&... values) :
        data(data)
    {
        static_assert(std::tuple_size_v<std::tuple<V...>> == T::COLUMN_COUNT,
            "Values count does not match to columns count in the table");
        addValues(std::forward<V>(values)...);
    }

    template<typename... V>
    explicit InsertRow(InsertData&& data, V&&... values) :
        data(std::move(data))
    {
        static_assert(std::tuple_size_v<std::tuple<V...>> == T::COLUMN_COUNT,
            "Values count does not match to columns count in the table");
        addValues(std::forward<V>(values)...);
    }

    friend class Insert<T>;

public:
    ~InsertRow() override = default;

    void dump(std::ostream& stream) const override
    {
        data.dump(stream);
    }
    Result execute(const Database& db) const override
    {
        return data.execute(db);
    }

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
        addValues(std::forward<VV>(values)...);
    }
    void addValues()
    {}

private:
    InsertData data;
};

template<typename T>
class InsertValues : public Statement
{
protected:
    using Statement::Statement;

    template<typename... V>
    InsertValues(const std::string& tableName, const Value<T, V>&... values) :
        data(tableName)
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

    void dump(std::ostream& stream) const override
    {
        data.dump(stream);
    }
    Result execute(const Database& db) const override
    {
        return data.execute(db);
    }

private:
    template<typename V, typename... VV>
    void addValues(V&& value, VV&&... values)
    {
        data.addValue(value.getColumn().getName(), createBind(value.getValue()));
        addValues(std::forward<VV>(values)...);
    }
    void addValues()
    {}

private:
    InsertData data;
};

//template<typename V, typename... VV>
//InsertValues<T> Insert<T>::values(T&& value, TT&&... values)
//{
//    InsertValues ret;
//    ret.tableName = tableName;
//    ret.addValues(std::forward<T>(value), std::forward<TT>(values)...);
//    return ret;
//}

//class SelectWhere : public Statement
//{
//protected:
//    using Statement::Statement;
//    friend class Select;
//
//public:
//    ~SelectWhere() override;
//};
//
//class Select : public Statement
//{
//protected:
//    using Statement::Statement;
//
//public:
//    template<typename T, typename... TT>
//    static Select make(T&& t, TT&&... tt)
//    {
//        Select res(std::string("SELECT FROM "));
//        res.from(std::forward<T>(t), std::forward<TT>(tt)...);
//        return res;
//    }
//
//    ~Select() override;
//
//    SelectWhere where()
//    {
//        return SelectWhere(std::string(request));
//    }
//
//private:
//    template<typename T, typename... TT>
//    void from(T&& t, TT&&... tt)
//    {
//        fromHandler(t);
//        from(std::forward<TT>(tt)...);
//    }
//    void from()
//    {}
//
//    void fromHandler(const Table& table);
//    void fromHandler(const ColumnBase& column);
//};

} /* namespace stmt */

//template<typename... T>
//stmt::Select select(T&&... t)
//{
//    return stmt::Select::make(std::forward<T>(t)...);
//}

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
inline stmt::InsertValues<typename V::Table> insertValues(
    const V& value,
    const VV&... values)
{
    return stmt::InsertValues<typename V::Table>::make(value, values...);
}

} /* namespace sqlpp */

#endif /* SQLPP_STATEMENT_H_ */
