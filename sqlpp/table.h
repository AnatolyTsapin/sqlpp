#ifndef SQLPP_TABLE_H_
#define SQLPP_TABLE_H_

#include <sqlpp/column.h>
#include <sqlpp/types.h>

#include <string>
#include <tuple>
#include <array>

namespace sqlpp
{

template<typename T, typename... V>
class Table
{
public:
    using Self = T;

    using Row = std::tuple<typename Converter<V>::DbType...>;

    template<size_t N>
    using Field = std::tuple_element_t<N, Row>;

    template<size_t N>
    using Column = sqlpp::Column<Self, std::tuple_element_t<N, std::tuple<V...>>>;

    static constexpr size_t COLUMN_COUNT = std::tuple_size_v<Row>;

    Table(std::string name, std::array<std::string, COLUMN_COUNT> columnNames) :
        name(std::move(name)), columnNames(std::move(columnNames))
    {}
    virtual ~Table() = default;

    const std::string& getName() const
    {
        return name;
    }

    template<size_t N>
    Column<N> column() const
    {
        return Column<N>(static_cast<const Self&>(*this), columnNames[N]);
    }

    const std::string& getColumnName(size_t i) const
    {
        return columnNames[i];
    }

private:
    const std::string name;
    const std::array<std::string, COLUMN_COUNT> columnNames;
};

} /* namespace sqlpp */

#endif /* SQLPP_TABLE_H_ */
