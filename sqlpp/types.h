#ifndef SQLPP_TYPES_H_
#define SQLPP_TYPES_H_

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <string>
#include <vector>
#include <functional>

typedef struct sqlite3_stmt sqlite3_stmt;

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
auto toDb(V&& value)
{
    return Converter<V>::toDb(std::forward<V>(value));
}

template<typename V>
auto fromDb(V&& value)
{
    return Converter<V>::fromDb(std::forward<V>(value));
}

template<typename V>
struct Converter<V, std::enable_if_t<
    std::is_same_v<remove_cvref_t<V>, Integer> ||
    std::is_same_v<remove_cvref_t<V>, Real> ||
    std::is_same_v<remove_cvref_t<V>, Text> ||
    std::is_same_v<remove_cvref_t<V>, Blob>>>
{
    using DbType = remove_cvref_t<V>;
    static DbType toDb(V&& value)
    {
        return std::forward<V>(value);
    }
    static V fromDb(DbType&& value)
    {
        return std::forward<V>(value);
    }
};

template<typename V>
struct Converter<V, std::enable_if_t<
    std::is_integral_v<remove_cvref_t<V>> && !std::is_same_v<remove_cvref_t<V>, Integer>>>
{
    using DbType = Integer;
    static Integer toDb(V&& value)
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
    static Real toDb(V&& value)
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

namespace types
{

template<typename... T>
struct PackSizeS;

template<typename... T>
inline constexpr size_t PackSize = PackSizeS<T...>::value;

template<typename T, typename... TT>
struct PackSizeS<T, TT...>
{
    static constexpr size_t value = 1 + PackSize<TT...>;
};

template<>
struct PackSizeS<>
{
    static constexpr size_t value = 0;
};

template<typename... T>
struct List
{};

template<typename T, typename... TT>
struct List<T, TT...>
{
    using Head = T;
    using Tail = List<TT...>;
};

template<typename L>
using Head = typename L::Head;

template<typename L>
using Tail = typename L::Tail;

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
    static constexpr bool value = std::is_same_v<U, Head<List<T...>>> || Contains<U, Tail<List<T...>>>;
};

template<typename U>
struct ContainsS<U, List<>>
{
    static constexpr bool value = false;
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
struct AddS;

template<typename U, typename L>
using Add = typename AddS<U, L>::Type;

template<typename U, typename... T>
struct AddS<U, List<T...>>
{
    using Type = std::conditional_t<Contains<U, List<T...>>, List<T...>, List<U, T...>>;;
};

template<typename... T>
struct MakeListS;

template<typename... T>
using MakeList = typename MakeListS<T...>::Type;

template<typename T, typename... TT>
struct MakeListS<T, TT...>
{
    using Type = Add<T, MakeList<TT...>>;
};

template<>
struct MakeListS<>
{
    using Type = List<>;
};

} /* namespace types */

} /* namespace sqlpp */

#endif /* SQLPP_TYPES_H_ */
