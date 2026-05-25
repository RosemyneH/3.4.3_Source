#include "ScriptMgr.h"
#include "CollectionMgr.h"
#include "DatabaseEnv.h"

namespace
{
    constexpr uint32 JOYOUS_JOURNEYS = 377749;

    void StripJoyousJourneys(Player* player)
    {
        if (!player)
            return;

        player->RemoveAura(JOYOUS_JOURNEYS);
        if (player->HasSpell(JOYOUS_JOURNEYS))
            player->RemoveSpell(JOYOUS_JOURNEYS, false, false, true);

        CharacterDatabase.DirectPExecute(
            "DELETE FROM character_spell WHERE guid = {} AND spell = {}",
            player->GetGUID().GetCounter(), JOYOUS_JOURNEYS);
        CharacterDatabase.DirectPExecute(
            "DELETE FROM character_aura WHERE guid = {} AND spell = {}",
            player->GetGUID().GetCounter(), JOYOUS_JOURNEYS);
    }

    void StripHeirlooms(Player* player)
    {
        if (!player || !player->GetSession())
            return;

        player->GetSession()->GetCollectionMgr()->ClearHeirlooms();
    }

    void ProcessAlphaItems(Player* player)
    {
        if (!player->HasItemCount(1100, 1))
        {
            player->AddItem(1100, 4);
            player->ModifyMoney(100000);
        }
    }

    void ProcessUnclaimedBpayItems(const WorldSession* session)
    {
        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_BATTLE_PAY_PURCHASES);
        stmt->setUInt32(0, session->GetAccountId());

        if (PreparedQueryResult result = LoginDatabase.Query(stmt))
        {
            do
            {
                const auto fields = result->Fetch();

                const uint32 id        = fields[0].GetUInt32();
                const uint32 productId = fields[1].GetUInt32();

                Battlepay::Purchase purchase;
                purchase.ProductID  = productId;
                purchase.PurchaseID = id;

                session->GetBattlePayMgr()->ProcessDelivery(purchase, true);

            } while (result->NextRow());
        }
    }
}

class CustomWrathionPlayer : public PlayerScript
{
public:
    CustomWrathionPlayer()
        : PlayerScript("CustomWrathionPlayer") { }

    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        const WorldSession* session = player->GetSession();

        StripJoyousJourneys(player);
        StripHeirlooms(player);
        ProcessUnclaimedBpayItems(session);
        ProcessAlphaItems(player);
    }

    void OnMapChanged(Player* player) override
    {
        StripJoyousJourneys(player);
        StripHeirlooms(player);
    }
};

void AddSC_CustomWrathion()
{
    new CustomWrathionPlayer();
}
