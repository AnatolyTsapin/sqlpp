#ifndef SRC_SQLPP_EXPR_CONDITION_H_
#define SRC_SQLPP_EXPR_CONDITION_H_

#include <sqlpp/expr/expression.h>

namespace sqlpp
{

namespace stmt
{

template<typename T, typename V, typename C>
class SelectWhere;

template<typename T, typename C>
class UpdateWhere;

}

namespace expr
{

template<typename T>
class Condition : public Expression<T, bool>
{
    template<typename A>
    friend class Condition;

    template<typename B, typename C, typename D>
    friend class stmt::SelectWhere;

    template<typename E, typename F>
    friend class stmt::UpdateWhere;

public:
    using ExpressionType = Condition<T>;

    using Expression<T, bool>::Expression;

    Condition& operator=(const Condition&) = default;
    Condition& operator=(Condition&&) = default;
};

template<typename... C>
struct BoolExpr;

template<typename... T>
struct BoolExpr<Condition<T>...>
{
    using Tables = types::Merge<T...>;
};

template<template<typename...> typename S, typename... E>
using CondResult = Condition<ExprTables<S, E...>>;

template<typename C>
CondResult<BoolExpr, C> operator!(C&& c)
{
    return CondResult<BoolExpr, C>(UnaryOperator::Op::NOT, std::forward<C>(c));
}

template<typename E1, typename E2>
CondResult<AnyExpr, E1, E2> operator<(E1&& e1, E2&& e2)
{
    return CondResult<AnyExpr, E1, E2>(BinaryOperator::Op::LS, std::forward<E1>(e1), std::forward<E2>(e2));
}

template<typename E>
CondResult<AnyExpr, E> operator<(E&& e, LiteralTerm<AnyExpr, E>&& l)
{
    return CondResult<AnyExpr, E>(BinaryOperator::Op::LS, std::forward<E>(e), std::move(l));
}

template<typename E>
CondResult<AnyExpr, E> operator<(LiteralTerm<AnyExpr, E>&& l, E&& e)
{
    return CondResult<AnyExpr, E>(BinaryOperator::Op::LS, std::move(l), std::forward<E>(e));
}

template<typename E1, typename E2>
CondResult<AnyExpr, E1, E2> operator<=(E1&& e1, E2&& e2)
{
    return CondResult<AnyExpr, E1, E2>(BinaryOperator::Op::LE, std::forward<E1>(e1), std::forward<E2>(e2));
}

template<typename E>
CondResult<AnyExpr, E> operator<=(E&& e, LiteralTerm<AnyExpr, E>&& l)
{
    return CondResult<AnyExpr, E>(BinaryOperator::Op::LE, std::forward<E>(e), std::move(l));
}

template<typename E>
CondResult<AnyExpr, E> operator<=(LiteralTerm<AnyExpr, E>&& l, E&& e)
{
    return CondResult<AnyExpr, E>(BinaryOperator::Op::LE, std::move(l), std::forward<E>(e));
}

template<typename E1, typename E2>
CondResult<AnyExpr, E1, E2> operator>(E1&& e1, E2&& e2)
{
    return CondResult<AnyExpr, E1, E2>(BinaryOperator::Op::GT, std::forward<E1>(e1), std::forward<E2>(e2));
}

template<typename E>
CondResult<AnyExpr, E> operator>(E&& e, LiteralTerm<AnyExpr, E>&& l)
{
    return CondResult<AnyExpr, E>(BinaryOperator::Op::GT, std::forward<E>(e), std::move(l));
}

template<typename E>
CondResult<AnyExpr, E> operator>(LiteralTerm<AnyExpr, E>&& l, E&& e)
{
    return CondResult<AnyExpr, E>(BinaryOperator::Op::GT, std::move(l), std::forward<E>(e));
}

template<typename E1, typename E2>
CondResult<AnyExpr, E1, E2> operator>=(E1&& e1, E2&& e2)
{
    return CondResult<AnyExpr, E1, E2>(BinaryOperator::Op::GE, std::forward<E1>(e1), std::forward<E2>(e2));
}

template<typename E>
CondResult<AnyExpr, E> operator>=(E&& e, LiteralTerm<AnyExpr, E>&& l)
{
    return CondResult<AnyExpr, E>(BinaryOperator::Op::GE, std::forward<E>(e), std::move(l));
}

template<typename E>
CondResult<AnyExpr, E> operator>=(LiteralTerm<AnyExpr, E>&& l, E&& e)
{
    return CondResult<AnyExpr, E>(BinaryOperator::Op::GE, std::move(l), std::forward<E>(e));
}

template<typename E1, typename E2>
CondResult<AnyExpr, E1, E2> operator==(E1&& e1, E2&& e2)
{
    return CondResult<AnyExpr, E1, E2>(BinaryOperator::Op::EQ, std::forward<E1>(e1), std::forward<E2>(e2));
}

template<typename E>
CondResult<AnyExpr, E> operator==(E&& e, LiteralTerm<AnyExpr, E>&& l)
{
    return CondResult<AnyExpr, E>(BinaryOperator::Op::EQ, std::forward<E>(e), std::move(l));
}

template<typename E>
CondResult<AnyExpr, E> operator==(LiteralTerm<AnyExpr, E>&& l, E&& e)
{
    return CondResult<AnyExpr, E>(BinaryOperator::Op::EQ, std::move(l), std::forward<E>(e));
}

template<typename E1, typename E2>
CondResult<AnyExpr, E1, E2> operator!=(E1&& e1, E2&& e2)
{
    return CondResult<AnyExpr, E1, E2>(BinaryOperator::Op::NE, std::forward<E1>(e1), std::forward<E2>(e2));
}

template<typename E>
CondResult<AnyExpr, E> operator!=(E&& e, LiteralTerm<AnyExpr, E>&& l)
{
    return CondResult<AnyExpr, E>(BinaryOperator::Op::NE, std::forward<E>(e), std::move(l));
}

template<typename E>
CondResult<AnyExpr, E> operator!=(LiteralTerm<AnyExpr, E>&& l, E&& e)
{
    return CondResult<AnyExpr, E>(BinaryOperator::Op::NE, std::move(l), std::forward<E>(e));
}

template<typename C1, typename C2>
CondResult<BoolExpr, C1, C2> operator&&(C1&& c1, C2&& c2)
{
    return CondResult<BoolExpr, C1, C2>(BinaryOperator::Op::AND, std::forward<C1>(c1), std::forward<C2>(c2));
}

template<typename C1, typename C2>
CondResult<BoolExpr, C1, C2> operator||(C1&& c1, C2&& c2)
{
    return CondResult<BoolExpr, C1, C2>(BinaryOperator::Op::OR, std::forward<C1>(c1), std::forward<C2>(c2));
}

} /* namespace expr */

} /* namespace sqlpp */

#endif /* SRC_SQLPP_EXPR_CONDITION_H_ */
