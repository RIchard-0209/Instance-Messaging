#pragma once

// 查询语句
extern const char* SELECT_USER_ALL_BY_UID = "SELECT * FROM user WHERE uid = ?";

extern const char* SELECT_USER_ALL_BY_NAME = "SELECT * from user WHERE name = ?";

extern const char* SELECT_USER_EMAIL_BY_NAME = "SELECT email from user WHERE name = ?";

extern const char* KEEP_ALIVE_QUERY = "SELECT 1";

// 更新语句
extern const char* UPDATE_USER_PWD_BY_NAME = "UPDATE user SET pwd = ? where name = ?";


// 调用存储过程
extern const char* CALL_REG_USER = "CALL reg_user(?,?,?,@result)";
extern const char* REG_USER_QUERY = "SELECT @result AS result";