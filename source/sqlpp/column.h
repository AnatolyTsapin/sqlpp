#ifndef SQLPP_COLUMN_H_
#define SQLPP_COLUMN_H_

#include <sqlpp/value.h>
#include <sqlpp/types.h>
#include <sqlpp/expr/expression.h>

#include <string>

namespace sqlpp
{

template<typename T, typename V, size_t I>
class Column : public expr::Expression<types::MakeSet<T>, DbType<V>>
{
public:
    using TableType = T;
    using ValueType = types::MakeList<V>;
    using ExpressionType = expr::Expression<types::MakeSet<T>, DbType<V>>;

    Column(const T& table, const std::string& name) :
        ExpressionType(table.getName(), name),
        table(table), name(name)
    {}

    const T& getTable() const
    {
        return table;
    }

    const std::string& getName() const
    {
        return name;
    }

    auto operator <<=(const V& value) const
    {
        return Value<T, V, I>(*this, toDb(value));
    }

protected:
    const T& table;
    const std::string name;
};

} /* namespace sqlpp */

#endif /* SQLPP_COLUMN_H_ */
