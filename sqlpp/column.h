#ifndef SQLPP_COLUMN_H_
#define SQLPP_COLUMN_H_

#include "value.h"
#include "types.h"

#include <string>

namespace sqlpp
{

template<typename T, typename V>
class Column
{
public:
    using TableType = T;

    Column(const T& table, const std::string& name) :
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

    template<typename U>
    Value<T, V> operator <<(U&& value) const
    {
        return Value<T, V>(*this, toDb(std::forward<U>(value)));
    }

protected:
    const T& table;
    const std::string name;
};

} /* namespace sqlpp */

#endif /* SQLPP_COLUMN_H_ */
