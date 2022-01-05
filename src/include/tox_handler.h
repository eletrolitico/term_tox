#ifndef TOX_HANDLER_H
#define TOX_HANDLER_H

#include <tox/tox.h>
#include <vector>
#include <thread>
#include <memory>

#define PORT_RANGE_START 33445 // tox listen port range
#define PORT_RANGE_END 34445
#define SAVE_DATA_INTERVAL 5 // interval in seconds

struct ToxFile
{
    enum FileStatus
    {
        INITIALIZING = 0,
        PAUSED = 1,
        TRANSMITTING = 2,
        BROKEN = 3,
        CANCELED = 4,
        FINISHED = 5,
    };

    enum FileDirection : bool
    {
        SENDING = 0,
        RECEIVING = 1,
    };

    ToxFile(){};
    ToxFile(uint32_t FileNum, uint32_t FriendId, std::string FileName, FileDirection Direction);

    uint32_t file_num;
    uint32_t friend_num;
    std::string file_name;
    std::shared_ptr<std::fstream> file;
    FileStatus status;
    FileDirection direction;
    uint8_t file_id[TOX_FILE_ID_LENGTH];
};

typedef struct
{
    uint32_t friend_num;
    char *name;
    char *status_message;
    uint8_t pubkey[TOX_PUBLIC_KEY_SIZE];
    TOX_CONNECTION connection;

    std::vector<std::string> hist;
    std::string get_pub_key();
} Friend;

struct FriendUserData
{
    uint8_t pubkey[TOX_PUBLIC_KEY_SIZE];
};

enum class MESSAGE
{
    RECEIVED = 0,
    SENT
};

typedef struct
{
    char *msg;
    uint32_t id;
    bool is_friend_request;
    FriendUserData userdata;
    std::string get_pub_key();
} Request;

class ToxHandler
{
public:
    ToxHandler();
    ~ToxHandler();

    // Actions
    static void send_message(uint32_t fNum, const std::string &msg, bool add_to_msg = true);
    static uint32_t accept_request(Request);
    static TOX_ERR_FRIEND_ADD add_friend(const std::string &toxID, const std::string &msg);

    // Info getters
    static Friend *get_friend(uint32_t fNum);
    static std::vector<Friend *> &get_friends();
    static std::vector<Request> get_requests();
    static std::vector<std::pair<MESSAGE, std::string>> get_messages(uint32_t fNum);
    static std::string get_self_status();
    static std::string get_self_name();
    std::string get_self_tox_address();
    static std::string get_self_status_message();
    static uint32_t get_avg_tox_sleep_time();

    // Setters
    void set_name(const std::string &);
    static void set_status_message(const std::string &);
    static void set_update_callback(void (*update_cb)());

    // Converters
    static const char *connection_enum2text(TOX_CONNECTION);
    static const char *add_friend_err_enum2text(TOX_ERR_FRIEND_ADD);

private:
    std::thread *m_tox_thread;
    std::string m_self_tox_address;

    void (*update_cb)();
    void setup_tox();
    void create_tox();
    void init_friends();
    void bootstrap();
    void update_savedata_file();
    static Friend *add_friend(uint32_t);
};
#endif