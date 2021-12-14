#ifndef TOX_HANDLER_H
#define TOX_HANDLER_H

#include <tox/tox.h>
#include <vector>
#include <thread>

#define PORT_RANGE_START 33445 // tox listen port range
#define PORT_RANGE_END 34445
#define SAVE_DATA_INTERVAL 5 // interval in seconds

typedef enum FriendUpdate
{
    MESSAGE,
    NAME,
    CON_STATUS,
    STATUS_MSG
} FriendUpdate;

typedef struct
{
    uint32_t friend_num;
    char *name;
    char *status_message;
    uint8_t pubkey[TOX_PUBLIC_KEY_SIZE];
    TOX_CONNECTION connection;

    std::vector<std::string> hist;
} Friend;

struct FriendUserData
{
    uint8_t pubkey[TOX_PUBLIC_KEY_SIZE];
};

typedef struct
{
    char *msg;
    uint32_t id;
    bool is_friend_request;
    FriendUserData userdata;
} Request;

class ToxHandler
{
public:
    ToxHandler();
    ~ToxHandler();

    std::string m_toxID;
    std::string m_name;

    void SetName(const std::string &);
    std::vector<Request> GetRequests();
    std::vector<Friend *> GetFriends();
    Friend *GetFriend(uint32_t fNum);
    void SendMessage(uint32_t fNum, std::string msg);
    uint32_t AcceptRequest(Request);
    uint32_t AddFriend(std::string toxID, std::string msg);

    static const char *connection_enum2text(TOX_CONNECTION conn);

    inline void setUpdateCallback(void (*updateCb)())
    {
        this->updateCb = updateCb;
    }

private:
    void (*updateCb)();
    void setup_tox();
    void create_tox();
    void init_friends();
    void bootstrap();
    void update_savedata_file();
    Friend *add_friend(uint32_t);
};
#endif