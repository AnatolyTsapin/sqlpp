#include <sqlpp/stmt/common.h>

namespace sqlpp {

Statement::Statement() = default;
Statement::Statement(Statement&&) = default;
Statement::Statement(const Statement&) = default;
Statement::~Statement() = default;
Statement& Statement::operator=(const Statement&) = default;
Statement& Statement::operator=(Statement&&) = default;

} /* namespace sqlpp */
