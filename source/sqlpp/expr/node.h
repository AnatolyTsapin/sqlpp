#ifndef SRC_SQLPP_EXPR_NODE_H_
#define SRC_SQLPP_EXPR_NODE_H_

#include <sqlpp/types.h>

#include <iostream>
#include <memory>
#include <unordered_set>

namespace sqlpp {

namespace stmt {

class SelectData;

class UpdateData;

}  // namespace stmt

namespace expr {

class Node {
 public:
  using Ptr = std::unique_ptr<Node>;

  template <typename T, typename... V>
  static Ptr make(V&&... v) {
    return Ptr(new T(std::forward<V>(v)...));
  }

  virtual ~Node();

  virtual int getPrecedence() const = 0;

  virtual Ptr clone() const = 0;
  virtual void dump(std::ostream& stream, bool parenthesis = false) const = 0;
};

template <typename T>
class NodeT : public Node {
 public:
  ~NodeT() override = default;

  Ptr clone() const override {
    return Ptr(new T(static_cast<const T&>(*this)));
  }
};

class Leaf : public NodeT<Leaf> {
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

class UnaryOperator : public NodeT<UnaryOperator> {
 public:
  enum class Op {
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

class BinaryOperator : public NodeT<BinaryOperator> {
 public:
  enum class Op {
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

class Data {
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

  operator bool() const { return !!root; }

  void dump(std::ostream& stream) const;

 private:
  Node::Ptr root;
  std::unordered_set<std::string> tables;
  std::vector<Bind> binds;

  friend class stmt::SelectData;
  friend class stmt::UpdateData;
};

} /* namespace expr */

} /* namespace sqlpp */

#endif /* SRC_SQLPP_EXPR_NODE_H_ */
