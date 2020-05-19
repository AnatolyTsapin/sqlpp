#ifndef SRC_SQLPP_STMT_UPDATE_H_
#define SRC_SQLPP_STMT_UPDATE_H_

#include <sqlpp/stmt/common.h>
#include <sqlpp/expr/condition.h>

namespace sqlpp
{
namespace stmt
{

class UpdateData
{
public:
    explicit UpdateData(const std::string& tableName);

    UpdateData(const UpdateData&);
    UpdateData(UpdateData&&);

    UpdateData& operator=(const UpdateData&);
    UpdateData& operator=(UpdateData&&);

    void addAssignment(const std::string& column, const expr::Data& data);
    void addAssignment(const std::string& column, expr::Data&& data);

    void addCondition(const expr::Data& cond);
    void addCondition(expr::Data&& cond);

    void dump(std::ostream& stream) const;
    Result execute(const Database& db) const;

private:
    std::string tableName;
    std::vector<std::tuple<std::string, expr::Node::Ptr>> assignemts;
    std::vector<Bind> binds;
    expr::Node::Ptr root;
};

template<typename T, typename C>
class UpdateWhere;

template<typename T>
class Update final : public StatementD<UpdateData>
{
private:
    using StatementD::StatementD;

    template<typename... A>
    explicit Update(const std::string& tableName, A&&... assignments) :
        StatementD(tableName)
    {
        addAssignments<types::IntList<>>(std::forward<A>(assignments)...);
    }

public:
    template<typename A, typename... AA>
    static Update<T> make(A&& a, AA&&... aa)
    {
        return Update<T>(a.getColumn().getTable().getName(), std::forward<A>(a), std::forward<AA>(aa)...);
    }

    ~Update() override = default;

    template<typename C>
    UpdateWhere<T, C> where(const expr::Condition<C>& condition) const
    {
        return UpdateWhere<T, C>(data, condition);
    }

private:
    template<typename I, typename A, typename... AA>
    void addAssignments(A&& a, AA&&... aa)
    {
        constexpr size_t INDEX = remove_cvref_t<A>::INDEX;
        static_assert(!I::contains(INDEX), "Cannot update the same value twice");
        data.addAssignment(a.getColumn().getName(), std::forward<A>(a).getExpr().data);
        if constexpr(types::PackSize<AA...>)
            addAssignments<types::AddIntList<INDEX, I>>(std::forward<AA>(aa)...);
    }
};

template<typename T, typename C>
class UpdateWhere final : public StatementD<UpdateData>
{
    friend class Update<T>;

private:
    using StatementD::StatementD;

    UpdateWhere(const UpdateData& data, const expr::Condition<C>& condition) :
        StatementD(data)
    {
        this->data.addCondition(condition.data);
    }

public:
    ~UpdateWhere() override = default;
};

} /* namespace stmt */

template<typename A, typename... AA>
stmt::Update<typename A::Table> update(A&& a, AA&&... aa)
{
    return stmt::Update<typename A::Table>::make(std::forward<A>(a), std::forward<AA>(aa)...);
}

} /* namespace sqlpp */

#endif /* SRC_SQLPP_STMT_UPDATE_H_ */
