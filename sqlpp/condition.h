#ifndef SRC_SQLPP_CONDITION_H_
#define SRC_SQLPP_CONDITION_H_

#include <sqlpp/column.h>

#include <iostream>
#include <variant>
#include <memory>

namespace sqlpp
{

namespace stmt
{

template<typename B, typename C>
class SelectWhere;

}

namespace condition
{

enum class Op : int
{
    EQ,
    NE,
	LS,
	LE,
	GT,
	GE,
    AND,
	OR,
};

class Node
{
public:
    using Ptr = std::unique_ptr<Node>;

    template<typename T, typename... V>
    static Ptr make(V&&... v)
    {
        return Ptr(new T(std::forward<V>(v)...));
    }

    virtual ~Node();

    virtual Ptr clone() const = 0;
    virtual void dump(std::ostream& stream) const = 0;
};

template<typename T>
class NodeT : public Node
{
public:
    ~NodeT() override = default;

    Ptr clone() const override
    {
        return Ptr(new T(static_cast<const T&>(*this)));
    }
};

class Leaf : public NodeT<Leaf>
{
public:
    Leaf(const std::string& table, const std::string& field);
    explicit Leaf(Bind&& bind);

    Leaf(const Leaf& other);

    ~Leaf() override;

    void dump(std::ostream& stream) const override;

private:
    std::variant<std::pair<std::string, std::string>, Bind> value;
};

class Operand : public NodeT<Operand>
{
public:
    Operand(Op op, const Node::Ptr& left, const Node::Ptr& right);
    Operand(Op op, Node::Ptr&& left, Node::Ptr&& right);

    Operand(const Operand& other);

    ~Operand() override;

    void dump(std::ostream& stream) const override;

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

    Data(Op op, const Data& left, const Data& right);
    Data(Op op, Data&& left, Data&& right);

    Data(Op op, const Node::Ptr& left, const Node::Ptr& right);
    Data(Op op, Node::Ptr&& left, Node::Ptr&& right);

    Data& operator=(const Data& other);
    Data& operator=(Data&& other);

    operator bool() const
    {
        return !!root;
    }

    void dump(std::ostream& stream) const;

private:
    Node::Ptr root;
};

} /* namespace condition */

template<typename T>
class Condition
{
    template<typename A>
    friend class Condition;

    template<typename B, typename C>
    friend class stmt::SelectWhere;

public:
    template<typename L, typename R>
    Condition(condition::Op op, L&& left, R&& right) :
        data(op, std::forward<L>(left), std::forward<R>(right))
    {}

    template<typename T1, typename T2>
    Condition(condition::Op op, const Condition<T1>& left, const Condition<T2>& right) :
        data(op, left.data, right.data)
    {}

    template<typename T1, typename T2>
    Condition(condition::Op op, Condition<T1>&& left, Condition<T2>&& right) :
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

template<typename T1, typename V1, typename T2, typename V2>
auto operator==(const Column<T1, V1>& c1, const Column<T2, V2>& c2)
{
    static_assert(std::is_same_v<DbType<V1>, DbType<V2>>, "Different DB types cannot be compared");
    return Condition<types::MakeList<T1, T2>>(condition::Op::EQ,
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()),
        condition::Node::make<condition::Leaf>(c2.getTable().getName(), c2.getName()));
}

template<typename T, typename V>
auto operator==(const Column<T, V>& c1, V&& v2)
{
    return Condition<types::MakeList<T>>(condition::Op::EQ,
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()),
        condition::Node::make<condition::Leaf>(createBind(std::forward<V>(v2))));
}

template<typename T, typename V>
auto operator==(V&& v2, const Column<T, V>& c1)
{
    return Condition<types::MakeList<T>>(condition::Op::EQ,
        condition::Node::make<condition::Leaf>(createBind(std::forward<V>(v2))),
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()));
}

template<typename T1, typename V1, typename T2, typename V2>
auto operator!=(const Column<T1, V1>& c1, const Column<T2, V2>& c2)
{
    static_assert(std::is_same_v<DbType<V1>, DbType<V2>>, "Different DB types cannot be compared");
    return Condition<types::MakeList<T1, T2>>(condition::Op::NE,
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()),
        condition::Node::make<condition::Leaf>(c2.getTable().getName(), c2.getName()));
}

template<typename T, typename V>
auto operator!=(const Column<T, V>& c1, V&& v2)
{
    return Condition<types::MakeList<T>>(condition::Op::NE,
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()),
        condition::Node::make<condition::Leaf>(createBind(std::forward<V>(v2))));
}

template<typename T, typename V>
auto operator!=(V&& v2, const Column<T, V>& c1)
{
    return Condition<types::MakeList<T>>(condition::Op::NE,
        condition::Node::make<condition::Leaf>(createBind(std::forward<V>(v2))),
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()));
}

template<typename T1, typename V1, typename T2, typename V2>
auto operator<(const Column<T1, V1>& c1, const Column<T2, V2>& c2)
{
    static_assert(std::is_same_v<DbType<V1>, DbType<V2>>, "Different DB types cannot be compared");
    return Condition<types::MakeList<T1, T2>>(condition::Op::LS,
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()),
        condition::Node::make<condition::Leaf>(c2.getTable().getName(), c2.getName()));
}

template<typename T, typename V>
auto operator<(const Column<T, V>& c1, V&& v2)
{
    return Condition<types::MakeList<T>>(condition::Op::LS,
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()),
        condition::Node::make<condition::Leaf>(createBind(std::forward<V>(v2))));
}

template<typename T, typename V>
auto operator<(V&& v2, const Column<T, V>& c1)
{
    return Condition<types::MakeList<T>>(condition::Op::LS,
        condition::Node::make<condition::Leaf>(createBind(std::forward<V>(v2))),
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()));
}

template<typename T1, typename V1, typename T2, typename V2>
auto operator<=(const Column<T1, V1>& c1, const Column<T2, V2>& c2)
{
    static_assert(std::is_same_v<DbType<V1>, DbType<V2>>, "Different DB types cannot be compared");
    return Condition<types::MakeList<T1, T2>>(condition::Op::LE,
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()),
        condition::Node::make<condition::Leaf>(c2.getTable().getName(), c2.getName()));
}

template<typename T, typename V>
auto operator<=(const Column<T, V>& c1, V&& v2)
{
    return Condition<types::MakeList<T>>(condition::Op::LE,
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()),
        condition::Node::make<condition::Leaf>(createBind(std::forward<V>(v2))));
}

template<typename T, typename V>
auto operator<=(V&& v2, const Column<T, V>& c1)
{
    return Condition<types::MakeList<T>>(condition::Op::LE,
        condition::Node::make<condition::Leaf>(createBind(std::forward<V>(v2))),
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()));
}

template<typename T1, typename V1, typename T2, typename V2>
auto operator>(const Column<T1, V1>& c1, const Column<T2, V2>& c2)
{
    static_assert(std::is_same_v<DbType<V1>, DbType<V2>>, "Different DB types cannot be compared");
    return Condition<types::MakeList<T1, T2>>(condition::Op::GT,
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()),
        condition::Node::make<condition::Leaf>(c2.getTable().getName(), c2.getName()));
}

template<typename T, typename V>
auto operator>(const Column<T, V>& c1, V&& v2)
{
    return Condition<types::MakeList<T>>(condition::Op::GT,
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()),
        condition::Node::make<condition::Leaf>(createBind(std::forward<V>(v2))));
}

template<typename T, typename V>
auto operator>(V&& v2, const Column<T, V>& c1)
{
    return Condition<types::MakeList<T>>(condition::Op::GT,
        condition::Node::make<condition::Leaf>(createBind(std::forward<V>(v2))),
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()));
}

template<typename T1, typename V1, typename T2, typename V2>
auto operator>=(const Column<T1, V1>& c1, const Column<T2, V2>& c2)
{
    static_assert(std::is_same_v<DbType<V1>, DbType<V2>>, "Different DB types cannot be compared");
    return Condition<types::MakeList<T1, T2>>(condition::Op::GE,
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()),
        condition::Node::make<condition::Leaf>(c2.getTable().getName(), c2.getName()));
}

template<typename T, typename V>
auto operator>=(const Column<T, V>& c1, V&& v2)
{
    return Condition<types::MakeList<T>>(condition::Op::GE,
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()),
        condition::Node::make<condition::Leaf>(createBind(std::forward<V>(v2))));
}

template<typename T, typename V>
auto operator>=(V&& v2, const Column<T, V>& c1)
{
    return Condition<types::MakeList<T>>(condition::Op::GE,
        condition::Node::make<condition::Leaf>(createBind(std::forward<V>(v2))),
        condition::Node::make<condition::Leaf>(c1.getTable().getName(), c1.getName()));
}

template<typename T1, typename T2>
auto operator&&(const Condition<T1>& c1, const Condition<T2>& c2)
{
    return Condition<types::Merge<T1, T2>>(condition::Op::AND, c1, c2);
}

template<typename T1, typename T2>
auto operator||(const Condition<T1>& c1, const Condition<T2>& c2)
{
    return Condition<types::Merge<T1, T2>>(condition::Op::OR, c1, c2);
}

} /* namespace sqlpp */

#endif /* SRC_SQLPP_CONDITION_H_ */
