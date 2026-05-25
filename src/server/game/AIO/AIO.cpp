/*
 * All-In-One server <-> client addon bridge.
 */

#include "AIO.h"
#include "Player.h"
#include "WorldSession.h"
#include <sstream>

namespace AIO
{
namespace
{
void SendRaw(Player* player, std::string const& message)
{
    if (!player || !player->GetSession())
        return;

    player->WhisperAddon(message, Manager::PREFIX, false, player);
}
}

namespace Codec
{
std::string Encode(std::initializer_list<std::pair<std::string_view, std::string_view>> fields)
{
    std::ostringstream oss;
    bool first = true;
    for (auto const& [key, value] : fields)
    {
        if (!first)
            oss << ';';
        first = false;
        oss << key << '=' << value;
    }
    return oss.str();
}

std::unordered_map<std::string, std::string> Decode(std::string_view payload)
{
    std::unordered_map<std::string, std::string> out;
    while (!payload.empty())
    {
        size_t semi = payload.find(';');
        std::string_view token = payload.substr(0, semi);
        if (semi == std::string_view::npos)
            payload = {};
        else
            payload = payload.substr(semi + 1);

        size_t eq = token.find('=');
        if (eq == std::string_view::npos)
            continue;

        out.emplace(token.substr(0, eq), token.substr(eq + 1));
    }
    return out;
}

std::string Get(std::unordered_map<std::string, std::string> const& map, std::string_view key,
    std::string_view fallback)
{
    auto it = map.find(std::string(key));
    if (it == map.end())
        return std::string(fallback);
    return it->second;
}

} // namespace Codec

Manager::Manager()
{
    _capabilities = { "core", "dev", "localhost" };
}

Manager* Manager::instance()
{
    static Manager instance;
    return &instance;
}

void Manager::RegisterHandler(std::string name, HandlerFn handler)
{
    _handlers[std::move(name)] = std::move(handler);
}

void Manager::UnregisterHandler(std::string const& name)
{
    _handlers.erase(name);
}

bool Manager::HandleIncoming(Player* player, std::string_view message)
{
    if (!player || message.empty())
        return false;

    size_t delim = message.find(DELIM);
    if (delim == std::string_view::npos)
        return false;

    std::string_view handlerName = message.substr(0, delim);
    std::string_view payload = message.substr(delim + 1);

    auto it = _handlers.find(std::string(handlerName));
    if (it == _handlers.end())
        return false;

    it->second(player, payload);
    return true;
}

void Manager::Send(Player* player, std::string_view handler, std::string_view payload)
{
    if (!player)
        return;

    std::string message;
    message.reserve(handler.size() + payload.size() + 1);
    message.append(handler).push_back(DELIM);
    message.append(payload);
    SendRaw(player, message);
}

void Manager::SendTable(Player* player, std::string_view handler,
    std::initializer_list<std::pair<std::string_view, std::string_view>> fields)
{
    Send(player, handler, Codec::Encode(fields));
}

void Manager::SendRaw(Player* player, std::string const& message)
{
    if (!player || !player->GetSession())
        return;

    player->WhisperAddon(message, PREFIX, false, player);
}

} // namespace AIO
