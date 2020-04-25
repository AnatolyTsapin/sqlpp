#ifndef SQLPP_COLUMN_H_
#define SQLPP_COLUMN_H_

#include <sqlpp/value.h>
#include <sqlpp/types.h>
#include <sqlpp/condition.h>

#include <string>

namespace sqlpp
{

template<typename T, typename V>
class Column : public Term<types::MakeList<T>, DbType<V>>
{
public:
    using TableType = T;

    Column(const T& table, const std::string& name) :
        Term<types::MakeList<T>, DbType<V>>(table.getName(), name),
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

    Value<T, V> operator <<=(V&& value) const
    {
        return Value<T, V>(*this, toDb(std::forward<V>(value)));
    }

protected:
    const T& table;
    const std::string name;
};

} /* namespace sqlpp */

#endif /* SQLPP_COLUMN_H_ */
