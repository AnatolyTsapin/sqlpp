#ifndef SRC_SQLPP_EXPR_CONDITION_H_
#define SRC_SQLPP_EXPR_CONDITION_H_

#include <sqlpp/expr/term.h>

namespace sqlpp
{

namespace expr
{

template<typename T>
class Condition
{
    template<typename A>
    friend class Condition;

    template<typename B, typename C, typename D>
    friend class stmt::SelectWhere;

public:
    template<typename T1, typename T2, typename V>
    Condition(BinaryOperator::Op op, const Term<T1, V>& left, const Term<T2, V>& right) :
        data(op, left.data, right.data)
    {}
    template<typename T1, typename T2, typename V>
    Condition(BinaryOperator::Op op, Term<T1, V>&& left, const Term<T2, V>& right) :
        data(op, std::move(left.data), right.data)
    {}
    template<typename T1, typename T2, typename V>
    Condition(BinaryOperator::Op op, const Term<T1, V>& left, Term<T2, V>&& right) :
        data(op, left.data, std::move(right.data))
    {}
    template<typename T1, typename T2, typename V>
    Condition(BinaryOperator::Op op, Term<T1, V>&& left, Term<T2, V>&& right) :
        data(op, std::move(left.data), std::move(right.data))
    {}

    template<typename T1, typename T2>
    Condition(BinaryOperator::Op op, const Condition<T1>& left, const Condition<T2>& right) :
        data(op, left.data, right.data)
    {}
    template<typename T1, typename T2>
    Condition(BinaryOperator::Op op, Condition<T1>&& left, const Condition<T2>& right) :
        data(op, std::move(left.data), right.data)
    {}
    template<typename T1, typename T2>
    Condition(BinaryOperator::Op op, const Condition<T1>& left, Condition<T2>&& right) :
        data(op, left.data, std::move(right.data))
    {}
    template<typename T1, typename T2>
    Condition(BinaryOperator::Op op, Condition<T1>&& left, Condition<T2>&& right) :
        data(op, std::move(left.data), std::move(right.data))
    {}

    Condition(const Condition&) = default;
    Condition(Condition&&) = default;

    Condition& operator=(const Condition&) = default;
    Condition& operator=(Condition&&) = default;

    void dump(std::ostream& stream) const
    {
        data.dump(stream);
    }

private:
    Data data;
};

#define BIN_OP_T(func, op, type) \
template<typename T1, typename T2> \
auto func(const Term<T1, type>& t1, const Term<T2, type>& t2) \
{ \
    return Condition<types::Merge<T1, T2>>(BinaryOperator::Op::op, t1, t2); \
} \
template<typename T1, typename T2> \
auto func(Term<T1, type>&& t1, const Term<T2, type>& t2) \
{ \
    return Condition<types::Merge<T1, T2>>(BinaryOperator::Op::op, std::move(t1), t2); \
} \
template<typename T1, typename T2> \
auto func(const Term<T1, type>& t1, Term<T2, type>&& t2) \
{ \
    return Condition<types::Merge<T1, T2>>(BinaryOperator::Op::op, t1, std::move(t2)); \
} \
template<typename T1, typename T2> \
auto func(Term<T1, type>&& t1, Term<T2, type>&& t2) \
{ \
    return Condition<types::Merge<T1, T2>>(BinaryOperator::Op::op, std::move(t1), std::move(t2)); \
} \
template<typename T> \
auto func(const Term<T, type>& t, Literal<type> v) \
{ \
    return Condition<T>(BinaryOperator::Op::op, t, std::move(v)); \
} \
template<typename T> \
auto func(Term<T, type>&& t, Literal<type> v) \
{ \
    return Condition<T>(BinaryOperator::Op::op, std::move(t), std::move(v)); \
} \
template<typename T> \
auto func(Literal<type> v, const Term<T, type>& t) \
{ \
    return Condition<T>(BinaryOperator::Op::op, std::move(v), t); \
} \
template<typename T> \
auto func(Literal<type> v, Term<T, type>&& t) \
{ \
    return Condition<T>(BinaryOperator::Op::op, std::move(v), std::move(t)); \
}

#define BIN_OP(func, op) \
    BIN_OP_T(func, op, Integer) \
    BIN_OP_T(func, op, Real) \
    BIN_OP_T(func, op, Text) \
    BIN_OP_T(func, op, Blob)

BIN_OP(operator<, LS)
BIN_OP(operator<=, LE)
BIN_OP(operator>, GT)
BIN_OP(operator>=, GE)
BIN_OP(operator==, EQ)
BIN_OP(operator!=, NE)

#undef BIN_OP

#undef BIN_OP_T

#define BIN_OP(func, op) \
template<typename T1, typename T2> \
auto func(const Condition<T1>& c1, const Condition<T2>& c2) \
{ \
    return Condition<types::Merge<T1, T2>>(BinaryOperator::Op::op, c1, c2); \
} \
template<typename T1, typename T2> \
auto func(Condition<T1>&& c1, const Condition<T2>& c2) \
{ \
    return Condition<types::Merge<T1, T2>>(BinaryOperator::Op::op, std::move(c1), c2); \
} \
template<typename T1, typename T2> \
auto func(const Condition<T1>& c1, Condition<T2>&& c2) \
{ \
    return Condition<types::Merge<T1, T2>>(BinaryOperator::Op::op, c1, std::move(c2)); \
} \
template<typename T1, typename T2> \
auto func(Condition<T1>&& c1, Condition<T2>&& c2) \
{ \
    return Condition<types::Merge<T1, T2>>(BinaryOperator::Op::op, std::move(c1), std::move(c2)); \
}

BIN_OP(operator&&, AND)
BIN_OP(operator||, OR)

#undef BIN_OP

} /* namespace expr */

} /* namespace sqlpp */

#endif /* SRC_SQLPP_EXPR_CONDITION_H_ */
