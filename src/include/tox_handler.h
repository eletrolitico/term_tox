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

    void set_name(const std::string &);
    std::vector<Request> get_requests();
    std::vector<Friend *> get_friends();
    Friend *get_friend(uint32_t fNum);
    void send_message(uint32_t fNum, const std::string &msg);
    uint32_t accept_request(Request);
    TOX_ERR_FRIEND_ADD add_friend(const std::string &toxID, const std::string &msg);

    static const char *connection_enum2text(TOX_CONNECTION);
    static const char *add_friend_err_enum2text(TOX_ERR_FRIEND_ADD);

    inline void set_update_callback(void (*update_cb)())
    {
        this->update_cb = update_cb;
    }

private:
    void (*update_cb)();
    void setup_tox();
    void create_tox();
    void init_friends();
    void bootstrap();
    void update_savedata_file();
    Friend *add_friend(uint32_t);
};
#endif