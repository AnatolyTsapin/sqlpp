#ifndef SRC_SQLPP_CONDITION_H_
#define SRC_SQLPP_CONDITION_H_

#include <sqlpp/types.h>

#include <iostream>
#include <memory>
#include <unordered_set>

namespace sqlpp
{

namespace stmt
{

class SelectData;

template<typename B, typename C>
class SelectWhere;

}

namespace condition
{

class Node
{
public:
    using Ptr = std::unique_ptr<Node>;

    template<typename T, typename ... V>
    static Ptr make(V&& ... v)
    {
        return Ptr(new T(std::forward<V>(v)...));
    }

    virtual ~Node();

    virtual int getPrecedence() const = 0;

    virtual Ptr clone() const = 0;
    virtual void dump(std::ostream& stream, bool parenthesis = false) const = 0;
};

template<typename T>
class NodeT: public Node
{
public:
    ~NodeT() override = default;

    Ptr clone() const override
    {
        return Ptr(new T(static_cast<const T&>(*this)));
    }
};

class Leaf: public NodeT<Leaf>
{
public:
    Leaf();
    Leaf(const std::string& table, const std::string& field);

    Leaf(const Leaf& other);

    ~Leaf() override;

    int getPrecedence() const override;

    void dump(std::ostream& stream, bool parenthesis) const override;

private:
    std::string value;
};

class UnaryOperator: public NodeT<UnaryOperator>
{
public:
    enum class Op
    {
        MINUS,
        PLUS,
        COMPLEMENT,
        NOT,
    };

    UnaryOperator(Op op, const Node::Ptr& child);
    UnaryOperator(Op op, Node::Ptr&& child);

    UnaryOperator(const UnaryOperator& other);

    ~UnaryOperator() override;

    int getPrecedence() const override;

    void dump(std::ostream& stream, bool parenthesis) const override;

private:
    const Op op;
    Node::Ptr child;
};

class BinaryOperator: public NodeT<BinaryOperator>
{
public:
    enum class Op
    {
        MUL,
        DIV,
        MOD,
        PLUS,
        MINUS,
        SHL,
        SHR,
        BAND,
        BOR,
        LS,
        LE,
        GT,
        GE,
        EQ,
        NE,
        AND,
        OR,
    };

    BinaryOperator(Op op, const Node::Ptr& left, const Node::Ptr& right);
    BinaryOperator(Op op, Node::Ptr&& left, Node::Ptr&& right);

    BinaryOperator(const BinaryOperator& other);

    ~BinaryOperator() override;

    int getPrecedence() const override;

    void dump(std::ostream& stream, bool parenthesis) const override;

private:
    const Op op;
    Node::Ptr left;
    Node::Ptr right;
};

class Data
{
public:
    Data();
    Data(const Data& other);
    Data(Data&& other);

    Data(const std::string& table, const std::string& field);
    explicit Data(Bind&& bind);

    Data(UnaryOperator::Op op, const Data& child);
    Data(UnaryOperator::Op op, Data&& child);

    Data(BinaryOperator::Op op, const Data& left, const Data& right);
    Data(BinaryOperator::Op op, Data&& left, const Data& right);
    Data(BinaryOperator::Op op, const Data& left, Data&& right);
    Data(BinaryOperator::Op op, Data&& left, Data&& right);

    Data& operator=(const Data& other);
    Data& operator=(Data&& other);

    operator bool() const
    {
        return !!root;
    }

    void dump(std::ostream& stream) const;

private:
    Node::Ptr root;
    std::unordered_set<std::string> tables;
    std::vector<Bind> binds;

    friend class stmt::SelectData;
};

} /* namespace condition */

template<typename T, typename V>
class Term
{
    template<typename A, typename B>
    friend class Term;

public:
    Term(const std::string& table, const std::string& field) :
        data(table, field)
    {}
    explicit Term(Bind&& bind) :
        data(std::move(bind))
    {}

    Term(condition::UnaryOperator::Op op, const Term<T, V>& t) :
        data(op, t.data)
    {}
    Term(condition::UnaryOperator::Op op, Term<T, V>&& t) :
        data(op, std::move(t.data))
    {}

    template<typename T1, typename T2>
    Term(condition::BinaryOperator::Op op, const Term<T1, V>& t1, const Term<T2, V>& t2) :
        data(op, t1.data, t2.data)
    {}
    template<typename T1, typename T2>
    Term(condition::BinaryOperator::Op op, Term<T1, V>&& t1, const Term<T2, V>& t2) :
        data(op, std::move(t1.data), t2.data)
    {}
    template<typename T1, typename T2>
    Term(condition::BinaryOperator::Op op, const Term<T1, V>& t1, Term<T2, V>&& t2) :
        data(op, t1.data, std::move(t2.data))
    {}
    template<typename T1, typename T2>
    Term(condition::BinaryOperator::Op op, Term<T1, V>&& t1, Term<T2, V>&& t2) :
        data(op, std::move(t1.data), std::move(t2.data))
    {}

private:
    condition::Data data;

    template<typename A>
    friend class Condition;
};

template<typename T>
class Condition
{
    template<typename A>
    friend class Condition;

    template<typename B, typename C>
    friend class stmt::SelectWhere;

public:
    template<typename T1, typename T2, typename V>
    Condition(condition::BinaryOperator::Op op, const Term<T1, V>& left, const Term<T2, V>& right) :
        data(op, left.data, right.data)
    {}
    template<typename T1, typename T2, typename V>
    Condition(condition::BinaryOperator::Op op, Term<T1, V>&& left, const Term<T2, V>& right) :
        data(op, std::move(left.data), right.data)
    {}
    template<typename T1, typename T2, typename V>
    Condition(condition::BinaryOperator::Op op, const Term<T1, V>& left, Term<T2, V>&& right) :
        data(op, left.data, std::move(right.data))
    {}
    template<typename T1, typename T2, typename V>
    Condition(condition::BinaryOperator::Op op, Term<T1, V>&& left, Term<T2, V>&& right) :
        data(op, std::move(left.data), std::move(right.data))
    {}

    template<typename T1, typename T2>
    Condition(condition::BinaryOperator::Op op, const Condition<T1>& left, const Condition<T2>& right) :
        data(op, left.data, right.data)
    {}
    template<typename T1, typename T2>
    Condition(condition::BinaryOperator::Op op, Condition<T1>&& left, const Condition<T2>& right) :
        data(op, std::move(left.data), right.data)
    {}
    template<typename T1, typename T2>
    Condition(condition::BinaryOperator::Op op, const Condition<T1>& left, Condition<T2>&& right) :
        data(op, left.data, std::move(right.data))
    {}
    template<typename T1, typename T2>
    Condition(condition::BinaryOperator::Op op, Condition<T1>&& left, Condition<T2>&& right) :
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
    condition::Data data;
};

#define UN_OP(func, op) \
template<typename T, typename V> \
auto func(const Term<T, V>& t) \
{ \
    return Term<T, V>(condition::UnaryOperator::Op::op, t); \
} \
template<typename T, typename V> \
auto func(Term<T, V>&& t) \
{ \
    return Term<T, V>(condition::UnaryOperator::Op::op, std::move(t)); \
}

UN_OP(operator-, MINUS)
UN_OP(operator+, PLUS)
UN_OP(operator~, COMPLEMENT)
UN_OP(operator!, NOT)

#undef UN_OP

#define BIN_OP(func, op) \
template<typename T1, typename T2, typename V> \
auto func(const Term<T1, V>& t1, const Term<T2, V>& t2) \
{ \
    return Term<types::Merge<T1, T2>, V>(condition::BinaryOperator::Op::op, t1, t2); \
} \
template<typename T1, typename T2, typename V> \
auto func(Term<T1, V>&& t1, const Term<T2, V>& t2) \
{ \
    return Term<types::Merge<T1, T2>, V>(condition::BinaryOperator::Op::op, std::move(t1), t2); \
} \
template<typename T1, typename T2, typename V> \
auto func(const Term<T1, V>& t1, Term<T2, V>&& t2) \
{ \
    return Term<types::Merge<T1, T2>, V>(condition::BinaryOperator::Op::op, t1, std::move(t2)); \
} \
template<typename T1, typename T2, typename V> \
auto func(Term<T1, V>&& t1, Term<T2, V>&& t2) \
{ \
    return Term<types::Merge<T1, T2>, V>(condition::BinaryOperator::Op::op, std::move(t1), std::move(t2)); \
} \
template<typename T, typename V> \
auto func(const Term<T, V>& t, V v) \
{ \
    return Term<T, V>(condition::BinaryOperator::Op::op, t, Term<types::MakeList<>, V>(createBind(std::move(v)))); \
} \
template<typename T, typename V> \
auto func(Term<T, V>&& t, V v) \
{ \
    return Term<T, V>(condition::BinaryOperator::Op::op, std::move(t), Term<types::MakeList<>, V>(createBind(std::move(v)))); \
} \
template<typename T, typename V> \
auto func(V v, const Term<T, V>& t) \
{ \
    return Term<T, V>(condition::BinaryOperator::Op::op, Term<types::MakeList<>, V>(createBind(std::move(v))), t); \
} \
template<typename T, typename V> \
auto func(V v, Term<T, V>&& t) \
{ \
    return Term<T, V>(condition::BinaryOperator::Op::op, Term<types::MakeList<>, V>(createBind(std::move(v))), std::move(t)); \
}

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

#define BIN_OP(func, op) \
template<typename T1, typename T2, typename V> \
auto func(const Term<T1, V>& t1, const Term<T2, V>& t2) \
{ \
    return Condition<types::Merge<T1, T2>>(condition::BinaryOperator::Op::op, t1, t2); \
} \
template<typename T1, typename T2, typename V> \
auto func(Term<T1, V>&& t1, const Term<T2, V>& t2) \
{ \
    return Condition<types::Merge<T1, T2>>(condition::BinaryOperator::Op::op, std::move(t1), t2); \
} \
template<typename T1, typename T2, typename V> \
auto func(const Term<T1, V>& t1, Term<T2, V>&& t2) \
{ \
    return Condition<types::Merge<T1, T2>>(condition::BinaryOperator::Op::op, t1, std::move(t2)); \
} \
template<typename T1, typename T2, typename V> \
auto func(Term<T1, V>&& t1, Term<T2, V>&& t2) \
{ \
    return Condition<types::Merge<T1, T2>>(condition::BinaryOperator::Op::op, std::move(t1), std::move(t2)); \
} \
template<typename T, typename V> \
auto func(const Term<T, V>& t, V v) \
{ \
    return Condition<T>(condition::BinaryOperator::Op::op, t, Term<types::MakeList<>, V>(createBind(std::move(v)))); \
} \
template<typename T, typename V> \
auto func(Term<T, V>&& t, V v) \
{ \
    return Condition<T>(condition::BinaryOperator::Op::op, std::move(t), Term<types::MakeList<>, V>(createBind(std::move(v)))); \
} \
template<typename T, typename V> \
auto func(V v, const Term<T, V>& t) \
{ \
    return Condition<T>(condition::BinaryOperator::Op::op, Term<types::MakeList<>, V>(createBind(std::move(v))), t); \
} \
template<typename T, typename V> \
auto func(V v, Term<T, V>&& t) \
{ \
    return Condition<T>(condition::BinaryOperator::Op::op, Term<types::MakeList<>, V>(createBind(std::move(v))), std::move(t)); \
}

BIN_OP(operator<, LS)
BIN_OP(operator<=, LE)
BIN_OP(operator>, GT)
BIN_OP(operator>=, GE)
BIN_OP(operator==, EQ)
BIN_OP(operator!=, NE)

#undef BIN_OP

#define BIN_OP(func, op) \
template<typename T1, typename T2> \
auto func(const Condition<T1>& c1, const Condition<T2>& c2) \
{ \
    return Condition<types::Merge<T1, T2>>(condition::BinaryOperator::Op::op, c1, c2); \
} \
template<typename T1, typename T2> \
auto func(Condition<T1>&& c1, const Condition<T2>& c2) \
{ \
    return Condition<types::Merge<T1, T2>>(condition::BinaryOperator::Op::op, std::move(c1), c2); \
} \
template<typename T1, typename T2> \
auto func(const Condition<T1>& c1, Condition<T2>&& c2) \
{ \
    return Condition<types::Merge<T1, T2>>(condition::BinaryOperator::Op::op, c1, std::move(c2)); \
} \
template<typename T1, typename T2> \
auto func(Condition<T1>&& c1, Condition<T2>&& c2) \
{ \
    return Condition<types::Merge<T1, T2>>(condition::BinaryOperator::Op::op, std::move(c1), std::move(c2)); \
}

BIN_OP(operator&&, AND)
BIN_OP(operator||, OR)

#undef BIN_OP

} /* namespace sqlpp */

#endif /* SRC_SQLPP_CONDITION_H_ */
