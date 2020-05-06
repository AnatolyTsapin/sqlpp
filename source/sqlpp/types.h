#ifndef SQLPP_TYPES_H_
#define SQLPP_TYPES_H_

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <string>
#include <vector>
#include <functional>

struct sqlite3_stmt;

namespace sqlpp
{

template<typename V>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<V>>;

using Integer = int64_t;
using Real = double;
using Text = std::string;
using Blob = std::vector<std::byte>;

template<typename V>
struct TypeName
{
    static std::string get()
    {
        return "UNKNOWN";
    }
};

template<>
struct TypeName<Integer>
{
    static std::string get()
    {
        return "INTEGER";
    }
};

template<>
struct TypeName<Real>
{
    static std::string get()
    {
        return "REAL";
    }
};

template<>
struct TypeName<Text>
{
    static std::string get()
    {
        return "TEXT";
    }
};

template<>
struct TypeName<Blob>
{
    static std::string get()
    {
        return "BLOB";
    }
};

template<typename V, typename E = void>
struct Converter;

template<typename V>
using DbType = typename Converter<remove_cvref_t<V>>::DbType;

template<typename V>
auto toDb(const V& value)
{
    return Converter<remove_cvref_t<V>>::toDb(value);
}

template<typename V, typename U>
V fromDb(const U& value)
{
    return Converter<V>::fromDb(value);
}

template<typename V>
struct Converter<V, std::enable_if_t<
    std::is_same_v<remove_cvref_t<V>, Integer> ||
    std::is_same_v<remove_cvref_t<V>, Real> ||
    std::is_same_v<remove_cvref_t<V>, Text> ||
    std::is_same_v<remove_cvref_t<V>, Blob>>>
{
    using DbType = remove_cvref_t<V>;
    static DbType toDb(const DbType& value)
    {
        return value;
    }
    static DbType fromDb(const DbType& value)
    {
        return value;
    }
};

template<typename V>
struct Converter<V, std::enable_if_t<
    std::is_integral_v<remove_cvref_t<V>> && !std::is_same_v<remove_cvref_t<V>, Integer>>>
{
    using DbType = Integer;
    static Integer toDb(const V& value)
    {
        return static_cast<Integer>(value);
    }
    static V fromDb(const Integer& value)
    {
        return static_cast<V>(value);
    }
};

template<typename V>
struct Converter<V, std::enable_if_t<
    std::is_floating_point_v<remove_cvref_t<V>> && !std::is_same_v<remove_cvref_t<V>, Real>>>
{
    using DbType = Real;
    static Real toDb(const V& value)
    {
        return static_cast<Real>(value);
    }
    static V fromDb(const Real& value)
    {
        return static_cast<V>(value);
    }
};

using Bind = std::function<void(sqlite3_stmt*, int)>;

void bind(sqlite3_stmt* stmt, int idx, const Integer& value);
void bind(sqlite3_stmt* stmt, int idx, const Real& value);
void bind(sqlite3_stmt* stmt, int idx, const Text& value);
void bind(sqlite3_stmt* stmt, int idx, const Blob& value);

template<typename V>
auto createBind(V&& value)
{
    return [v = toDb(std::forward<V>(value))] (sqlite3_stmt* stmt, int idx) { bind(stmt, idx, v); };
}

template<typename T>
using TableType = typename T::TableType;

template<typename T>
using ValueType = typename T::ValueType;

namespace types
{

template<typename... T>
inline constexpr size_t PackSize = sizeof...(T);

template<typename... T>
struct List
{};

template<typename T, typename... TT>
struct List<T, TT...>
{
    using Head = T;
    using Tail = List<TT...>;
};

template<typename T>
struct HeadS
{
    using Type = T;
};

template<typename... T>
struct HeadS<List<T...>>
{
    using Type = typename List<T...>::Head;
};

template<typename T>
using Head = typename HeadS<T>::Type;

template<typename T>
struct TailS
{
    using Type = List<>;
};

template<typename... T>
struct TailS<List<T...>>
{
    using Type = typename List<T...>::Tail;
};

template<typename T>
using Tail = typename TailS<T>::Type;

template<size_t I, typename L>
struct GetS;

template<size_t I, typename L>
using Get = typename GetS<I, L>::Type;

template<size_t I, typename... T>
struct GetS<I, List<T...>>
{
    using Type = Get<I - 1, Tail<List<T...>>>;
};

template<typename... T>
struct GetS<0, List<T...>>
{
    using Type = Head<List<T...>>;
};

template<typename U, typename L>
struct ContainsS;

template<typename U, typename L>
inline constexpr bool Contains = ContainsS<U, L>::value;

template<typename U, typename... T>
struct ContainsS<U, List<T...>>
{
    static constexpr bool value = (std::is_same_v<Head<U>, Head<List<T...>>> || Contains<Head<U>, Tail<List<T...>>>) && Contains<Tail<U>, List<T...>>;
};

template<typename U>
struct ContainsS<U, List<>>
{
    static constexpr bool value = false;
};

template<typename... T>
struct ContainsS<List<>, List<T...>>
{
    static constexpr bool value = true;
};

template<typename L>
struct SizeS;

template<typename L>
inline constexpr size_t Size = SizeS<L>::value;

template<typename... T>
struct SizeS<List<T...>>
{
    static constexpr size_t value = 1 + Size<Tail<List<T...>>>;
};

template<>
struct SizeS<List<>>
{
    static constexpr size_t value = 0;
};

template<typename U, typename L>
struct InsertS;

template<typename U, typename L>
using Insert = typename InsertS<U, L>::Type;

template<typename U, typename... T>
struct InsertS<U, List<T...>>
{
    using Type = List<U, T...>;
};

template<typename U, typename L>
struct InsertBackS;

template<typename U, typename L>
using InsertBack = typename InsertBackS<U, L>::Type;

template<typename U, typename... T>
struct InsertBackS<U, List<T...>>
{
    using Type = List<T..., U>;
};

template<typename... T>
struct MakeListS;

template<typename... T>
using MakeList = typename MakeListS<T...>::Type;

template<typename T, typename... TT>
struct MakeListS<T, TT...>
{
    using Type = Insert<T, MakeList<TT...>>;
};

template<>
struct MakeListS<>
{
    using Type = List<>;
};

template<typename... L>
struct ConcatS;

template<typename... L>
using Concat = typename ConcatS<L...>::Type;

template<typename L1, typename L2, typename... LL>
struct ConcatS<L1, L2, LL...>
{
    using Type = Concat<Concat<L1, L2>, LL...>;
};

template<typename L, typename T, typename... TT>
struct ConcatS<L, List<T, TT...>>
{
    using Type = Concat<InsertBack<T, L>, List<TT...>>;
};

template<typename L>
struct ConcatS<L, List<>>
{
    using Type = L;
};

template<typename... T>
struct ConcatS<List<T...>>
{
    using Type = List<T...>;
};

template<typename U, typename L>
struct AddS;

template<typename U, typename L>
using Add = typename AddS<U, L>::Type;

template<typename U, typename... T>
struct AddS<U, List<T...>>
{
    using Type = std::conditional_t<Contains<U, List<T...>>, List<T...>, List<U, T...>>;;
};

template<typename... T>
struct MakeSetS;

template<typename... T>
using MakeSet = typename MakeSetS<T...>::Type;

template<typename T, typename... TT>
struct MakeSetS<T, TT...>
{
    using Type = Add<T, MakeSet<TT...>>;
};

template<>
struct MakeSetS<>
{
    using Type = List<>;
};

template<typename... LL>
struct MergeS;

template<typename... LL>
using Merge = typename MergeS<LL...>::Type;

template<typename L1, typename L2>
struct Merge2S;

template<typename L1, typename L2>
using Merge2 = typename Merge2S<L1, L2>::Type;

template<typename L1, typename L2, typename... LL>
struct MergeS<L1, L2, LL...>
{
    using Type = Merge<Merge2<L1, L2>, LL...>;
};

template<typename L1, typename L2>
struct MergeS<L1, L2>
{
    using Type = Merge2<L1, L2>;
};

template<typename L1>
struct MergeS<L1>
{
    using Type = L1;
};

template<typename L1, typename T, typename... TT>
struct Merge2S<L1, List<T, TT...>>
{
    using Type = Add<T, Merge2<L1, List<TT...>>>;
};

template<typename L1>
struct Merge2S<L1, List<>>
{
    using Type = L1;
};

template<size_t... I>
struct IntList;

template<size_t I, size_t... II>
struct IntList<I, II...>
{
    static constexpr bool contains(size_t j)
    {
        return j == I || IntList<II...>::contains(j);
    }
};

template<>
struct IntList<>
{
    static constexpr bool contains(size_t)
    {
        return false;
    }
};

template<size_t J, typename L>
struct AddIntListS;

template<size_t J, typename L>
using AddIntList = typename AddIntListS<J, L>::Type;

template<size_t J, size_t... I>
struct AddIntListS<J, IntList<I...>>
{
    using Type = IntList<J, I...>;
};

} /* namespace types */

} /* namespace sqlpp */

#endif /* SQLPP_TYPES_H_ */
