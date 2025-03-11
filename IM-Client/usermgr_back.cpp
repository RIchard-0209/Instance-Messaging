// #include "usermgr.h"

// #include "tcpmgr.h"

// UserMgr::~UserMgr() {}

// void UserMgr::SetUserInfo(std::shared_ptr<UserInfo> user_info) {
//   _user_info = user_info;
// }

// void UserMgr::SetToken(QString token) { _token = token; }

// int UserMgr::GetUid() { return _user_info->_uid; }

// QString UserMgr::GetName() { return _user_info->_name; }

// QString UserMgr::GetIocn() { return _user_info->_icon; }

// std::vector<std::shared_ptr<ApplyInfo>> UserMgr::GetApplyList() {
//   return _apply_list;
// }

// std::vector<std::shared_ptr<FriendInfo>> UserMgr::GetChatListPerPage() {
//   std::vector<std::shared_ptr<FriendInfo>> friend_list;
//   int begin = _chat_loaded;
//   int end = begin + CHAT_COUNT_PER_PAGE;

//   if (begin >= _friend_list.size()) return friend_list;

//   if (end > _friend_list.size()) {
//     friend_list = std::vector<std::shared_ptr<FriendInfo>>(
//         _friend_list.begin() + begin, _friend_list.end());
//     return friend_list;
//   }

//   friend_list = std::vector<std::shared_ptr<FriendInfo>>(
//       _friend_list.begin() + begin, _friend_list.begin() + end);
//   return friend_list;
// }

// std::vector<std::shared_ptr<FriendInfo>> UserMgr::GetConListPerPage() {
//   std::vector<std::shared_ptr<FriendInfo>> friend_list;
//   int begin = _chat_loaded;
//   int end = begin + CHAT_COUNT_PER_PAGE;

//   if (begin >= _friend_list.size()) return friend_list;

//   if (end > _friend_list.size()) {
//     friend_list = std::vector<std::shared_ptr<FriendInfo>>(
//         _friend_list.begin() + begin, _friend_list.end());
//     return friend_list;
//   }

//   friend_list = std::vector<std::shared_ptr<FriendInfo>>(
//       _friend_list.begin() + begin, _friend_list.begin() + end);
//   return friend_list;
// }

// void UserMgr::AppendApplyList(QJsonArray array) {
//   for (const QJsonValue& value : array) {
//     auto info = std::make_shared<ApplyInfo>(
//         value["name"].toString(), value["desc"].toString(),
//         value["icon"].toString(), value["nick"].toString(),
//         value["sex"].toInt(), value["uid"].toInt(), value["status"].toString());
//     _apply_list.push_back(info);
//   }
// }

// void UserMgr::AppendFriendList(QJsonArray array) {
//   for (const QJsonValue& value : array) {
//     auto info = std::make_shared<FriendInfo>(
//         value["name"].toString(), value["desc"].toString(),
//         value["icon"].toString(), value["nick"].toString(),
//         value["sex"].toInt(), value["uid"].toInt(), value["status"].toString());
//     _friend_list.push_back(info);
//     _frined_map.insert(value["uid"].toInt(), info);
//   }
// }

// void UserMgr::UpdateChatLoadedCount() {
//   int begin = _chat_loaded;
//   int end = begin + CHAT_COUNT_PER_PAGE;

//   if (begin >= _friend_list.size()) return;
//   if (end > _friend_list.size()) {
//     _chat_loaded = _friend_list.size();
//     return;
//   }

//   _chat_loaded = end;
// }

// void UserMgr::UpdateContactLoadedCount() {
//   int begin = _contact_loaded;
//   int end = begin + CHAT_COUNT_PER_PAGE;

//   if (begin >= _friend_list.size()) return;
//   if (end > _friend_list.size()) {
//     _contact_loaded = _friend_list.size();
//     return;
//   }

//   _contact_loaded = end;
// }

// bool UserMgr::IsLoadChatFin() {
//   return _chat_loaded >= _friend_list.size() ? true : false;
// }

// bool UserMgr::IsLoadConFin() {
//   return _contact_loaded >= _friend_list.size() ? true : false;
// }

// bool UserMgr::CheckFriendById(int uid) {
//   return _frined_map.find(uid) == _frined_map.end() ? true : false;
// }

// void UserMgr::AddFriend(std::shared_ptr<AuthRsp> auth_rsp) {
//   auto friend_info = std::make_shared<FriendInfo>(auth_rsp);
//   _frined_map[friend_info->_uid] = friend_info;
// }

// void UserMgr::AddFriend(std::shared_ptr<AuthInfo> auth_info) {
//   auto friend_info = std::make_shared<FriendInfo>(auth_info);
//   _frined_map[friend_info->_uid] = friend_info;
// }

// void UserMgr::SlotAddFriendRsp(std::shared_ptr<AuthRsp> rsp) { AddFriend(rsp); }

// void UserMgr::SlotAddFriendAuth(std::shared_ptr<AuthInfo> auth) {
//   AddFriend(auth);
// }

// UserMgr::UserMgr() : _user_info(nullptr), _chat_loaded(0), _contact_loaded(0) {
//   connect(TCPMgr::GetInstance().get(), &TCPMgr::sign_auth_rsp, this,
//           &UserMgr::SlotAddFriendRsp);
//   connect(TCPMgr::GetInstance().get(), &TCPMgr::sign_add_auth_friend, this,
//           &UserMgr::SlotAddFriendAuth);
// }
