/*
 * All-In-One server <-> client addon bridge.
 * Wire format: WHISPER addon message, prefix "AIO", body "handler<TAB>payload".
 * Payload encoding: key=value pairs separated by ';' (see AIO::Codec).
 */

#ifndef TRINITY_AIO_H
#define TRINITY_AIO_H

#include "Define.h"
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

class Player;

namespace AIO
{
    using HandlerFn = std::function<void(Player* player, std::string_view payload)>;

    namespace Codec
    {
        std::string Encode(std::initializer_list<std::pair<std::string_view, std::string_view>> fields);
        std::unordered_map<std::string, std::string> Decode(std::string_view payload);
        std::string Get(std::unordered_map<std::string, std::string> const& map, std::string_view key,
            std::string_view fallback = {});
    }

    class TC_GAME_API Manager
    {
    public:
        static Manager* instance();

        static constexpr char PREFIX[] = "AIO";
        static constexpr char DELIM = '\t';
        static constexpr char VERSION[] = "1.0.0";

        void RegisterHandler(std::string name, HandlerFn handler);
        void UnregisterHandler(std::string const& name);

        bool HandleIncoming(Player* player, std::string_view message);
        void Send(Player* player, std::string_view handler, std::string_view payload);
        void SendTable(Player* player, std::string_view handler,
            std::initializer_list<std::pair<std::string_view, std::string_view>> fields);

        std::vector<std::string> const& Capabilities() const { return _capabilities; }

    private:
        Manager();

        std::unordered_map<std::string, HandlerFn> _handlers;
        std::vector<std::string> _capabilities;
    };
}

#define sAIO AIO::Manager::instance()

#endif
