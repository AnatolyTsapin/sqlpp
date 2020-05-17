#ifndef SRC_SQLPP_EXPR_EXPRESSION_H_
#define SRC_SQLPP_EXPR_EXPRESSION_H_

#include <sqlpp/expr/node.h>

namespace sqlpp
{

namespace expr
{

template<typename T, typename V>
class Expression
{
    template<typename A, typename B>
    friend class Expression;

protected:
    Expression(const std::string& table, const std::string& field) :
        data(table, field)
    {}
    explicit Expression(Bind&& bind) :
        data(std::move(bind))
    {}

public:
    using ExpressionType = Expression<T, V>;

    template<typename E>
    Expression(UnaryOperator::Op op, E&& e) :
        data(op, std::forward<E>(e).data)
    {}

    template<typename E1, typename E2>
    Expression(BinaryOperator::Op op, E1&& e1, E2&& e2) :
        data(op, std::forward<E1>(e1).data, std::forward<E2>(e2).data)
    {}

    Expression(const Expression&) = default;
    Expression(Expression&&) = default;

    Expression& operator=(const Expression&) = default;
    Expression& operator=(Expression&&) = default;

    void dump(std::ostream& stream) const
    {
        data.dump(stream);
    }

protected:
    Data data;
};

template<typename V>
class Literal : public Expression<types::List<>, V>
{
public:
    using ExpressionType = Expression<types::List<>, V>;

    template<typename U, std::enable_if_t<std::is_same_v<V, DbType<U>>, int> = 0>
    Literal(const U& value) :
        ExpressionType(createBind(value))
    {}
};

template<template<typename...> typename S, typename... E>
using ExprResult = typename S<typename remove_cvref_t<E>::ExpressionType...>::Result;

template<template<typename...> typename S, typename... E>
using ExprTerm = typename S<typename remove_cvref_t<E>::ExpressionType...>::Term;

template<template<typename...> typename S, typename... E>
using LiteralTerm = Literal<ExprTerm<S, E...>>;

template<typename... E>
struct IntExpr;

template<typename... T>
struct IntExpr<Expression<T, Integer>...>
{
    using Result = Expression<types::Merge<T...>, Integer>;
    using Term = Integer;
};

template<typename... E>
struct TxtExpr;

template<typename... T>
struct TxtExpr<Expression<T, Text>...>
{
    using Result = Expression<types::Merge<T...>, Text>;
    using Term = Text;
};

template<typename... E>
struct NumExpr;

template<typename... T>
struct NumExpr<Expression<T, Integer>...>
{
    using Result = Expression<types::Merge<T...>, Integer>;
    using Term = Integer;
};

template<typename... T>
struct NumExpr<Expression<T, Real>...>
{
    using Result = Expression<types::Merge<T...>, Real>;
    using Term = Real;
};

template<typename E>
ExprResult<NumExpr, E> operator-(E&& e)
{
    return ExprResult<NumExpr, E>(UnaryOperator::Op::MINUS, std::forward<E>(e));
}

template<typename E>
ExprResult<NumExpr, E> operator+(E&& e)
{
    return ExprResult<NumExpr, E>(UnaryOperator::Op::PLUS, std::forward<E>(e));
}

template<typename E>
ExprResult<IntExpr, E> operator~(E&& e)
{
    return ExprResult<IntExpr, E>(UnaryOperator::Op::COMPLEMENT, std::forward<E>(e));
}

template<typename E1, typename E2>
ExprResult<NumExpr, E1, E2> operator*(E1&& e1, E2&& e2)
{
    return ExprResult<NumExpr, E1, E2>(BinaryOperator::Op::MUL, std::forward<E1>(e1), std::forward<E2>(e2));
}

template<typename E>
ExprResult<NumExpr, E> operator*(E&& e, LiteralTerm<NumExpr, E>&& l)
{
    return ExprResult<NumExpr, E>(BinaryOperator::Op::MUL, std::forward<E>(e), std::move(l));
}

template<typename E>
ExprResult<NumExpr, E> operator*(LiteralTerm<NumExpr, E>&& l, E&& e)
{
    return ExprResult<NumExpr, E>(BinaryOperator::Op::MUL, std::move(l), std::forward<E>(e));
}

template<typename E1, typename E2>
ExprResult<NumExpr, E1, E2> operator/(E1&& e1, E2&& e2)
{
    return ExprResult<NumExpr, E1, E2>(BinaryOperator::Op::DIV, std::forward<E1>(e1), std::forward<E2>(e2));
}

template<typename E>
ExprResult<NumExpr, E> operator/(E&& e, LiteralTerm<NumExpr, E>&& l)
{
    return ExprResult<NumExpr, E>(BinaryOperator::Op::DIV, std::forward<E>(e), std::move(l));
}

template<typename E>
ExprResult<NumExpr, E> operator/(LiteralTerm<NumExpr, E>&& l, E&& e)
{
    return ExprResult<NumExpr, E>(BinaryOperator::Op::DIV, std::move(l), std::forward<E>(e));
}

template<typename E1, typename E2>
ExprResult<IntExpr, E1, E2> operator%(E1&& e1, E2&& e2)
{
    return ExprResult<IntExpr, E1, E2>(BinaryOperator::Op::MOD, std::forward<E1>(e1), std::forward<E2>(e2));
}

template<typename E>
ExprResult<IntExpr, E> operator%(E&& e, LiteralTerm<IntExpr, E>&& l)
{
    return ExprResult<IntExpr, E>(BinaryOperator::Op::MOD, std::forward<E>(e), std::move(l));
}

template<typename E>
ExprResult<IntExpr, E> operator%(LiteralTerm<IntExpr, E>&& l, E&& e)
{
    return ExprResult<IntExpr, E>(BinaryOperator::Op::MOD, std::move(l), std::forward<E>(e));
}

template<typename E1, typename E2>
ExprResult<NumExpr, E1, E2> operator+(E1&& e1, E2&& e2)
{
    return ExprResult<NumExpr, E1, E2>(BinaryOperator::Op::PLUS, std::forward<E1>(e1), std::forward<E2>(e2));
}

template<typename E>
ExprResult<NumExpr, E> operator+(E&& e, LiteralTerm<NumExpr, E>&& l)
{
    return ExprResult<NumExpr, E>(BinaryOperator::Op::PLUS, std::forward<E>(e), std::move(l));
}

template<typename E>
ExprResult<NumExpr, E> operator+(LiteralTerm<NumExpr, E>&& l, E&& e)
{
    return ExprResult<NumExpr, E>(BinaryOperator::Op::PLUS, std::move(l), std::forward<E>(e));
}

template<typename E1, typename E2>
ExprResult<NumExpr, E1, E2> operator-(E1&& e1, E2&& e2)
{
    return ExprResult<NumExpr, E1, E2>(BinaryOperator::Op::MINUS, std::forward<E1>(e1), std::forward<E2>(e2));
}

template<typename E>
ExprResult<NumExpr, E> operator-(E&& e, LiteralTerm<NumExpr, E>&& l)
{
    return ExprResult<NumExpr, E>(BinaryOperator::Op::MINUS, std::forward<E>(e), std::move(l));
}

template<typename E>
ExprResult<NumExpr, E> operator-(LiteralTerm<NumExpr, E>&& l, E&& e)
{
    return ExprResult<NumExpr, E>(BinaryOperator::Op::MINUS, std::move(l), std::forward<E>(e));
}

template<typename E1, typename E2>
ExprResult<IntExpr, E1, E2> operator<<(E1&& e1, E2&& e2)
{
    return ExprResult<IntExpr, E1, E2>(BinaryOperator::Op::SHL, std::forward<E1>(e1), std::forward<E2>(e2));
}

template<typename E>
ExprResult<IntExpr, E> operator<<(E&& e, LiteralTerm<IntExpr, E>&& l)
{
    return ExprResult<IntExpr, E>(BinaryOperator::Op::SHL, std::forward<E>(e), std::move(l));
}

template<typename E>
ExprResult<IntExpr, E> operator<<(LiteralTerm<IntExpr, E>&& l, E&& e)
{
    return ExprResult<IntExpr, E>(BinaryOperator::Op::SHL, std::move(l), std::forward<E>(e));
}

template<typename E1, typename E2>
ExprResult<IntExpr, E1, E2> operator>>(E1&& e1, E2&& e2)
{
    return ExprResult<IntExpr, E1, E2>(BinaryOperator::Op::SHR, std::forward<E1>(e1), std::forward<E2>(e2));
}

template<typename E>
ExprResult<IntExpr, E> operator>>(E&& e, LiteralTerm<IntExpr, E>&& l)
{
    return ExprResult<IntExpr, E>(BinaryOperator::Op::SHR, std::forward<E>(e), std::move(l));
}

template<typename E>
ExprResult<IntExpr, E> operator>>(LiteralTerm<IntExpr, E>&& l, E&& e)
{
    return ExprResult<IntExpr, E>(BinaryOperator::Op::SHR, std::move(l), std::forward<E>(e));
}

template<typename E1, typename E2>
ExprResult<IntExpr, E1, E2> operator&(E1&& e1, E2&& e2)
{
    return ExprResult<IntExpr, E1, E2>(BinaryOperator::Op::BAND, std::forward<E1>(e1), std::forward<E2>(e2));
}

template<typename E>
ExprResult<IntExpr, E> operator&(E&& e, LiteralTerm<IntExpr, E>&& l)
{
    return ExprResult<IntExpr, E>(BinaryOperator::Op::BAND, std::forward<E>(e), std::move(l));
}

template<typename E>
ExprResult<IntExpr, E> operator&(LiteralTerm<IntExpr, E>&& l, E&& e)
{
    return ExprResult<IntExpr, E>(BinaryOperator::Op::BAND, std::move(l), std::forward<E>(e));
}

template<typename E1, typename E2>
ExprResult<IntExpr, E1, E2> operator|(E1&& e1, E2&& e2)
{
    return ExprResult<IntExpr, E1, E2>(BinaryOperator::Op::BOR, std::forward<E1>(e1), std::forward<E2>(e2));
}

template<typename E>
ExprResult<IntExpr, E> operator|(E&& e, LiteralTerm<IntExpr, E>&& l)
{
    return ExprResult<IntExpr, E>(BinaryOperator::Op::BOR, std::forward<E>(e), std::move(l));
}

template<typename E>
ExprResult<IntExpr, E> operator|(LiteralTerm<IntExpr, E>&& l, E&& e)
{
    return ExprResult<IntExpr, E>(BinaryOperator::Op::BOR, std::move(l), std::forward<E>(e));
}

} /* namespace expr */

} /* namespace sqlpp */

#endif /* SRC_SQLPP_EXPR_EXPRESSION_H_ */
