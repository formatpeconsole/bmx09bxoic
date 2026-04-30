#include "uuid.h"

namespace uuid
{
// https://stackoverflow.com/a/72358504
std::string getUuid()
{
    UUID uuid;
    RPC_CSTR  uuid_str;
    std::string uuid_out;

    if (UuidCreate(&uuid) != RPC_S_OK)
        return "";

    if (UuidToStringA(&uuid, &uuid_str) != RPC_S_OK)
        return "";

    uuid_out = (char*)uuid_str;
    RpcStringFreeA(&uuid_str);
    return uuid_out;
}
}