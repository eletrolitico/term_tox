#include "include/tox_handler.h"
#include "include/utils.h"

#include <string.h>
#include <iostream>
#include <sodium.h>
#include "include/tox_nodes.h"

#define COUNTOF(x) (sizeof(x) / sizeof(*(x)))

Tox *mTox = NULL;
Friend self;
std::vector<Friend *> mFriends;
std::vector<Request> mRequests;
const char *savedata_filename = "./savedata.tox";
const char *savedata_tmp_filename = "./savedata.tox.tmp";

/*******************************************************************************
 *
 * Utils
 *
 ******************************************************************************/

Friend *getfriend(uint32_t fid)
{
    for (Friend *fnd : mFriends)
        if (fnd->friend_num == fid)
        {
            return fnd;
            break;
        }
    return NULL;
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
        return "One of the arguments to the function was NULL when it was not expected.";

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

    size_t size = tox_get_savedata_size(mTox);
    char *savedata = (char *)malloc(size);
    tox_get_savedata(mTox, (uint8_t *)savedata);

    FILE *f = fopen(savedata_tmp_filename, "wb");
    fwrite(savedata, size, 1, f);
    fclose(f);

    rename(savedata_tmp_filename, savedata_filename);

    free(savedata);
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
        printf("* receive MESSAGE ACTION type from %s, no supported\n", f->name);
        return;
    }

    f->hist.push_back(std::string((char *)message));

    printf("* receive message from %s\n", f->name);
}

void friend_name_cb(Tox *tox, uint32_t friend_num, const uint8_t *name, size_t length, void *user_data)
{
    Friend *f = getfriend(friend_num);

    if (f)
    {
        f->name = (char *)realloc(f->name, length + 1);
        sprintf(f->name, "%.*s", (int)length, (char *)name);
    }
}

void friend_status_message_cb(Tox *tox, uint32_t friend_num, const uint8_t *message, size_t length, void *user_data)
{
    Friend *f = getfriend(friend_num);
    if (f)
    {
        f->status_message = (char *)realloc(f->status_message, length + 1);
        sprintf(f->status_message, "%.*s", (int)length, (char *)message);
    }
}

void friend_connection_status_cb(Tox *tox, uint32_t friend_num, TOX_CONNECTION connection_status, void *user_data)
{
    Friend *f = getfriend(friend_num);
    if (f)
    {
        f->connection = connection_status;
        printf("* %s is %s\n", f->name, ToxHandler::connection_enum2text(connection_status));
    }
}

void friend_request_cb(Tox *tox, const uint8_t *public_key, const uint8_t *message, size_t length, void *user_data)
{
    Request req;

    req.id = 1 + (mRequests.size() ? mRequests.at(mRequests.size() - 1).id : 0);
    req.is_friend_request = true;
    memcpy(req.userdata.pubkey, public_key, TOX_PUBLIC_KEY_SIZE);
    req.msg = (char *)malloc(length + 1);
    sprintf(req.msg, "%.*s", (int)length, (char *)message);

    mRequests.push_back(req);
}

void self_connection_status_cb(Tox *tox, TOX_CONNECTION connection_status, void *user_data)
{
    self.connection = connection_status;
    printf("* You are %s\n", ToxHandler::connection_enum2text(connection_status));
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
    printf("[%s]: ", log_level[level]);

    if (message && file && line)
        printf("%s:%d %s", file, line, message);
    else if (func)
        printf("%s", func);
    else
        printf("Bug log toxcore");

    printf("\n");
}

void ToxHandler::create_tox()
{
    struct Tox_Options options;
    tox_options_default(&options);
    //tox_options_set_start_port(options, PORT_RANGE_START);
    //tox_options_set_end_port(options, PORT_RANGE_END);
    tox_options_set_log_callback(&options, log_callback);
    //tox_options_set_udp_enabled(&options, !settings.disableudp);

    tox_options_set_proxy_type(&options, TOX_PROXY_TYPE_NONE);
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

            mTox = tox_new(&options, &err_new);
            if (err_new != TOX_ERR_NEW_OK)
            {
                fprintf(stderr, "tox_new failed with error code %d\n", err_new);
                exit(1);
            }

            free(savedata);
        }
    }

    if (!mTox)
    {
        mTox = tox_new(&options, &err_new);
        if (err_new != TOX_ERR_NEW_OK)
        {
            fprintf(stderr, "tox_new failed with error code %d\n", err_new);
            exit(1);
        }
    }

    uint8_t tox_id_bin[TOX_ADDRESS_SIZE];
    tox_self_get_address(mTox, tox_id_bin);

    char tox_id[TOX_ADDRESS_SIZE * 2 + 1];
    sodium_bin2hex(tox_id, TOX_ADDRESS_SIZE * 2 + 1, tox_id_bin, TOX_ADDRESS_SIZE);

    m_toxID = std::string(tox_id);
}

Friend *ToxHandler::add_friend(uint32_t fNum)
{
    Friend *f = (Friend *)calloc(1, sizeof(Friend));
    f->friend_num = fNum;
    f->connection = TOX_CONNECTION_NONE;
    tox_friend_get_public_key(mTox, fNum, f->pubkey, NULL);
    mFriends.push_back(f);
    return f;
}

void ToxHandler::init_friends()
{
    size_t sz = tox_self_get_friend_list_size(mTox);
    uint32_t *friend_list = (uint32_t *)malloc(sizeof(uint32_t) * sz);
    tox_self_get_friend_list(mTox, friend_list);

    size_t len;

    for (size_t i = 0; i < sz; i++)
    {
        uint32_t friend_num = friend_list[i];
        Friend *f = add_friend(friend_num);

        len = tox_friend_get_name_size(mTox, friend_num, NULL) + 1;
        f->name = (char *)calloc(1, len);
        tox_friend_get_name(mTox, friend_num, (uint8_t *)f->name, NULL);

        len = tox_friend_get_status_message_size(mTox, friend_num, NULL) + 1;
        f->status_message = (char *)calloc(1, len);
        tox_friend_get_status_message(mTox, friend_num, (uint8_t *)f->status_message, NULL);

        tox_friend_get_public_key(mTox, friend_num, f->pubkey, NULL);
    }
    free(friend_list);

    // add self
    self.friend_num = UINT32_MAX;
    len = tox_self_get_name_size(mTox) + 1;
    self.name = (char *)calloc(1, len);
    tox_self_get_name(mTox, (uint8_t *)self.name);

    len = tox_self_get_status_message_size(mTox) + 1;
    self.status_message = (char *)calloc(1, len);
    tox_self_get_status_message(mTox, (uint8_t *)self.status_message);

    tox_self_get_public_key(mTox, self.pubkey);

    m_name = std::string(self.name);
}

void ToxHandler::update_savedata_file()
{
    if (!(savedata_filename && savedata_tmp_filename))
        return;

    size_t size = tox_get_savedata_size(mTox);
    char *savedata = (char *)malloc(size);
    tox_get_savedata(mTox, (uint8_t *)savedata);

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
        if (d->ipv6)
        {
            continue;
        }
        tox_bootstrap(mTox, d->address, d->port_udp, d->key, 0);
        tox_add_tcp_relay(mTox, d->address, d->port_tcp, d->key, 0);
        i++;
    }
}

void ToxHandler::setup_tox()
{
    create_tox();

    // register callbacks
    tox_callback_self_connection_status(mTox, self_connection_status_cb);
    tox_callback_friend_request(mTox, friend_request_cb);
    tox_callback_friend_message(mTox, friend_message_cb);
    tox_callback_friend_name(mTox, friend_name_cb);
    tox_callback_friend_status_message(mTox, friend_status_message_cb);
    tox_callback_friend_connection_status(mTox, friend_connection_status_cb);

    bootstrap();
    init_friends();
}

void ToxHandler::set_name(const std::string &str)
{
    m_name = str;
    tox_self_set_name(mTox, (uint8_t *)str.c_str(), str.size(), NULL);
    auto stsMsg = "sts msg";
    tox_self_set_status_message(mTox, (uint8_t *)stsMsg, strlen(stsMsg), NULL);
}

std::vector<Request> ToxHandler::get_requests()
{
    return mRequests;
}

std::vector<Friend *> ToxHandler::get_friends()
{
    return mFriends;
}

Friend *ToxHandler::get_friend(uint32_t fNum)
{
    return getfriend(fNum);
}

void ToxHandler::send_message(uint32_t fNum, const std::string &msg)
{
    TOX_ERR_FRIEND_SEND_MESSAGE err;
    tox_friend_send_message(mTox, fNum, TOX_MESSAGE_TYPE_NORMAL, (uint8_t *)msg.c_str(), msg.length(), &err);
    if (err != TOX_ERR_FRIEND_SEND_MESSAGE_OK)
    {
        std::cout << "Erro ao mandar msg: " << err << std::endl;
    }
}

TOX_ERR_FRIEND_ADD ToxHandler::add_friend(const std::string &toxID, const std::string &msg)
{
    const char *tox_id = toxID.c_str();

    uint8_t tox_id_bin[TOX_ADDRESS_SIZE];
    sodium_hex2bin(tox_id_bin, TOX_ADDRESS_SIZE, tox_id, strnlen(tox_id, TOX_ADDRESS_SIZE * 2 + 1), NULL, NULL, NULL);

    TOX_ERR_FRIEND_ADD err;
    uint32_t friend_num = tox_friend_add(mTox, tox_id_bin, (uint8_t *)msg.c_str(), msg.length(), &err);

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
    uint32_t friend_num = tox_friend_add_norequest(mTox, req.userdata.pubkey, &err);
    if (err != TOX_ERR_FRIEND_ADD_OK)
    {
        printf("! accept friend request failed, errcode:%d", err);
        return -1;
    }

    add_friend(friend_num);
    return friend_num;
}

ToxHandler::ToxHandler()
{
    setup_tox();
}

ToxHandler::~ToxHandler()
{
}