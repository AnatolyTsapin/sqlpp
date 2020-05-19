#ifndef SRC_SQLPP_ASSIGNMENT_H_
#define SRC_SQLPP_ASSIGNMENT_H_

#include <sqlpp/expr/expression.h>

namespace sqlpp
{

template<typename T, typename V, size_t I>
class Column;

template<typename T, typename V, size_t I>
class Assignment
{
public:
    using Table = T;
    static constexpr size_t INDEX = I;
    using Expression = expr::Expression<types::MakeSet<T>, DbType<V>>;

    Assignment(const Column<T, V, I>& column, const Expression& expression) :
        column(column), expression(expression)
    {}

    Assignment(const Column<T, V, I>& column, Expression&& expression) :
        column(column), expression(std::move(expression))
    {}

    const Column<T, V, I>& getColumn() const
    {
        return column;
    }

    const Expression& getExpr() const &
    {
        return expression;
    }
    Expression&& getExpr() &&
    {
        return std::move(expression);
    }

private:
    const Column<T, V, I>& column;
    Expression expression;
};

} /* namespace sqlpp */

#endif /* SRC_SQLPP_ASSIGNMENT_H_ */
