#include <sqlpp/expr/node.h>

#include <stdexcept>
#include <vector>

using namespace std;

namespace sqlpp {

namespace expr {

enum class Precedence : int {
  OR,
  AND,
  CMP,
  ORD,
  BOR,
  BAND,
  SHIFT,
  ADD,
  MUL,
  UNARY,
  VALUE
};

Node::~Node() = default;

Leaf::Leaf() : value("?") {}

Leaf::Leaf(const string& table, const string& field)
    : value(table + "." + field) {}

Leaf::Leaf(const Leaf& other) = default;

Leaf::~Leaf() = default;

int Leaf::getPrecedence() const { return static_cast<int>(Precedence::VALUE); }

void Leaf::dump(ostream& stream, bool parenthesis) const {
  if (parenthesis) stream << "(";
  stream << value;
  if (parenthesis) stream << ")";
}

UnaryOperator::UnaryOperator(Op op, const Node::Ptr& child)
    : op(op), child(child->clone()) {}

UnaryOperator::UnaryOperator(Op op, Node::Ptr&& child)
    : op(op), child(move(child)) {}

UnaryOperator::UnaryOperator(const UnaryOperator& other)
    : op(other.op), child(other.child->clone()) {}

UnaryOperator::~UnaryOperator() = default;

int UnaryOperator::getPrecedence() const {
  return static_cast<int>(Precedence::UNARY);
}

void UnaryOperator::dump(ostream& stream, bool parenthesis) const {
  static const vector<string> OpStr = {"-", "+", "~", "NOT "};

  if (parenthesis) stream << "(";
  stream << OpStr[static_cast<int>(op)];
  child->dump(stream, getPrecedence() > child->getPrecedence());
  if (parenthesis) stream << ")";
}

BinaryOperator::BinaryOperator(Op op, const Node::Ptr& left,
                               const Node::Ptr& right)
    : op(op), left(left->clone()), right(right->clone()) {}

BinaryOperator::BinaryOperator(Op op, Node::Ptr&& left, Node::Ptr&& right)
    : op(op), left(move(left)), right(move(right)) {}

BinaryOperator::BinaryOperator(const BinaryOperator& other)
    : op(other.op), left(other.left->clone()), right(other.right->clone()) {}

BinaryOperator::~BinaryOperator() = default;

int BinaryOperator::getPrecedence() const {
  switch (op) {
    case Op::MUL:
    case Op::DIV:
    case Op::MOD:
      return static_cast<int>(Precedence::MUL);

    case Op::PLUS:
    case Op::MINUS:
      return static_cast<int>(Precedence::ADD);

    case Op::SHL:
    case Op::SHR:
      return static_cast<int>(Precedence::SHIFT);

    case Op::BAND:
      return static_cast<int>(Precedence::BAND);

    case Op::BOR:
      return static_cast<int>(Precedence::BOR);

    case Op::LS:
    case Op::LE:
    case Op::GT:
    case Op::GE:
      return static_cast<int>(Precedence::ORD);

    case Op::EQ:
    case Op::NE:
      return static_cast<int>(Precedence::CMP);

    case Op::AND:
      return static_cast<int>(Precedence::AND);

    case Op::OR:
      return static_cast<int>(Precedence::OR);

    default:
      throw invalid_argument("Unknown operation code");
  }
}

void BinaryOperator::dump(ostream& stream, bool parenthesis) const {
  static const vector<string> OpStr = {
      " * ", " / ",  " % ", " + ",  " - ", " << ", " >> ",  " & ", " | ",
      " < ", " <= ", " > ", " >= ", " = ", " <> ", " AND ", " OR "};

  if (parenthesis) stream << "(";
  left->dump(stream, getPrecedence() > left->getPrecedence());
  stream << OpStr[static_cast<int>(op)];
  right->dump(stream, getPrecedence() > right->getPrecedence());
  if (parenthesis) stream << ")";
}

Data::Data() = default;

Data::Data(const Data& other)
    : root(other.root->clone()), tables(other.tables), binds(other.binds) {}

Data::Data(Data&& other) = default;

Data::Data(const std::string& table, const std::string& field)
    : root(Node::make<Leaf>(table, field)), tables({table}) {}

Data::Data(Bind&& bind) : root(Node::make<Leaf>()), binds({std::move(bind)}) {}

Data::Data(UnaryOperator::Op op, const Data& child)
    : root(Node::make<UnaryOperator>(op, child.root->clone())),
      tables(child.tables),
      binds(child.binds) {}

Data::Data(UnaryOperator::Op op, Data&& child)
    : root(Node::make<UnaryOperator>(op, move(child.root))),
      tables(move(child.tables)),
      binds(move(child.binds)) {}

Data::Data(BinaryOperator::Op op, const Data& left, const Data& right)
    : root(Node::make<BinaryOperator>(op, left.root->clone(),
                                      right.root->clone())),
      tables(left.tables),
      binds(left.binds) {
  tables.insert(right.tables.begin(), right.tables.end());
  binds.insert(binds.end(), right.binds.begin(), right.binds.end());
}

Data::Data(BinaryOperator::Op op, Data&& left, const Data& right)
    : root(
          Node::make<BinaryOperator>(op, move(left.root), right.root->clone())),
      tables(move(left.tables)),
      binds(move(left.binds)) {
  tables.insert(right.tables.begin(), right.tables.end());
  binds.insert(binds.end(), right.binds.begin(), right.binds.end());
}

Data::Data(BinaryOperator::Op op, const Data& left, Data&& right)
    : root(
          Node::make<BinaryOperator>(op, left.root->clone(), move(right.root))),
      tables(move(right.tables)),
      binds(move(right.binds)) {
  tables.insert(left.tables.begin(), left.tables.end());
  binds.insert(binds.end(), left.binds.begin(), left.binds.end());
}

Data::Data(BinaryOperator::Op op, Data&& left, Data&& right)
    : root(Node::make<BinaryOperator>(op, move(left.root), move(right.root))),
      tables(move(left.tables)),
      binds(move(left.binds)) {
  tables.insert(right.tables.begin(), right.tables.end());
  binds.insert(binds.end(), right.binds.begin(), right.binds.end());
}

Data& Data::operator=(const Data& other) {
  if (this != &other) {
    root = other.root->clone();
    tables = other.tables;
    binds = other.binds;
  }
  return *this;
}

Data& Data::operator=(Data&& other) = default;

void Data::dump(ostream& stream) const {
  if (root) root->dump(stream);
}

} /* namespace expr */

} /* namespace sqlpp */
