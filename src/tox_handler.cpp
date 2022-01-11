#include "include/tox_handler.h"
#include "include/utils.h"

#include <string.h>
#include <sstream>
#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sodium.h>
#include <unordered_map>
#include "include/tox_nodes.h"

#define COUNTOF(x) (sizeof(x) / sizeof(*(x)))

namespace fs = std::filesystem;

std::unordered_map<int, std::vector<std::pair<MESSAGE, std::string>>> messages_;

uint32_t avg_tox_sleep_time_ = 0;
Tox *tox = nullptr;
Friend self_;
std::vector<Friend *> friends;
std::vector<Request> mRequests;
std::unordered_map<uint64_t, ToxFile> mFiles;
const char *savedata_filename = "./savedata.tox";
const char *savedata_tmp_filename = "./savedata.tox.tmp";
bool is_running = false;

// callback para atualizar a interface
void (*iface_update_cb)();

ToxFile::ToxFile(std::shared_ptr<std::fstream> file, uint32_t FileNum, uint32_t FriendId, std::string FileName, FileDirection Direction)
{
    this->file_num_ = FileNum;
    this->friend_num_ = FriendId;
    this->file_name_ = FileName;
    this->file_ = file;
}

/*******************************************************************************
 *
 * Utils
 *
 ******************************************************************************/

Friend *getfriend(uint32_t fid)
{
    for (Friend *fnd : friends)
        if (fnd->friend_num_ == fid)
        {
            return fnd;
            break;
        }
    return nullptr;
}

const char *ToxHandler::connection_enum2text(TOX_CONNECTION conn)
{
    switch (conn)
    {
    case TOX_CONNECTION_NONE:
        return "Offline";
    case TOX_CONNECTION_TCP:
        return "Online(TCP)";
    case TOX_CONNECTION_UDP:
        return "Online(UDP)";
    default:
        return "UNKNOWN";
    }
}

const char *ToxHandler::add_friend_err_enum2text(TOX_ERR_FRIEND_ADD err)
{
    switch (err)
    {
    case TOX_ERR_FRIEND_ADD_OK:
        return "Friend successfully added!";

    case TOX_ERR_FRIEND_ADD_NULL:
        return "One of the arguments to the function was nullptr when it was not expected.";

    case TOX_ERR_FRIEND_ADD_TOO_LONG:
        return "The length of the friend request message exceeded TOX_MAX_FRIEND_REQUEST_LENGTH.";

    case TOX_ERR_FRIEND_ADD_NO_MESSAGE:
        return "The friend request message was empty. This, and the TOO_LONG code will never be returned from tox_friend_add_norequest.";

    case TOX_ERR_FRIEND_ADD_OWN_KEY:
        return "The friend address belongs to the sending client.";

    case TOX_ERR_FRIEND_ADD_ALREADY_SENT:
        return "that is already on the friend list.";

    case TOX_ERR_FRIEND_ADD_BAD_CHECKSUM:
        return "The friend address checksum failed.";

    case TOX_ERR_FRIEND_ADD_SET_NEW_NOSPAM:
        return "The friend was already there, but the nospam value was different.";

    case TOX_ERR_FRIEND_ADD_MALLOC:
        return "A memory allocation failed when trying to increase the friend list size.";

    default:
        return "UNKNOWN";
    }
}

void update_savedata_file(void)
{
    if (!(savedata_filename && savedata_tmp_filename))
        return;

    size_t size = tox_get_savedata_size(tox);
    char *savedata = (char *)malloc(size);
    tox_get_savedata(tox, (uint8_t *)savedata);

    FILE *f = fopen(savedata_tmp_filename, "wb");
    fwrite(savedata, size, 1, f);
    fclose(f);

    rename(savedata_tmp_filename, savedata_filename);

    free(savedata);
}

std::string ls_files()
{
    constexpr std::string_view path = "shared";
    std::string names;
    for (const auto &entry : fs::directory_iterator(path))
    {
        std::string file = entry.path();
        names += file.substr(path.size() + 1);
        if (entry.is_directory())
        {
            names += "/";
        }
        names += ' ';
    }
    return names;
}

std::vector<std::string> tokenize(std::string str)
{
    std::vector<std::string> tokens;
    std::stringstream stream(str);
    std::string intermediate;

    // Tokenizing w.r.t. space ' '
    while (getline(stream, intermediate, ' '))
    {
        tokens.push_back(intermediate);
    }

    return tokens;
}

void download_files(std::vector<std::string> files, uint32_t friend_num)
{
    for (auto f : files)
    {
        auto file = std::make_shared<std::fstream>("shared/" + f, std::ios::binary | std::ios::in);
        if (!file->is_open())
            continue;

        file->seekg(0, std::ios::end);
        uint64_t f_size = file->tellg();

        file->seekg(0, std::ios::beg);

        TOX_ERR_FILE_SEND err;
        uint32_t file_num = tox_file_send(tox, friend_num, TOX_FILE_KIND_DATA, f_size, 0, (uint8_t *)f.c_str(), f.size(), &err);
        if (err != TOX_ERR_FILE_SEND_OK)
        {
            log("tox_file_send falhou com cod " + std::to_string(err));
            continue;
        }

        ToxFile tox_file{file, file_num, friend_num, f, ToxFile::SENDING};
        tox_file_get_file_id(tox, friend_num, file_num, tox_file.file_id_, nullptr);

        mFiles[file_num] = tox_file;
    }
}

/*******************************************************************************
 *
 * Tox Callbacks
 *
 ******************************************************************************/

void friend_message_cb(Tox *tox, uint32_t friend_num, TOX_MESSAGE_TYPE type, const uint8_t *message, size_t length, void *user_data)
{
    Friend *f = getfriend(friend_num);

    if (!f)
        return;

    if (type != TOX_MESSAGE_TYPE_NORMAL)
    {
        log("* receive MESSAGE ACTION type from  no supported");
        return;
    }

    std::string msg = std::string((char *)message);

    if (msg.starts_with("cmd:"))
    {
        constexpr std::string_view dlCmd = "download ";

        std::string cmd = msg.substr(msg.find(":") + 1);
        log("recebido comando: " + cmd);

        if (cmd.starts_with("ls"))
        {
            std::string files = ls_files();
            log(files);
            ToxHandler::send_message(friend_num, files, false);
        }

        if (cmd.starts_with(dlCmd))
        {
            download_files(tokenize(cmd.substr(dlCmd.size())), friend_num);
        }
    }
    else
    {
        f->hist_.push_back(msg);
        messages_[friend_num].push_back({MESSAGE::RECEIVED, msg});

        iface_update_cb();
    }
}

void file_recv_cb(Tox *tox, uint32_t friend_number, uint32_t file_number, uint32_t kind, uint64_t file_size, const uint8_t *filename, size_t filename_length, void *user_data)
{
    TOX_ERR_FILE_CONTROL err;
    tox_file_control(tox, friend_number, file_number, TOX_FILE_CONTROL_RESUME, &err);
    if (err != TOX_ERR_FILE_CONTROL_OK)
    {
        log("file_recv_cb falhou com cod " + std::to_string(err));
        return;
    }

    ToxFile tox_file{std::make_shared<std::fstream>(), file_number, friend_number, std::string((char *)filename), ToxFile::RECEIVING};
    tox_file_get_file_id(tox, friend_number, file_number, tox_file.file_id_, nullptr);

    mFiles[file_number] = tox_file;
}

// mandar chunks
void file_chunk_request_cb(Tox *tox, uint32_t friend_number, uint32_t file_number, uint64_t position, size_t length, void *user_data)
{
    auto file = mFiles[file_number];
    if (!file.file_)
    {
        log("There isn't a file");
        tox_file_send_chunk(tox, friend_number, file_number, position, nullptr, 0, nullptr);
        return;
    }

    if (!length)
    {
        file.status_ = ToxFile::FINISHED;
        file.file_->close();
        return;
    }

    std::unique_ptr<uint8_t[]> data(new uint8_t[length]);
    int64_t nread;

    file.file_->seekg(position);
    file.file_->read(reinterpret_cast<char *>(data.get()), length);
    nread = file.file_->gcount();

    if (nread <= 0 || file.file_->eof())
    {
        log("Failed to read from file");
        tox_file_send_chunk(tox, friend_number, file_number, position, nullptr, 0, nullptr);
        file.file_->close();
        return;
    }

    if (!tox_file_send_chunk(tox, friend_number, file_number, position, data.get(), nread, nullptr))
    {
        log("Failed to send data chunk");
        return;
    }
}

// receber chunks
void file_recv_chunk_cb(Tox *tox, uint32_t friend_number, uint32_t file_number, uint64_t position, const uint8_t *data, size_t length, void *user_data)
{
    auto file = mFiles[file_number];

    if (!length)
    {
        file.status_ = ToxFile::FINISHED;
        file.file_->close();
        return;
    }

    if (!file.file_->is_open())
    {
        file.file_->open("downloads/" + file.file_name_, std::ios::out | std::ios::trunc);
        log("abriu file");
        if (!file.file_->is_open())
        {
            log("deu ruim open file");
            return;
        }
    }

    file.file_->write(reinterpret_cast<const char *>(data), length);
}

// talvez n precisa
void file_recv_control_cb(Tox *tox, uint32_t friend_number, uint32_t file_number, TOX_FILE_CONTROL control, void *user_data)
{
}

void friend_name_cb(Tox *tox, uint32_t friend_num, const uint8_t *name, size_t length, void *user_data)
{
    Friend *f = getfriend(friend_num);

    if (f)
    {
        f->name_ = (char *)realloc(f->name_, length + 1);
        sprintf(f->name_, "%.*s", (int)length, (char *)name);
    }
    iface_update_cb();
}

void friend_status_message_cb(Tox *tox, uint32_t friend_num, const uint8_t *message, size_t length, void *user_data)
{
    Friend *f = getfriend(friend_num);
    if (f)
    {
        f->status_message_ = (char *)realloc(f->status_message_, length + 1);
        sprintf(f->status_message_, "%.*s", (int)length, (char *)message);
    }
    iface_update_cb();
}

void friend_connection_status_cb(Tox *tox, uint32_t friend_num, TOX_CONNECTION connection_status, void *user_data)
{
    Friend *f = getfriend(friend_num);
    if (f)
    {
        f->connection_ = connection_status;
    }
    iface_update_cb();
}

void friend_request_cb(Tox *tox, const uint8_t *public_key, const uint8_t *message, size_t length, void *user_data)
{
    Request req;

    req.id_ = 1 + (mRequests.size() ? mRequests.at(mRequests.size() - 1).id_ : 0);
    req.is_friend_request_ = true;
    memcpy(req.user_data_.public_key, public_key, TOX_PUBLIC_KEY_SIZE);
    req.msg_ = (char *)malloc(length + 1);
    sprintf(req.msg_, "%.*s", (int)length, (char *)message);

    mRequests.push_back(req);
    iface_update_cb();
}

void self_connection_status_cb(Tox *tox, TOX_CONNECTION connection_status, void *user_data)
{
    self_.connection_ = connection_status;
    log("[INFO]: Fiquei online");
    iface_update_cb();
}

const char *log_level[5] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
};

static void log_callback(Tox *tox, TOX_LOG_LEVEL level, const char *file, uint32_t line,
                         const char *func, const char *message, void *user_data)
{
    std::stringstream msg;

    msg << "[" << log_level[level] << "]: ";

    if (message && file && line)
        msg << file << ":" << line << " " << message;
    else if (func)
        msg << func;
    else
        msg << "Bug log toxcore";

    log(msg.str(), "tox_log.log");
}

void ToxHandler::create_tox()
{
    struct Tox_Options options;
    tox_options_default(&options);
    // tox_options_set_start_port(&options, PORT_RANGE_START);
    // tox_options_set_end_port(&options, PORT_RANGE_END);
    tox_options_set_log_callback(&options, log_callback);
    tox_options_set_udp_enabled(&options, true);

    tox_options_set_proxy_type(&options, TOX_PROXY_TYPE_NONE);
    //tox_options_set_experimental_thread_safety(&options, true);
    tox_options_set_hole_punching_enabled(&options, true);
    //tox_options_set_proxy_host(&options, (char *)settings.proxy_ip);
    //tox_options_set_proxy_port(&options, settings.proxy_port);

    TOX_ERR_NEW err_new;

    if (savedata_filename)
    {
        FILE *f = fopen(savedata_filename, "rb");
        if (f)
        {
            fseek(f, 0, SEEK_END);
            long fsize = ftell(f);
            fseek(f, 0, SEEK_SET);

            char *savedata = (char *)malloc(fsize);
            fread(savedata, fsize, 1, f);
            fclose(f);

            tox_options_set_savedata_type(&options, TOX_SAVEDATA_TYPE_TOX_SAVE);
            tox_options_set_savedata_data(&options, (uint8_t *)savedata, fsize);

            tox = tox_new(&options, &err_new);
            if (err_new != TOX_ERR_NEW_OK)
            {
                fprintf(stderr, "tox_new failed with error code %d\n", err_new);
                exit(1);
            }

            free(savedata);
        }
    }

    if (!tox)
    {
        tox = tox_new(&options, &err_new);
        if (err_new != TOX_ERR_NEW_OK)
        {
            fprintf(stderr, "tox_new failed with error code %d\n", err_new);
            exit(1);
        }
    }

    uint8_t tox_id_bin[TOX_ADDRESS_SIZE];
    tox_self_get_address(tox, tox_id_bin);

    char tox_id[TOX_ADDRESS_SIZE * 2 + 1];
    sodium_bin2hex(tox_id, TOX_ADDRESS_SIZE * 2 + 1, tox_id_bin, TOX_ADDRESS_SIZE);

    m_self_tox_address_ = std::string(tox_id);
}

Friend *ToxHandler::add_friend(uint32_t fNum)
{
    Friend *f = (Friend *)calloc(1, sizeof(Friend));
    f->friend_num_ = fNum;
    f->connection_ = TOX_CONNECTION_NONE;
    tox_friend_get_public_key(tox, fNum, f->public_key, nullptr);
    friends.push_back(f);
    return f;
}

void ToxHandler::init_friends()
{
    size_t sz = tox_self_get_friend_list_size(tox);
    uint32_t *friend_list = (uint32_t *)malloc(sizeof(uint32_t) * sz);
    tox_self_get_friend_list(tox, friend_list);

    size_t len;

    for (size_t i = 0; i < sz; i++)
    {
        uint32_t friend_num = friend_list[i];
        Friend *f = add_friend(friend_num);

        len = tox_friend_get_name_size(tox, friend_num, nullptr) + 1;
        f->name_ = (char *)calloc(1, len);
        tox_friend_get_name(tox, friend_num, (uint8_t *)f->name_, nullptr);

        len = tox_friend_get_status_message_size(tox, friend_num, nullptr) + 1;
        f->status_message_ = (char *)calloc(1, len);
        tox_friend_get_status_message(tox, friend_num, (uint8_t *)f->status_message_, nullptr);

        tox_friend_get_public_key(tox, friend_num, f->public_key, nullptr);
    }
    free(friend_list);

    // add self
    self_.friend_num_ = UINT32_MAX;
    len = tox_self_get_name_size(tox) + 1;
    self_.name_ = (char *)calloc(1, len);
    tox_self_get_name(tox, (uint8_t *)self_.name_);

    len = tox_self_get_status_message_size(tox) + 1;
    self_.status_message_ = (char *)calloc(1, len);
    tox_self_get_status_message(tox, (uint8_t *)self_.status_message_);

    tox_self_get_public_key(tox, self_.public_key);
}

void ToxHandler::update_savedata_file()
{
    if (!(savedata_filename && savedata_tmp_filename))
        return;

    size_t size = tox_get_savedata_size(tox);
    char *savedata = (char *)malloc(size);
    tox_get_savedata(tox, (uint8_t *)savedata);

    FILE *f = fopen(savedata_tmp_filename, "wb");
    fwrite(savedata, size, 1, f);
    fclose(f);

    rename(savedata_tmp_filename, savedata_filename);

    free(savedata);
}

void ToxHandler::bootstrap()
{
    static unsigned int j = 0;

    if (j == 0)
    {
        j = rand();
    }

    int i = 0;
    while (i < 4)
    {
        struct bootstrap_node *d = &bootstrap_nodes[j++ % COUNTOF(bootstrap_nodes)];
        // do not add IPv6 bootstrap nodes if IPv6 is not enabled
        if (d->ipv6_)
        {
            continue;
        }
        TOX_ERR_BOOTSTRAP btErr;
        bool ok;
        ok = tox_bootstrap(tox, d->address_, d->port_udp_, d->key_, &btErr);
        if (btErr != TOX_ERR_BOOTSTRAP_OK || !ok)
        {
            printf("err bootstrap: %d", btErr);
        }
        ok = tox_add_tcp_relay(tox, d->address_, d->port_tcp_, d->key_, &btErr);
        if (btErr != TOX_ERR_BOOTSTRAP_OK || !ok)
        {
            printf("err bootstrap relay: %d", btErr);
        }
        i++;
    }
}

void ToxHandler::setup_tox()
{
    create_tox();

    // register callbacks
    tox_callback_self_connection_status(tox, self_connection_status_cb);
    tox_callback_friend_request(tox, friend_request_cb);
    tox_callback_friend_message(tox, friend_message_cb);
    tox_callback_friend_name(tox, friend_name_cb);
    tox_callback_friend_status_message(tox, friend_status_message_cb);
    tox_callback_friend_connection_status(tox, friend_connection_status_cb);

    // files
    tox_callback_file_chunk_request(tox, file_chunk_request_cb);
    tox_callback_file_recv_chunk(tox, file_recv_chunk_cb);
    tox_callback_file_recv_control(tox, file_recv_control_cb);
    tox_callback_file_recv(tox, file_recv_cb);

    bootstrap();
    init_friends();
}

void ToxHandler::set_name(const std::string &str)
{
    tox_self_set_name(tox, (uint8_t *)str.c_str(), str.size(), nullptr);

    if (self_.name_)
        free(self_.name_);
    self_.name_ = (char *)malloc((str.size() + 1) * sizeof(char));
    strcpy(self_.name_, str.c_str());
}

void ToxHandler::set_status_message(const std::string &str)
{
    tox_self_set_status_message(tox, (uint8_t *)str.c_str(), str.size(), nullptr);

    if (self_.status_message_)
        free(self_.status_message_);
    self_.status_message_ = (char *)malloc((str.size() + 1) * sizeof(char));
    strcpy(self_.status_message_, str.c_str());
}

std::vector<Request> ToxHandler::get_requests()
{
    return mRequests;
}

std::vector<Friend *> &ToxHandler::get_friends()
{
    return friends;
}

Friend *ToxHandler::get_friend(uint32_t fNum)
{
    return getfriend(fNum);
}

void ToxHandler::send_message(uint32_t fNum, const std::string &msg, bool add_to_msg)
{
    TOX_ERR_FRIEND_SEND_MESSAGE err;
    tox_friend_send_message(tox, fNum, TOX_MESSAGE_TYPE_NORMAL, (uint8_t *)msg.c_str(), msg.length(), &err);
    if (err != TOX_ERR_FRIEND_SEND_MESSAGE_OK)
    {
        log("Erro ao mandar msg: " + std::to_string(err));
        return;
    }

    if (add_to_msg)
        messages_[fNum].push_back({MESSAGE::SENT, msg});
}

TOX_ERR_FRIEND_ADD ToxHandler::add_friend(const std::string &toxID, const std::string &msg)
{
    const char *tox_id = toxID.c_str();

    uint8_t tox_id_bin[TOX_ADDRESS_SIZE];
    sodium_hex2bin(tox_id_bin, TOX_ADDRESS_SIZE, tox_id, strnlen(tox_id, TOX_ADDRESS_SIZE * 2 + 1), nullptr, nullptr, nullptr);

    TOX_ERR_FRIEND_ADD err;
    uint32_t friend_num = tox_friend_add(tox, tox_id_bin, (uint8_t *)msg.c_str(), msg.length(), &err);

    if (err != TOX_ERR_FRIEND_ADD_OK)
    {
        log("add friend with id " + std::string(tox_id) + " failed, errcode: " + std::to_string(err));
        return err;
    }

    add_friend(friend_num);
    return TOX_ERR_FRIEND_ADD_OK;
}

uint32_t ToxHandler::accept_request(Request req)
{
    TOX_ERR_FRIEND_ADD err;
    uint32_t friend_num = tox_friend_add_norequest(tox, req.user_data_.public_key, &err);
    if (err != TOX_ERR_FRIEND_ADD_OK)
    {
        printf("! accept friend request failed, errcode:%d", err);
        return -1;
    }

    add_friend(friend_num);
    return friend_num;
}

std::string ToxHandler::get_self_status()
{
    return std::string(ToxHandler::connection_enum2text(self_.connection_));
}

void iterate_tox()
{
    uint32_t tmp;
    while (is_running)
    {
        tox_iterate(tox, nullptr);
        tmp = tox_iteration_interval(tox);

        if (avg_tox_sleep_time_ == 0)
        {
            avg_tox_sleep_time_ = tmp;
        }
        else
        {
            avg_tox_sleep_time_ += tmp;
            avg_tox_sleep_time_ /= 2;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(tmp));
    }

    update_savedata_file();
    tox_kill(tox);
    printf("terminating thread\n");
}

std::string ToxHandler::get_self_name()
{
    return std::string(self_.name_);
}
std::string ToxHandler::get_self_tox_address()
{
    return m_self_tox_address_;
}
std::string ToxHandler::get_self_status_message()
{
    return std::string(self_.status_message_);
}

ToxHandler::ToxHandler()
{
    setup_tox();
    if (!get_self_name().size())
    {
        set_name("Toxer");
    }
    is_running = true;
    m_tox_thread_ = new std::thread(iterate_tox);
}

ToxHandler::~ToxHandler()
{
    is_running = false;
    m_tox_thread_->join();
    delete m_tox_thread_;
}

std::string Friend::get_pub_key()
{
    char key[TOX_PUBLIC_KEY_SIZE * 2 + 1];
    sodium_bin2hex(key, TOX_PUBLIC_KEY_SIZE * 2 + 1, this->public_key, TOX_PUBLIC_KEY_SIZE);
    return std::string(key);
}

std::string Request::get_pub_key()
{
    char key[TOX_PUBLIC_KEY_SIZE * 2 + 1];
    sodium_bin2hex(key, TOX_PUBLIC_KEY_SIZE * 2 + 1, this->user_data_.public_key, TOX_PUBLIC_KEY_SIZE);
    return std::string(key);
}

uint32_t ToxHandler::get_avg_tox_sleep_time()
{
    return avg_tox_sleep_time_;
}

void ToxHandler::set_update_callback(void (*update_cb)())
{
    iface_update_cb = update_cb;
}

std::vector<std::pair<MESSAGE, std::string>> ToxHandler::get_messages(uint32_t fNUm)
{
    return messages_[fNUm];
}