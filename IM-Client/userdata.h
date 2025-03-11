#ifndef USERDATA_H
#define USERDATA_H
#include <QString>
class SearchInfo {
public:
    SearchInfo(int uid, QString name, QString nick, QString desc, int sex);
    int _uid;
    QString _name;
    QString _nick;
    QString _desc;
    int _sex;
};

class AddFriendApply {
public:
    AddFriendApply(int from_uid, QString name, QString desc);
    int _from_uid;
    QString _name;
    QString _desc;
};

#endif
