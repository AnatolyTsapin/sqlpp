#ifndef SRC_SQLPP_CONDITION_H_
#define SRC_SQLPP_CONDITION_H_

#include <sqlpp/column.h>

namespace sqlpp
{

template<typename T>
class Condition
{
    using Tables = T;
};

template<typename T1, typename V1, typename T2, typename V2>
auto operator==(const Column<T1, V1>& c1, const Column<T2, V2>& c2)
{
    return Condition<types::MakeList<T1, T2>>();
}

} /* namespace sqlpp */

#endif /* SRC_SQLPP_CONDITION_H_ */
