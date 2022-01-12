#ifndef TOX_HANDLER_H
#define TOX_HANDLER_H

#include <tox/tox.h>
#include <vector>
#include <thread>
#include <memory>

struct ToxFile
{
    enum class FileStatus
    {
        INITIALIZING = 0,
        PAUSED = 1,
        TRANSMITTING = 2,
        BROKEN = 3,
        CANCELED = 4,
        FINISHED = 5,
    };

    enum class FileDirection : bool
    {
        SENDING = 0,
        RECEIVING = 1,
    };

    ToxFile() {};
    ToxFile(std::shared_ptr<std::fstream> file, uint32_t file_num, uint32_t friend_number, std::string file_name, FileDirection direction)
                : file_{file}, file_num_{file_num}, friend_number_{friend_number}, file_name_{file_name}, direction_{direction}
    {};

    std::shared_ptr<std::fstream> file_;
    uint32_t file_num_;
    uint32_t friend_number_;
    std::string file_name_;
    FileDirection direction_;
    FileStatus status_;
    uint8_t file_id_[TOX_FILE_ID_LENGTH];
};

struct Friend
{
    char *name_;
    char *status_message_;

    uint32_t friend_num_;
    
    uint8_t public_key[TOX_PUBLIC_KEY_SIZE];
    TOX_CONNECTION connection_;

    std::vector<std::string> hist_;
    std::string get_pub_key();
};

struct FriendUserData
{
    uint8_t public_key[TOX_PUBLIC_KEY_SIZE];
};

enum class MESSAGE
{
    RECEIVED = 0,
    SENT
};

struct Request
{
    char *msg_;
    uint32_t id_;
    bool is_friend_request_;
    FriendUserData user_data_;
    std::string get_pub_key();
};

class ToxHandler
{
public:
    ToxHandler(const ToxHandler&) = delete;

    static ToxHandler& get()
    {
        static ToxHandler handler;
        return handler;
    }

    ~ToxHandler();

    // Actions
    static void send_message(uint32_t friend_num, const std::string &msg, bool add_to_msg = true);
    static uint32_t accept_request(Request);
    static TOX_ERR_FRIEND_ADD add_friend(const std::string &toxID, const std::string &msg);

    // Info getters
    static Friend *get_friend(uint32_t friend_num);
    static std::vector<Friend *> &get_friends();
    static std::vector<Request> get_requests();
    static std::vector<std::pair<MESSAGE, std::string>> get_messages(uint32_t friend_num);
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
    ToxHandler();
    
    std::thread *m_tox_thread_;
    std::string m_self_tox_address_;

    void (*update_cb)();
    void setup_tox();
    void create_tox();
    void init_friends();
    void bootstrap();
    void update_savedata_file();
    static Friend *add_friend(uint32_t);
};
#endif