#ifndef SRC_SQLPP_EXPR_TERM_H_
#define SRC_SQLPP_EXPR_TERM_H_

#include <sqlpp/expr/node.h>

namespace sqlpp
{

namespace expr
{

template<typename T, typename V>
class Term
{
    template<typename A, typename B>
    friend class Term;

    template<typename A>
    friend class Condition;

public:
    Term(const std::string& table, const std::string& field) :
        data(table, field)
    {}
    explicit Term(Bind&& bind) :
        data(std::move(bind))
    {}

    Term(UnaryOperator::Op op, const Term<T, V>& t) :
        data(op, t.data)
    {}
    Term(UnaryOperator::Op op, Term<T, V>&& t) :
        data(op, std::move(t.data))
    {}

    template<typename T1, typename T2>
    Term(BinaryOperator::Op op, const Term<T1, V>& t1, const Term<T2, V>& t2) :
        data(op, t1.data, t2.data)
    {}
    template<typename T1, typename T2>
    Term(BinaryOperator::Op op, Term<T1, V>&& t1, const Term<T2, V>& t2) :
        data(op, std::move(t1.data), t2.data)
    {}
    template<typename T1, typename T2>
    Term(BinaryOperator::Op op, const Term<T1, V>& t1, Term<T2, V>&& t2) :
        data(op, t1.data, std::move(t2.data))
    {}
    template<typename T1, typename T2>
    Term(BinaryOperator::Op op, Term<T1, V>&& t1, Term<T2, V>&& t2) :
        data(op, std::move(t1.data), std::move(t2.data))
    {}

private:
    Data data;
};

template<typename V>
class Literal : public Term<types::List<>, V>
{
    using Base = Term<types::List<>, V>;

public:
    template<typename U, std::enable_if_t<std::is_same_v<V, DbType<U>>, int> = 0>
    Literal(const U& value) :
        Base(createBind(value))
    {}
};

#define UN_OP(func, op) \
template<typename T, typename V> \
auto func(const Term<T, V>& t) \
{ \
    return Term<T, V>(UnaryOperator::Op::op, t); \
} \
template<typename T, typename V> \
auto func(Term<T, V>&& t) \
{ \
    return Term<T, V>(UnaryOperator::Op::op, std::move(t)); \
}

UN_OP(operator-, MINUS)
UN_OP(operator+, PLUS)
UN_OP(operator~, COMPLEMENT)
UN_OP(operator!, NOT)

#undef UN_OP

#define BIN_OP_T(func, op, type) \
template<typename T1, typename T2> \
auto func(const Term<T1, type>& t1, const Term<T2, type>& t2) \
{ \
    return Term<types::Merge<T1, T2>, type>(BinaryOperator::Op::op, t1, t2); \
} \
template<typename T1, typename T2> \
auto func(Term<T1, type>&& t1, const Term<T2, type>& t2) \
{ \
    return Term<types::Merge<T1, T2>, type>(BinaryOperator::Op::op, std::move(t1), t2); \
} \
template<typename T1, typename T2> \
auto func(const Term<T1, type>& t1, Term<T2, type>&& t2) \
{ \
    return Term<types::Merge<T1, T2>, type>(BinaryOperator::Op::op, t1, std::move(t2)); \
} \
template<typename T1, typename T2> \
auto func(Term<T1, type>&& t1, Term<T2, type>&& t2) \
{ \
    return Term<types::Merge<T1, T2>, type>(BinaryOperator::Op::op, std::move(t1), std::move(t2)); \
} \
template<typename T> \
auto func(const Term<T, type>& t, Literal<type> v) \
{ \
    return Term<T, type>(BinaryOperator::Op::op, t, std::move(v)); \
} \
template<typename T> \
auto func(Term<T, type>&& t, Literal<type> v) \
{ \
    return Term<T, type>(BinaryOperator::Op::op, std::move(t), std::move(v)); \
} \
template<typename T> \
auto func(Literal<type> v, const Term<T, type>& t) \
{ \
    return Term<T, type>(BinaryOperator::Op::op, std::move(v), t); \
} \
template<typename T> \
auto func(Literal<type> v, Term<T, type>&& t) \
{ \
    return Term<T, type>(BinaryOperator::Op::op, std::move(v), std::move(t)); \
}

#define BIN_OP(func, op) \
    BIN_OP_T(func, op, Integer) \
    BIN_OP_T(func, op, Real)

BIN_OP(operator*, MUL)
BIN_OP(operator/, DIV)
BIN_OP(operator%, MOD)
BIN_OP(operator+, PLUS)
BIN_OP(operator-, MINUS)
BIN_OP(operator<<, SHL)
BIN_OP(operator>>, SHR)
BIN_OP(operator&, BAND)
BIN_OP(operator|, BOR)

#undef BIN_OP

#undef BIN_OP_T

} /* namespace expr */

} /* namespace sqlpp */

#endif /* SRC_SQLPP_EXPR_TERM_H_ */
