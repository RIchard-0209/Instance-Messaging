// #ifndef USERMGR_H
// #define USERMGR_H

// #include <QJsonArray>
// #include <QObject>
// #include <memory>
// #include <vector>

// #include "singleton.h"
// #include "userdata.h"


// class UserMgr : public QObject,
//                 public Singleton<UserMgr>,
//                 public std::enable_shared_from_this<UserMgr> {
//   Q_OBJECT
//  public:
//   friend class Singleton<UserMgr>;
//   ~UserMgr();
//   void SetUserInfo(std::shared_ptr<UserInfo> user_info);
//   void SetToken(QString token);

//   int GetUid();
//   QString GetName();
//   QString GetIocn();
//   std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();
//   std::vector<std::shared_ptr<FriendInfo>> GetChatListPerPage();
//   std::vector<std::shared_ptr<FriendInfo>> GetConListPerPage();
//   void AppendApplyList(QJsonArray array);
//   void AppendFriendList(QJsonArray array);
//   void UpdateChatLoadedCount();
//   void UpdateContactLoadedCount();

//   bool IsLoadChatFin();
//   bool IsLoadConFin();
//   bool CheckFriendById(int uid);

//   void AddFriend(std::shared_ptr<AuthRsp> auth_rsp);
//   void AddFriend(std::shared_ptr<AuthInfo> auth_info);

//  public slots:
//   void SlotAddFriendRsp(std::shared_ptr<AuthRsp> rsp);
//   void SlotAddFriendAuth(std::shared_ptr<AuthInfo> auth);

//  private:
//   UserMgr();
//   std::shared_ptr<UserInfo> _user_info;
//   std::vector<std::shared_ptr<ApplyInfo>> _apply_list;
//   std::vector<std::shared_ptr<FriendInfo>> _friend_list;
//   QMap<int, std::shared_ptr<FriendInfo>> _frined_map;
//   QString _token;
//   int _chat_loaded;
//   int _contact_loaded;
// };

// #endif  // USERMGR_H
