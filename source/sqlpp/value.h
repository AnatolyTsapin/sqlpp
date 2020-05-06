#ifndef SQLPP_VALUE_H_
#define SQLPP_VALUE_H_

#include <sqlpp/types.h>

namespace sqlpp
{

template<typename T, typename V, size_t I>
class Column;

template<typename T, typename V, size_t I>
class Value
{
public:
    using Table = T;
    using ValueT = DbType<V>;
    static constexpr size_t INDEX = I;

    Value(const Column<T, V, I>& column, ValueT&& value) :
        column(column), value(std::forward<ValueT>(value))
    {}

    const Column<T, V, I>& getColumn() const
    {
        return column;
    }

    const ValueT& getValue() const &
    {
        return value;
    }

    ValueT&& getValue() &&
    {
        return std::move(value);
    }

private:
    const Column<T, V, I>& column;
    ValueT value;
};

} /* namespace sqlpp */

#endif /* SQLPP_VALUE_H_ */
