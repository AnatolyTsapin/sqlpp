#include <sqlpp/condition.h>

using namespace std;

namespace sqlpp
{

namespace condition
{

static const std::string OpStr[] = {
    "=",
    "!=",
    "AND",
};

Node::~Node() = default;

Leaf::Leaf(const string& table, const string& field) :
    value(make_pair(table, field))
{}

Leaf::Leaf(Bind&& bind) :
    value(move(bind))
{}

Leaf::Leaf(const Leaf& other) = default;

Leaf::~Leaf() = default;

void Leaf::dump(std::ostream& stream) const
{
    switch(value.index())
    {
    case 0:
    {
        auto&& v = get<pair<string, string>>(value);
        stream << v.first << "." << v.second;
        break;
    }

    case 1:
    {
        stream << "value";
        break;
    }

    default:
        std::invalid_argument("Unsupported type of value");
    }
}

Operand::Operand(Op op, const Node::Ptr& left, const Node::Ptr& right) :
    op(op), left(left->clone()), right(right->clone())
{}

Operand::Operand(Op op, Node::Ptr&& left, Node::Ptr&& right) :
    op(op), left(move(left)), right(move(right))
{}

Operand::Operand(const Operand& other) :
    op(other.op), left(other.left->clone()), right(other.right->clone())
{}

Operand::~Operand() = default;

void Operand::dump(std::ostream& stream) const
{
    stream << "(";
    left->dump(stream);
    stream << ") " << OpStr[static_cast<int>(op)] << " (";
    right->dump(stream);
    stream << ")";
}

Data::Data() = default;

Data::Data(const Data& other) :
    root(other.root->clone())
{}

Data::Data(Data&& other) = default;

Data::Data(Op op, const Data& left, const Data& right) :
    root(new Operand(op, left.root, right.root))
{}

Data::Data(Op op, Data&& left, Data&& right) :
    root(new Operand(op, move(left.root), move(right.root)))
{}

Data::Data(Op op, const Node::Ptr& left, const Node::Ptr& right) :
    root(new Operand(op, left, right))
{}

Data::Data(Op op, Node::Ptr&& left, Node::Ptr&& right) :
    root(new Operand(op, move(left), move(right)))
{}

Data& Data::operator=(const Data& other)
{
    if(this != &other)
        root = other.root->clone();
    return *this;
}

Data& Data::operator=(Data&& other) = default;

void Data::dump(std::ostream& stream) const
{
    if(root)
        root->dump(stream);
}

} /* namespace condition */

} /* namespace sqlpp */
