#include "SQLconstance.h"

const char* CALL_PROC_REG_USER = "CALL reg_user(?,?,?,@result)";
const char* PROC_REG_USER_QUERY = "SELECT @result AS result";

const char* SELECT_USER_EMAIL_BY_NAME = "SELECT email from user WHERE name = ?";
const char* SELECT_USER_ALL_BY_NAME = "SELECT * from user WHERE name = ?";
const char* SELECT_USER_ALL_BY_UID = "SELECT * FROM user WHERE uid = ?";

const char* UPDATE_USER_PWD_BY_NAME = "UPDATE user SET pwd = ? where name = ?";