#ifndef SQLPP_VALUE_H_
#define SQLPP_VALUE_H_

#include <sqlpp/types.h>

#include <optional>

namespace sqlpp
{

template<typename T, typename V>
class Column;

template<typename T, typename V>
class Value
{
public:
    using Table = T;
    using ValueT = DbType<V>;

    Value(const Column<T, V>& column, ValueT&& value) :
        column(column), value(std::forward<ValueT>(value))
    {}

    const Column<T, V>& getColumn() const
    {
        return column;
    }

    const ValueT& getValue() const
    {
        return value;
    }

private:
    const Column<T, V>& column;
    ValueT value;
};

} /* namespace sqlpp */

#endif /* SQLPP_VALUE_H_ */
