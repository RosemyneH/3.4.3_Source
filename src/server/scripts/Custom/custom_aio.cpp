#include "ScriptMgr.h"
#include "AIO.h"
#include "Player.h"
#include "RealmList.h"
#include "World.h"
#include <sstream>

namespace
{
    Realm const* LocalRealm()
    {
        Battlenet::RealmHandle handle(0, 0, 1);
        return sRealmList->GetRealm(handle);
    }

    std::string CapabilityList()
    {
        std::ostringstream oss;
        bool first = true;
        for (std::string const& cap : sAIO->Capabilities())
        {
            if (!first)
                oss << ',';
            first = false;
            oss << cap;
        }
        return oss.str();
    }

    void SendReady(Player* player)
    {
        std::string realmName = "Trinity";
        std::string build = "54261";
        if (Realm const* realm = LocalRealm())
        {
            realmName = realm->Name;
            build = std::to_string(realm->Build);
        }

        sAIO->SendTable(player, "core.ready", {
            { "version", AIO::Manager::VERSION },
            { "build", build },
            { "realm", realmName },
            { "portal", "127.0.0.1" },
            { "capabilities", CapabilityList() },
        });
    }

    void HandleCorePing(Player* player, std::string_view /*payload*/)
    {
        sAIO->SendTable(player, "core.pong", {
            { "uptime", std::to_string(sWorld->GetUptime()) },
        });
    }

    void HandleExamplePing(Player* player, std::string_view payload)
    {
        auto data = AIO::Codec::Decode(payload);
        std::string text = AIO::Codec::Get(data, "text", "");
        sAIO->SendTable(player, "example.echo", {
            { "text", "server heard: " + text },
        });
    }

    void HandleCoreHandshake(Player* player, std::string_view payload)
    {
        auto data = AIO::Codec::Decode(payload);
        std::string clientVersion = AIO::Codec::Get(data, "version", "unknown");

        sAIO->SendTable(player, "core.capabilities", {
            { "list", CapabilityList() },
        });

        sAIO->SendTable(player, "core.notify", {
            { "color", "00ff00" },
            { "text", "AIO linked (client " + clientVersion + ")" },
        });

        SendReady(player);

        std::string realmName = "Trinity";
        std::string port = std::to_string(sWorld->getIntConfig(CONFIG_PORT_WORLD));
        if (Realm const* realm = LocalRealm())
            realmName = realm->Name;

        sAIO->SendTable(player, "dev.localhost", {
            { "portal", "127.0.0.1" },
            { "realm", realmName },
            { "port", port },
        });
    }
}

class AIOWorldScript : public WorldScript
{
public:
    AIOWorldScript() : WorldScript("AIOWorldScript") { }

    void OnStartup() override
    {
        sAIO->RegisterHandler("core.ping", HandleCorePing);
        sAIO->RegisterHandler("core.handshake", HandleCoreHandshake);
        sAIO->RegisterHandler("example.ping", HandleExamplePing);
    }
};

class AIOPlayerScript : public PlayerScript
{
public:
    AIOPlayerScript() : PlayerScript("AIOPlayerScript") { }

    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        SendReady(player);
    }
};

void AddSC_CustomAIO()
{
    new AIOWorldScript();
    new AIOPlayerScript();
}
