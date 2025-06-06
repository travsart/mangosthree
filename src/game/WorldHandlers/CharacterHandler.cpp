/**
 * MaNGOS is a full featured server for World of Warcraft, supporting
 * the following clients: 1.12.x, 2.4.3, 3.3.5a, 4.3.4a and 5.4.8
 *
 * Copyright (C) 2005-2025 MaNGOS <https://www.getmangos.eu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * World of Warcraft, and all World of Warcraft or Warcraft art, images,
 * and lore are copyrighted by Blizzard Entertainment, Inc.
 */

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "WorldPacket.h"
#include "SharedDefines.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "World.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "ObjectAccessor.h"
#include "Group.h"
#include "Database/DatabaseImpl.h"
#include "PlayerDump.h"
#include "SocialMgr.h"
#include "Util.h"
#include "Language.h"
#include "SpellMgr.h"
#include "Calendar.h"
#include "GameTime.h"
#include "Timer.h"
#ifdef ENABLE_ELUNA
#include "LuaEngine.h"
#endif /* ENABLE_ELUNA */

#include "playerbot/playerbot.h"
#include "playerbot/PlayerbotAIConfig.h"

// config option SkipCinematics supported values
enum CinematicsSkipMode
{
    CINEMATICS_SKIP_NONE      = 0,
    CINEMATICS_SKIP_SAME_RACE = 1,
    CINEMATICS_SKIP_ALL       = 2,
};


class PlayerbotLoginQueryHolder : public LoginQueryHolder
{
private:
    uint32 masterAccountId;
    PlayerbotHolder* playerbotHolder;

public:
    PlayerbotLoginQueryHolder(PlayerbotHolder* playerbotHolder, uint32 masterAccount, uint32 accountId, uint32 guid)
        : LoginQueryHolder(accountId, ObjectGuid(HIGHGUID_PLAYER, guid)), masterAccountId(masterAccount), playerbotHolder(playerbotHolder) { }

public:
    uint32 GetMasterAccountId() const { return masterAccountId; }
    PlayerbotHolder* GetPlayerbotHolder() { return playerbotHolder; }
};

void PlayerbotHolder::AddPlayerBot(uint32 playerGuid, uint32 masterAccount)
{
    // has bot already been added?
    ObjectGuid guid = ObjectGuid(HIGHGUID_PLAYER, playerGuid);
    Player* bot = sObjectMgr.GetPlayer(guid);

    if (bot && bot->IsInWorld())
        return;

    uint32 accountId = sObjectMgr.GetPlayerAccountIdByGUID(guid);
    if (accountId == 0)
        return;

    PlayerbotLoginQueryHolder *holder = new PlayerbotLoginQueryHolder(this, masterAccount, accountId, playerGuid);
    if (!holder->Initialize())
    {
        delete holder;                                      // delete all unprocessed queries
        return;
    }

    CharacterDatabase.DelayQueryHolder(this, &PlayerbotHolder::HandlePlayerBotLoginCallback, holder);
}

void PlayerbotHolder::HandlePlayerBotLoginCallback(QueryResult * dummy, SqlQueryHolder * holder)
{
    if (!holder)
        return;

    PlayerbotLoginQueryHolder* lqh = (PlayerbotLoginQueryHolder*)holder;
    uint32 masterAccount = lqh->GetMasterAccountId();

    WorldSession* masterSession = masterAccount ? sWorld.FindSession(masterAccount) : NULL;
    uint32 botAccountId = lqh->GetAccountId();
    WorldSession *botSession = new WorldSession(botAccountId, NULL, SEC_PLAYER, MAX_EXPANSION, 0, LOCALE_enUS, "", 0, 0, false);
    botSession->SetNoAnticheat();

    // has bot already been added?
    if (sObjectMgr.GetPlayer(lqh->GetGuid()))
        return;

    uint32 guid = lqh->GetGuid().GetRawValue();
    botSession->HandlePlayerLogin(lqh); // will delete lqh

    Player* bot = botSession->GetPlayer();
    if (!bot)
    {
        sLog.outError("Error logging in bot %d, please try to reset all random bots", guid);
        return;
    }

    bot->RemovePlayerbotMgr();
    sRandomPlayerbotMgr.OnPlayerLogin(bot);

    bool allowed = false;
    if (botAccountId == masterAccount)
        allowed = true;
    else if (masterSession && sPlayerbotAIConfig.allowGuildBots && bot->GetGuildId() == masterSession->GetPlayer()->GetGuildId())
        allowed = true;
    else if (sPlayerbotAIConfig.IsInRandomAccountList(botAccountId))
        allowed = true;

    if (allowed)
    {
        OnBotLogin(bot);
        return;
    }

    if (masterSession)
    {
        ChatHandler ch(masterSession);
        ch.PSendSysMessage("You are not allowed to control bot %s", bot->GetName());
    }
    LogoutPlayerBot(bot->GetObjectGuid());
    sLog.outError("Attempt to add not allowed bot %s, please try to reset all random bots", bot->GetName());
}

class LoginQueryHolder : public SqlQueryHolder
{
    private:
        uint32 m_accountId;
        ObjectGuid m_guid;
    public:
        LoginQueryHolder(uint32 accountId, ObjectGuid guid)
            : m_accountId(accountId), m_guid(guid) { }
        ObjectGuid GetGuid() const { return m_guid; }
        uint32 GetAccountId() const { return m_accountId; }
        bool Initialize();
};

bool LoginQueryHolder::Initialize()
{
    SetSize(MAX_PLAYER_LOGIN_QUERY);

    bool res = true;

    // NOTE: all fields in `characters` must be read to prevent lost character data at next save in case wrong DB structure.
    // !!! NOTE: including unused `zone`,`online`
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADFROM,            "SELECT `guid`, `account`, `name`, `race`, `class`, `gender`, `level`, `xp`, `money`, `playerBytes`, `playerBytes2`, `playerFlags`,"
                     "`position_x`, `position_y`, `position_z`, `map`, `orientation`, `taximask`, `cinematic`, `totaltime`, `leveltime`, `rest_bonus`, `logout_time`, `is_logout_resting`, `resettalents_cost`,"
                     "`resettalents_time`, `primary_trees`, `trans_x`, `trans_y`, `trans_z`, `trans_o`, `transguid`, `extra_flags`, `stable_slots`, `at_login`, `zone`, `online`, `death_expire_time`, `taxi_path`, `dungeon_difficulty`,"
                     "`totalKills`, `todayKills`, `yesterdayKills`, `chosenTitle`, `watchedFaction`, `drunk`,"
                     "`health`, `power1`, `power2`, `power3`, `power4`, `power5`, `specCount`, `activeSpec`, `exploredZones`, `equipmentCache`, `knownTitles`, `actionBars`, `slot`, `createdDate` FROM `characters` WHERE `guid` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADGROUP,           "SELECT `groupId` FROM group_member WHERE `memberGuid` ='%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADBOUNDINSTANCES,  "SELECT `id`, `permanent`, `map`, `difficulty`, `resettime` FROM `character_instance` LEFT JOIN `instance` ON `instance` = `id` WHERE `guid` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADAURAS,           "SELECT `caster_guid`,`item_guid`,`spell`,`stackcount`,`remaincharges`,`basepoints0`,`basepoints1`,`basepoints2`,`periodictime0`,`periodictime1`,`periodictime2`,`maxduration`,`remaintime`,`effIndexMask` FROM `character_aura` WHERE `guid` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADSPELLS,          "SELECT `spell`,`active`,`disabled` FROM `character_spell` WHERE `guid` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADQUESTSTATUS,     "SELECT `quest`,`status`,`rewarded`,`explored`,`timer`,`mobcount1`,`mobcount2`,`mobcount3`,`mobcount4`,`itemcount1`,`itemcount2`,`itemcount3`,`itemcount4`,`itemcount5`,`itemcount6` FROM `character_queststatus` WHERE `guid` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADDAILYQUESTSTATUS, "SELECT `quest` FROM `character_queststatus_daily` WHERE `guid` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADWEEKLYQUESTSTATUS, "SELECT `quest` FROM `character_queststatus_weekly` WHERE `guid` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADMONTHLYQUESTSTATUS, "SELECT `quest` FROM `character_queststatus_monthly` WHERE `guid` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADREPUTATION,      "SELECT `faction`,`standing`,`flags` FROM `character_reputation` WHERE `guid` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADINVENTORY,       "SELECT `data`,`text`,`bag`,`slot`,`item`,`item_template` FROM `character_inventory` JOIN `item_instance` ON `character_inventory`.`item` = `item_instance`.`guid` WHERE `character_inventory`.`guid` = '%u' ORDER BY `bag`,`slot`", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADITEMLOOT,        "SELECT `guid`,`itemid`,`amount`,`suffix`,`property` FROM `item_loot` WHERE `owner_guid` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADACTIONS,         "SELECT `spec`,`button`,`action`,`type` FROM `character_action` WHERE `guid` = '%u' ORDER BY `button`", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADSOCIALLIST,      "SELECT `friend`,`flags`,`note` FROM `character_social` WHERE `guid` = '%u' LIMIT 255", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADHOMEBIND,        "SELECT `map`,`zone`,`position_x`,`position_y`,`position_z` FROM `character_homebind` WHERE `guid` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADSPELLCOOLDOWNS,  "SELECT `spell`,`item`,`time` FROM `character_spell_cooldown` WHERE `guid` = '%u'", m_guid.GetCounter());
    if (sWorld.getConfig(CONFIG_BOOL_DECLINED_NAMES_USED))
    {
        res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADDECLINEDNAMES,   "SELECT `genitive`, `dative`, `accusative`, `instrumental`, `prepositional` FROM `character_declinedname` WHERE `guid` = '%u'", m_guid.GetCounter());
    }
    // in other case still be dummy query
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADGUILD,           "SELECT `guildid`,`rank` FROM `guild_member` WHERE `guid` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADARENAINFO,       "SELECT `arenateamid`, `played_week`, `played_season`, `wons_season`, `personal_rating` FROM `arena_team_member` WHERE `guid`='%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADACHIEVEMENTS,    "SELECT `achievement`, `date` FROM `character_achievement` WHERE `guid` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADCRITERIAPROGRESS, "SELECT `criteria`, `counter`, `date` FROM `character_achievement_progress` WHERE `guid` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADEQUIPMENTSETS,   "SELECT `setguid`, `setindex`, `name`, `iconname`, `ignore_mask`, `item0`, `item1`, `item2`, `item3`, `item4`, `item5`, `item6`, `item7`, `item8`, `item9`, `item10`, `item11`, `item12`, `item13`, `item14`, `item15`, `item16`, `item17`, `item18` FROM `character_equipmentsets` WHERE `guid` = '%u' ORDER BY setindex", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADBGDATA,          "SELECT `instance_id`, `team`, `join_x`, `join_y`, `join_z`, `join_o`, `join_map`, `taxi_start`, `taxi_end`, `mount_spell` FROM `character_battleground_data` WHERE `guid` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADACCOUNTDATA,     "SELECT `type`, `time`, `data` FROM `character_account_data` WHERE `guid`='%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADTALENTS,         "SELECT `talent_id`, `current_rank`, `spec` FROM `character_talent` WHERE `guid` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADSKILLS,          "SELECT `skill`, `value`, `max` FROM `character_skills` WHERE `guid` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADGLYPHS,          "SELECT `spec`, `slot`, `glyph` FROM `character_glyphs` WHERE `guid`='%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADMAILS,           "SELECT `id`,`messageType`,`sender`,`receiver`,`subject`,`body`,`expire_time`,`deliver_time`,`money`,`cod`,`checked`,`stationery`,`mailTemplateId`,`has_items` FROM `mail` WHERE `receiver` = '%u' ORDER BY `id` DESC", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADMAILEDITEMS,     "SELECT `data`, `text`, `mail_id`, `item_guid`, `item_template` FROM `mail_items` JOIN `item_instance` ON `item_guid` = `guid` WHERE `receiver` = '%u'", m_guid.GetCounter());
    res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADCURRENCIES,      "SELECT `id`, `totalCount`, `weekCount`, `seasonCount`, `flags` FROM `character_currencies` WHERE `guid` = '%u'", m_guid.GetCounter());

    return res;
}

// don't call WorldSession directly
// it may get deleted before the query callbacks get executed
// instead pass an account id to this handler
class CharacterHandler
{
    public:
        void HandleCharEnumCallback(QueryResult* result, uint32 account)
        {
            WorldSession* session = sWorld.FindSession(account);
            if (!session)
            {
                delete result;
                return;
            }
            session->HandleCharEnum(result);
        }
        void HandlePlayerLoginCallback(QueryResult * /*dummy*/, SqlQueryHolder* holder)
        {
            if (!holder)
            {
                return;
            }

            WorldSession* session = sWorld.FindSession(((LoginQueryHolder*)holder)->GetAccountId());
            if (!session)
            {
                delete holder;
                return;
            }
            ObjectGuid guid = ((LoginQueryHolder*)holder)->GetGuid();
            session->HandlePlayerLogin((LoginQueryHolder*)holder);
            Player* player = sObjectMgr.GetPlayer(guid, true);
            if (player)
            {
                player->CreatePlayerbotMgr();
                player->GetPlayerbotMgr()->OnPlayerLogin(player);
                sRandomPlayerbotMgr.OnPlayerLogin(player);
            }
        }
} chrHandler;

void WorldSession::HandleCharEnum(QueryResult* result)
{
    WorldPacket data(SMSG_CHAR_ENUM, 270);

    ByteBuffer buffer;

    data.WriteBits(0, 23);
    data.WriteBit(1);
    data.WriteBits(result ? result->GetRowCount() : 0, 17);

    if (result)
    {
        do
        {
            sLog.outDetail("Loading char guid %u from account %u.", (*result)[0].GetUInt32(), GetAccountId());

            if (!Player::BuildEnumData(result, &data, &buffer))
            {
                sLog.outError("Building enum data for SMSG_CHAR_ENUM has failed, aborting");
                return;
            }
        }
        while (result->NextRow());

        data.FlushBits();
        data.append(buffer);
    }

    SendPacket(&data);
}

void WorldSession::HandleCharEnumOpcode(WorldPacket & /*recv_data*/)
{
    /// get all the data necessary for loading all characters (along with their pets) on the account
    CharacterDatabase.AsyncPQuery(&chrHandler, &CharacterHandler::HandleCharEnumCallback, GetAccountId(),
                                  !sWorld.getConfig(CONFIG_BOOL_DECLINED_NAMES_USED) ?
                                  //   ------- Query Without Declined Names --------
                                  //           0               1                2                3                 4                  5                       6                        7
                                  "SELECT `characters`.`guid`, `characters`.`name`, `characters`.`race`, `characters`.`class`, `characters`.`gender`, `characters`.`playerBytes`, `characters`.`playerBytes2`, `characters`.`level`, "
                                  //   8                9               10                     11                     12                     13                    14
                                  "`characters`.`zone`, `characters`.`map`, `characters`.`position_x`, `characters`.`position_y`, `characters`.`position_z`, `guild_member`.`guildid`, `characters`.`playerFlags`, "
                                  //             15                          16                       17                         18                    19                             20
                                  "`characters`.`at_login`, `character_pet`.`entry`, `character_pet`.`modelid`, `character_pet`.`level`, `characters`.`equipmentCache`, `characters`.`slot` "
                                  "FROM `characters` LEFT JOIN `character_pet` ON `characters`.`guid`=`character_pet`.`owner` AND `character_pet`.`slot`='%u' "
                                  "LEFT JOIN `guild_member` ON `characters`.`guid` = `guild_member`.`guid` "
                                  "WHERE `characters`.`account` = '%u' ORDER BY `characters`.`guid`"
                                  :
                                  //   --------- Query With Declined Names ---------
                                  //                    0                    1                    2                    3                     4                      5                           6                            7
                                  "SELECT `characters`.`guid`, `characters`.`name`, `characters`.`race`, `characters`.`class`, `characters`.`gender`, `characters`.`playerBytes`, `characters`.`playerBytes2`, `characters`.`level`, "
                                  //             8                    9                   10                         11                         12                           13                      14
                                  "`characters`.`zone`, `characters`.`map`, `characters`.`position_x`, `characters`.`position_y`, `characters`.`position_z`, `guild_member`.`guildid`, `characters`.`playerFlags`, "
                                  //             15                          16                       17                         18                    19                             20                               21
                                  "`characters`.`at_login`, `character_pet`.`entry`, `character_pet`.`modelid`, `character_pet`.`level`, `characters`.`equipmentCache`, `characters`.`slot`, `character_declinedname`.`genitive` "
                                  "FROM `characters` LEFT JOIN `character_pet` ON `characters`.`guid` = `character_pet`.`owner` AND `character_pet`.`slot`='%u' "
                                  "LEFT JOIN `character_declinedname` ON `characters`.`guid` = `character_declinedname`.`guid` "
                                  "LEFT JOIN `guild_member` ON `characters`.`guid` = `guild_member`.`guid` "
                                  "WHERE `characters`.`account` = '%u' ORDER BY `characters`.`guid`",
                                  PET_SAVE_AS_CURRENT, GetAccountId());
}

void WorldSession::HandleCharCreateOpcode(WorldPacket& recv_data)
{
    std::string name;
    uint8 race_, class_;

    recv_data >> name;

    recv_data >> race_;
    recv_data >> class_;

    // extract other data required for player creating
    uint8 gender, skin, face, hairStyle, hairColor, facialHair, outfitId;
    recv_data >> gender >> skin >> face;
    recv_data >> hairStyle >> hairColor >> facialHair >> outfitId;

    WorldPacket data(SMSG_CHAR_CREATE, 1);                  // returned with diff.values in all cases

    if (GetSecurity() == SEC_PLAYER)
    {
        if (uint32 mask = sWorld.getConfig(CONFIG_UINT32_CHARACTERS_CREATING_DISABLED))
        {
            bool disabled = false;

            Team team = Player::TeamForRace(race_);
            switch (team)
            {
                case ALLIANCE: disabled = mask & (1 << 0); break;
                case HORDE:    disabled = mask & (1 << 1); break;
                default: break;
            }

            if (disabled)
            {
                data << (uint8)CHAR_CREATE_DISABLED;
                SendPacket(&data);
                return;
            }
        }
    }

    ChrClassesEntry const* classEntry = sChrClassesStore.LookupEntry(class_);
    ChrRacesEntry const* raceEntry = sChrRacesStore.LookupEntry(race_);

    if (!classEntry || !raceEntry)
    {
        data << (uint8)CHAR_CREATE_FAILED;
        SendPacket(&data);
        sLog.outError("Class: %u or Race %u not found in DBC (Wrong DBC files?) or Cheater?", class_, race_);
        return;
    }

    // prevent character creating Expansion race without Expansion account
    if (raceEntry->expansion > Expansion())
    {
        data << (uint8)CHAR_CREATE_EXPANSION;
        sLog.outError("Expansion %u account:[%d] tried to Create character with expansion %u race (%u)", Expansion(), GetAccountId(), raceEntry->expansion, race_);
        SendPacket(&data);
        return;
    }

    // prevent character creating Expansion class without Expansion account
    if (classEntry->expansion > Expansion())
    {
        data << (uint8)CHAR_CREATE_EXPANSION_CLASS;
        sLog.outError("Expansion %u account:[%d] tried to Create character with expansion %u class (%u)", Expansion(), GetAccountId(), classEntry->expansion, class_);
        SendPacket(&data);
        return;
    }

    // prevent character creating with invalid name
    if (!normalizePlayerName(name))
    {
        data << (uint8)CHAR_NAME_NO_NAME;
        SendPacket(&data);
        sLog.outError("Account:[%d] but tried to Create character with empty [name]", GetAccountId());
        return;
    }

    // check name limitations
    uint8 res = ObjectMgr::CheckPlayerName(name, true);
    if (res != CHAR_NAME_SUCCESS)
    {
        data << uint8(res);
        SendPacket(&data);
        return;
    }

    if (GetSecurity() == SEC_PLAYER && sObjectMgr.IsReservedName(name))
    {
        data << (uint8)CHAR_NAME_RESERVED;
        SendPacket(&data);
        return;
    }

    if (sObjectMgr.GetPlayerGuidByName(name))
    {
        data << (uint8)CHAR_CREATE_NAME_IN_USE;
        SendPacket(&data);
        return;
    }

    QueryResult* resultacct = LoginDatabase.PQuery("SELECT SUM(`numchars`) FROM `realmcharacters` WHERE `acctid` = '%u'", GetAccountId());
    if (resultacct)
    {
        Field* fields = resultacct->Fetch();
        uint32 acctcharcount = fields[0].GetUInt32();
        delete resultacct;

        if (acctcharcount >= sWorld.getConfig(CONFIG_UINT32_CHARACTERS_PER_ACCOUNT))
        {
            data << (uint8)CHAR_CREATE_ACCOUNT_LIMIT;
            SendPacket(&data);
            return;
        }
    }

    QueryResult* result = CharacterDatabase.PQuery("SELECT COUNT(`guid`) FROM `characters` WHERE `account` = '%u'", GetAccountId());
    uint8 charcount = 0;
    if (result)
    {
        Field* fields = result->Fetch();
        charcount = fields[0].GetUInt8();
        delete result;

        if (charcount >= sWorld.getConfig(CONFIG_UINT32_CHARACTERS_PER_REALM))
        {
            data << (uint8)CHAR_CREATE_SERVER_LIMIT;
            SendPacket(&data);
            return;
        }
    }

    // speedup check for heroic class disabled case
    uint32 heroic_free_slots = sWorld.getConfig(CONFIG_UINT32_HEROIC_CHARACTERS_PER_REALM);
    if (heroic_free_slots == 0 && GetSecurity() == SEC_PLAYER && class_ == CLASS_DEATH_KNIGHT)
    {
        data << (uint8)CHAR_CREATE_UNIQUE_CLASS_LIMIT;
        SendPacket(&data);
        return;
    }

    // speedup check for heroic class disabled case
    uint32 req_level_for_heroic = sWorld.getConfig(CONFIG_UINT32_MIN_LEVEL_FOR_HEROIC_CHARACTER_CREATING);
    if (GetSecurity() == SEC_PLAYER && class_ == CLASS_DEATH_KNIGHT && req_level_for_heroic > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
    {
        data << (uint8)CHAR_CREATE_LEVEL_REQUIREMENT;
        SendPacket(&data);
        return;
    }

    bool AllowTwoSideAccounts = sWorld.getConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_ACCOUNTS) || GetSecurity() > SEC_PLAYER;
    CinematicsSkipMode skipCinematics = CinematicsSkipMode(sWorld.getConfig(CONFIG_UINT32_SKIP_CINEMATICS));

    bool have_same_race = false;

    // if 0 then allowed creating without any characters
    bool have_req_level_for_heroic = (req_level_for_heroic == 0);

    if (!AllowTwoSideAccounts || skipCinematics == CINEMATICS_SKIP_SAME_RACE || class_ == CLASS_DEATH_KNIGHT)
    {
        QueryResult* result2 = CharacterDatabase.PQuery("SELECT `level`,`race`,`class` FROM `characters` WHERE `account` = '%u' %s",
                               GetAccountId(), (skipCinematics == CINEMATICS_SKIP_SAME_RACE || class_ == CLASS_DEATH_KNIGHT) ? "" : "LIMIT 1");
        if (result2)
        {
            Team team_ = Player::TeamForRace(race_);

            Field* field = result2->Fetch();
            uint8 acc_race  = field[1].GetUInt32();

            if (GetSecurity() == SEC_PLAYER && class_ == CLASS_DEATH_KNIGHT)
            {
                uint8 acc_class = field[2].GetUInt32();
                if (acc_class == CLASS_DEATH_KNIGHT)
                {
                    if (heroic_free_slots > 0)
                    {
                        --heroic_free_slots;
                    }

                    if (heroic_free_slots == 0)
                    {
                        data << (uint8)CHAR_CREATE_UNIQUE_CLASS_LIMIT;
                        SendPacket(&data);
                        delete result2;
                        return;
                    }
                }

                if (!have_req_level_for_heroic)
                {
                    uint32 acc_level = field[0].GetUInt32();
                    if (acc_level >= req_level_for_heroic)
                    {
                        have_req_level_for_heroic = true;
                    }
                }
            }

            // need to check team only for first character
            // TODO: what to if account already has characters of both races?
            if (!AllowTwoSideAccounts)
            {
                if (acc_race == 0 || Player::TeamForRace(acc_race) != team_)
                {
                    data << (uint8)CHAR_CREATE_PVP_TEAMS_VIOLATION;
                    SendPacket(&data);
                    delete result2;
                    return;
                }
            }

            // search same race for cinematic or same class if need
            // TODO: check if cinematic already shown? (already logged in?; cinematic field)
            while ((skipCinematics == CINEMATICS_SKIP_SAME_RACE && !have_same_race) || class_ == CLASS_DEATH_KNIGHT)
            {
                if (!result2->NextRow())
                {
                    break;
                }

                field = result2->Fetch();
                acc_race = field[1].GetUInt32();

                if (!have_same_race)
                {
                    have_same_race = race_ == acc_race;
                }

                if (GetSecurity() == SEC_PLAYER && class_ == CLASS_DEATH_KNIGHT)
                {
                    uint8 acc_class = field[2].GetUInt32();
                    if (acc_class == CLASS_DEATH_KNIGHT)
                    {
                        if (heroic_free_slots > 0)
                        {
                            --heroic_free_slots;
                        }

                        if (heroic_free_slots == 0)
                        {
                            data << (uint8)CHAR_CREATE_UNIQUE_CLASS_LIMIT;
                            SendPacket(&data);
                            delete result2;
                            return;
                        }
                    }

                    if (!have_req_level_for_heroic)
                    {
                        uint32 acc_level = field[0].GetUInt32();
                        if (acc_level >= req_level_for_heroic)
                        {
                            have_req_level_for_heroic = true;
                        }
                    }
                }
            }
            delete result2;
        }
    }

    if (GetSecurity() == SEC_PLAYER && class_ == CLASS_DEATH_KNIGHT && !have_req_level_for_heroic)
    {
        data << (uint8)CHAR_CREATE_LEVEL_REQUIREMENT;
        SendPacket(&data);
        return;
    }

    Player* pNewChar = new Player(this);
    // Sets the createdTime of the character which is UNIX timestamp
    uint32 createdDate = GetUnixTimeStamp(); // Unix Timestamp in seconds
    pNewChar->SetCreatedDate(createdDate); // TODO get currentTimeStamp for createdTime

    if (!pNewChar->Create(sObjectMgr.GeneratePlayerLowGuid(), name, race_, class_, gender, skin, face, hairStyle, hairColor, facialHair, outfitId))
    {
        // Player not create (race/class problem?)
        delete pNewChar;

        data << (uint8)CHAR_CREATE_ERROR;
        SendPacket(&data);

        return;
    }

    if ((have_same_race && skipCinematics == CINEMATICS_SKIP_SAME_RACE) || skipCinematics == CINEMATICS_SKIP_ALL)
    {
        pNewChar->setCinematic(1);                           // not show intro
    }

    pNewChar->SetAtLoginFlag(AT_LOGIN_FIRST);               // First login

    // Player created, save it now
    pNewChar->SaveToDB();
    charcount += 1;

    LoginDatabase.PExecute("DELETE FROM `realmcharacters` WHERE `acctid`= '%u' AND `realmid`= '%u'", GetAccountId(), realmID);
    LoginDatabase.PExecute("INSERT INTO `realmcharacters` (`numchars`, `acctid`, `realmid`) VALUES (%u, %u, %u)",  charcount, GetAccountId(), realmID);
    uint32 pet_id = 1;
    if (CharacterDatabase.PQuery("SELECT id FROM character_pet ORDER BY id DESC LIMIT 1"))
    {
        Field* fields = CharacterDatabase.PQuery("SELECT id FROM character_pet ORDER BY id DESC LIMIT 1")->Fetch();
        pet_id = fields[0].GetUInt32();
        pet_id += 1;
    }
    //else
        //pet_id = 1;

    if (class_ == CLASS_WARLOCK)
    {
        // Imp
        CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `resettalents_cost`, `resettalents_time`, `abdata`) VALUES (%u, 416, %u, 4449, 0, 0, 1, 0, 0, ' ', 1, 100, 282, 72, 1295721046, 0, 0, '7 2 7 1 7 0 129 3110 1 0 1 0 1 0 6 2 6 1 6 0 ')", pet_id, pNewChar->GetGUIDLow());
        //CharacterDatabase.PExecute("UPDATE characters SET currentPetSlot = '100', petSlotUsed = '3452816845' WHERE guid = %u", pNewChar->GetGUIDLow());
        pNewChar->SetTemporaryUnsummonedPetNumber(pet_id);
    }
    if (class_ == CLASS_HUNTER)
    {
        switch(race_)
        {
        case RACE_HUMAN: // Wolf
            CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `resettalents_cost`, `resettalents_time`, `abdata`) VALUES (%u, 42717, %u, 903, 13481, 1, 1, 0, 0, ' ', 0, 0, 192, 0, 1295727347, 0, 0, '7 2 7 1 7 0 129 2649 129 17253 1 0 1 0 6 2 6 1 6 0 ')", pet_id, pNewChar->GetGUIDLow());
            break;
        case RACE_DWARF: // Bear
            CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `resettalents_cost`, `resettalents_time`, `abdata`) VALUES (%u, 42713, %u, 822, 13481, 1, 1, 0, 0, ' ', 0, 0, 212, 0, 1295727650, 0, 0, '7 2 7 1 7 0 129 2649 129 16827 1 0 1 0 6 2 6 1 6 0 ')", pet_id, pNewChar->GetGUIDLow());
            break;
        case RACE_ORC: // Boar
            CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `resettalents_cost`, `resettalents_time`, `abdata`) VALUES (%u, 42719, %u, 744, 13481, 1, 1, 0, 0, ' ', 0, 0, 212, 0, 1295727175, 0, 0, '7 2 7 1 7 0 129 2649 129 17253 1 0 1 0 6 2 6 1 6 0 ')", pet_id, pNewChar->GetGUIDLow());
            break;
        case RACE_NIGHTELF: // Cat
            CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `resettalents_cost`, `resettalents_time`, `abdata`) VALUES (%u, 42718, %u, 17090, 13481, 1, 1, 0, 0, ' ', 0, 0, 192, 0, 1295727501, 0, 0, '7 2 7 1 7 0 129 2649 129 16827 1 0 1 0 6 2 6 1 6 0 ')", pet_id, pNewChar->GetGUIDLow());
            break;
        case RACE_UNDEAD: // Spider
            CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `resettalents_cost`, `resettalents_time`, `abdata`) VALUES (%u, 51107, %u, 368, 13481, 1, 1, 0, 0, ' ', 0, 0, 202, 0, 1295727821, 0, 0, '7 2 7 1 7 0 129 2649 129 17253 1 0 1 0 6 2 6 1 6 0 ')", pet_id, pNewChar->GetGUIDLow());
            break;
        case RACE_TAUREN: // Tallstrider
            CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `resettalents_cost`, `resettalents_time`, `abdata`) VALUES (%u, 42720, %u, 29057, 13481, 1, 1, 0, 0, ' ', 0, 0, 192, 0, 1295727912, 0, 0, '7 2 7 1 7 0 129 2649 129 16827 1 0 1 0 6 2 6 1 6 0 ')", pet_id, pNewChar->GetGUIDLow());
            break;
        case RACE_TROLL: // Raptor
            CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `resettalents_cost`, `resettalents_time`, `abdata`) VALUES (%u, 42721, %u, 23518, 13481, 1, 1, 0, 0, ' ', 0, 0, 192, 0, 1295727987, 0, 0, '7 2 7 1 7 0 129 2649 129 50498 129 16827 1 0 6 2 6 1 6 0 ')", pet_id, pNewChar->GetGUIDLow());
            break;
        case RACE_GOBLIN: // Crab
            CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `resettalents_cost`, `resettalents_time`, `abdata`) VALUES (%u, 42715, %u, 27692, 13481, 1, 1, 0, 0, ' ', 0, 0, 212, 0, 1295720595, 0, 0, '7 2 7 1 7 0 129 2649 129 16827 1 0 1 0 6 2 6 1 6 0 ')", pet_id, pNewChar->GetGUIDLow());
            break;
        case RACE_BLOODELF: // Dragonhawk
            CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `resettalents_cost`, `resettalents_time`, `abdata`) VALUES (%u, 42710, %u, 23515, 13481, 1, 1, 0, 0, ' ', 0, 0, 202, 0, 1295728068, 0, 0, '7 2 7 1 7 0 129 2649 129 17253 1 0 1 0 6 2 6 1 6 0 ')", pet_id, pNewChar->GetGUIDLow());
            break;
        case RACE_DRAENEI: // Moth
            CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `resettalents_cost`, `resettalents_time`, `abdata`) VALUES (%u, 42712, %u, 29056, 13481, 1, 1, 0, 0, ' ', 0, 0, 192, 0, 1295728128, 0, 0, '7 2 7 1 7 0 129 2649 129 49966 1 0 1 0 6 2 6 1 6 0 ')", pet_id, pNewChar->GetGUIDLow());
            break;
        case RACE_WORGEN: // Dog
            CharacterDatabase.PExecute("REPLACE INTO character_pet (`id`, `entry`, `owner`, `modelid`, `CreatedBySpell`, `PetType`, `level`, `exp`, `Reactstate`, `name`, `renamed`, `slot`, `curhealth`, `curmana`, `savetime`, `resettalents_cost`, `resettalents_time`, `abdata`) VALUES (%u, 42722, %u, 30221, 13481, 1, 1, 0, 0, ' ', 0, 0, 192, 0, 1295728219, 0, 0, '7 2 7 1 7 0 129 2649 129 17253 1 0 1 0 6 2 6 1 6 0 ')", pet_id, pNewChar->GetGUIDLow());
            break;
        }
        //CharacterDatabase.PExecute("UPDATE characters SET currentPetSlot = '0', petSlotUsed = '1' WHERE guid = %u", pNewChar->GetGUIDLow());
        pNewChar->SetTemporaryUnsummonedPetNumber(pet_id);
    }

    pNewChar->CleanupsBeforeDelete();
    data << (uint8)CHAR_CREATE_SUCCESS;
    SendPacket(&data);

    std::string IP_str = GetRemoteAddress();
    BASIC_LOG("Account: %d (IP: %s) Create Character:[%s] (guid: %u)", GetAccountId(), IP_str.c_str(), name.c_str(), pNewChar->GetGUIDLow());
    sLog.outChar("Account: %d (IP: %s) Create Character:[%s] (guid: %u)", GetAccountId(), IP_str.c_str(), name.c_str(), pNewChar->GetGUIDLow());

    // Used by Eluna
#ifdef ENABLE_ELUNA
    if (Eluna* e = sWorld.GetEluna())
    {
        e->OnCreate(pNewChar);
    }
#endif /* ENABLE_ELUNA */

    delete pNewChar;                                        // created only to call SaveToDB()
}

void WorldSession::HandleCharDeleteOpcode(WorldPacket& recv_data)
{
    ObjectGuid guid;
    recv_data >> guid;

    // can't delete loaded character
    if (sObjectMgr.GetPlayer(guid))
    {
        return;
    }

    uint32 accountId = 0;
    std::string name;

    // is guild leader
    if (sGuildMgr.GetGuildByLeader(guid))
    {
        WorldPacket data(SMSG_CHAR_DELETE, 1);
        data << (uint8)CHAR_DELETE_FAILED_GUILD_LEADER;
        SendPacket(&data);
        return;
    }

    // is arena team captain
    if (sObjectMgr.GetArenaTeamByCaptain(guid))
    {
        WorldPacket data(SMSG_CHAR_DELETE, 1);
        data << (uint8)CHAR_DELETE_FAILED_ARENA_CAPTAIN;
        SendPacket(&data);
        return;
    }

    uint32 lowguid = guid.GetCounter();

    QueryResult* result = CharacterDatabase.PQuery("SELECT `account`,`name` FROM `characters` WHERE `guid`='%u'", lowguid);
    if (result)
    {
        Field* fields = result->Fetch();
        accountId = fields[0].GetUInt32();
        name = fields[1].GetCppString();
        delete result;
    }

    // prevent deleting other players' characters using cheating tools
    if (accountId != GetAccountId())
    {
        return;
    }

    std::string IP_str = GetRemoteAddress();
    BASIC_LOG("Account: %d (IP: %s) Delete Character:[%s] (guid: %u)", GetAccountId(), IP_str.c_str(), name.c_str(), lowguid);
    sLog.outChar("Account: %d (IP: %s) Delete Character:[%s] (guid: %u)", GetAccountId(), IP_str.c_str(), name.c_str(), lowguid);

    // Used by Eluna
#ifdef ENABLE_ELUNA
    if (Eluna* e = sWorld.GetEluna())
    {
        e->OnDelete(lowguid);
    }
#endif /* ENABLE_ELUNA */

    if (sLog.IsOutCharDump())                               // optimize GetPlayerDump call
    {
        std::string dump = PlayerDumpWriter().GetDump(lowguid);
        sLog.outCharDump(dump.c_str(), GetAccountId(), lowguid, name.c_str());
    }

    sCalendarMgr.RemovePlayerCalendar(guid);

    Player::DeleteFromDB(guid, GetAccountId());

    WorldPacket data(SMSG_CHAR_DELETE, 1);
    data << (uint8)CHAR_DELETE_SUCCESS;
    SendPacket(&data);
}

void WorldSession::HandlePlayerLoginOpcode(WorldPacket& recv_data)
{
    if (PlayerLoading() || GetPlayer() != NULL)
    {
        sLog.outError("Player tryes to login again, AccountId = %d", GetAccountId());
        return;
    }

    m_playerLoading = true;

    ObjectGuid playerGuid;
    Player* pCurrChar = nullptr;
    WorldPacket data(SMSG_CHARACTER_LOGIN_FAILED, 1);

    recv_data.ReadGuidMask<2, 3, 0, 6, 4, 5, 1, 7>(playerGuid);
    recv_data.ReadGuidBytes<2, 7, 0, 3, 5, 6, 1, 4>(playerGuid);
    pCurrChar = ObjectAccessor::FindPlayer(playerGuid, false);

    DEBUG_LOG("WORLD: Received opcode Player Logon Message from %s", playerGuid.GetString().c_str());


    if (pCurrChar && pCurrChar->GetPlayerbotAI())
    {
        WorldSession* botSession = pCurrChar->GetSession();
        SetPlayer(pCurrChar, playerGuid);
        _player->SetSession(this);
        _logoutTime = time(0);

        m_sessionDbcLocale = botSession->m_sessionDbcLocale;
        m_sessionDbLocaleIndex = botSession->m_sessionDbLocaleIndex;

        PlayerbotMgr* mgr = _player->GetPlayerbotMgr();
        if (!mgr || mgr->GetMaster() != _player)
        {
            _player->RemovePlayerbotMgr();
            _player->CreatePlayerbotMgr();
            _player->GetPlayerbotMgr()->OnPlayerLogin(_player);

            if (sRandomPlayerbotMgr.GetPlayerBot(playerGuid))

                sRandomPlayerbotMgr.MovePlayerBot(playerGuid, _player->GetPlayerbotMgr());
            else
                _player->GetPlayerbotMgr()->OnBotLogin(_player);
        }
    }

    if (_player)
    {
        // player is reconnecting

        if (!pCurrChar)
        {
            sLog.outError("HandlePlayerLoginOpcode> %s try to login a second char, AccountId = %u", _player->GetGuidStr().c_str(), GetAccountId());
            data << (uint8)CHAR_LOGIN_DUPLICATE_CHARACTER;
            SendPacket(data);
            return;
        }

        if (!isLogingOut())
        {
            sLog.outError("HandlePlayerLoginOpcode> %s try to login again, AccountId = %u", pCurrChar->GetGuidStr().c_str(), GetAccountId());
            data << (uint8)CHAR_LOGIN_FAILED;
            SendPacket(data);
            return;
        }

        if (!_player->IsInWorld())
            // finish pending transfers before starting the logout
            while (_player && _player->IsBeingTeleportedFar())
                HandleMoveWorldportAckOpcode();

        // release loot on reconnect
        if (Loot* loot = sLootMgr.GetLoot(_player))
            loot->Release(_player);

        HandlePlayerReconnect();
        return;
    }

    LoginQueryHolder* holder = new LoginQueryHolder(GetAccountId(), playerGuid);
    if (!holder->Initialize())
    {
        delete holder;                                      // delete all unprocessed queries
        m_playerLoading = false;
        return;
    }

    CharacterDatabase.DelayQueryHolder(&chrHandler, &CharacterHandler::HandlePlayerLoginCallback, holder);
}

void WorldSession::HandlePlayerLogin(LoginQueryHolder* holder)
{
    /* Store the player's GUID for later reference */
    ObjectGuid playerGuid = holder->GetGuid();

    /* Create a new instance of the player object */
    Player* pCurrChar = new Player(this);

    /* Initialize a motion generator */
    pCurrChar->GetMotionMaster()->Initialize();

    /* Account ID is validated in LoadFromDB (prevents cheaters logging in to characters not on their account) */
    if (!pCurrChar->LoadFromDB(playerGuid, holder))         /// Could not load character from database, cancel login
    {
        /* Disconnect the game client */
        KickPlayer();

        /* Remove references to avoid dangling pointers */
        delete pCurrChar;
        delete holder;

        /* Checked in WorldSession::Update */
        m_playerLoading = false;

        return;
    }

    /* Validation check completely, assign player to WorldSession::_player for later use */
    SetPlayer(pCurrChar);
    pCurrChar->SendDungeonDifficulty(false);

    WorldPacket data(SMSG_LOGIN_VERIFY_WORLD, 20);
    data << pCurrChar->GetMapId();
    data << pCurrChar->GetPositionX();
    data << pCurrChar->GetPositionY();
    data << pCurrChar->GetPositionZ();
    data << pCurrChar->GetOrientation();
    SendPacket(&data);

    // load player specific part before send times
    LoadAccountData(holder->GetResult(PLAYER_LOGIN_QUERY_LOADACCOUNTDATA), PER_CHARACTER_CACHE_MASK);
    SendAccountDataTimes(PER_CHARACTER_CACHE_MASK);

    data.Initialize(SMSG_FEATURE_SYSTEM_STATUS, 34);        // added in 2.2.0
    data << uint8(2);                                       // status
    data << uint32(1);                                      // Scrolls of Ressurection?
    data << uint32(1);
    data << uint32(2);
    data << uint32(0);
    data.WriteBit(true);
    data.WriteBit(true);
    data.WriteBit(false);
    data.WriteBit(true);
    data.WriteBit(false);
    data.WriteBit(false);                                   // enable(1)/disable(0) voice chat interface in client
    data << uint32(1);
    data << uint32(0);
    data << uint32(10);
    data << uint32(60);
    SendPacket(&data);

    // Send MOTD
    {
        data.Initialize(SMSG_MOTD, 50);                     // new in 2.0.1
        data << (uint32)0;

        uint32 linecount = 0;
        /* The MOTD itself */
        std::string str_motd = sWorld.GetMotd();
        /* Used for tracking our position within the MOTD while iterating through it */
        std::string::size_type pos = 0, nextpos;

        /* Find the next occurance of @ in the string
         * This is how newlines are represented */
        while ((nextpos = str_motd.find('@', pos)) != std::string::npos)
        {
            /* If these are not equal, it means a '@' was found
             * These are used to represent newlines in the string
             * It is set by the code above here */
            if (nextpos != pos)
            {
                /* Send the player a system message containing the substring from pos to nextpos - pos */
                data << str_motd.substr(pos, nextpos - pos);
                ++linecount;
            }
            pos = nextpos + 1;
        }
        /* There are no more newlines in our MOTD, so we send whatever is left */
        if (pos < str_motd.length())
        {
            data << str_motd.substr(pos);
            ++linecount;
        }

        data.put(0, linecount);

        SendPacket(&data);
        DEBUG_LOG("WORLD: Sent motd (SMSG_MOTD)");
    }

    // QueryResult *result = CharacterDatabase.PQuery("SELECT guildid,rank FROM guild_member WHERE guid = '%u'",pCurrChar->GetGUIDLow());
    QueryResult* resultGuild = holder->GetResult(PLAYER_LOGIN_QUERY_LOADGUILD);

    if (resultGuild)
    {
        /* We're in a guild, so set the player's guild data to represent that */
        Field* fields = resultGuild->Fetch();
        pCurrChar->SetInGuild(fields[0].GetUInt32());
        pCurrChar->SetRank(fields[1].GetUInt32());
        /* Avoid dangling pointers */
        delete resultGuild;
    }
    /* Player thinks they have a guild, but it isn't in the database. Clear that information */
    else if (pCurrChar->GetGuildId())
    {
        pCurrChar->SetInGuild(0);
        pCurrChar->SetGuildLevel(0);
        pCurrChar->SetRank(0);
    }

    /* Player is in a guild
     * TODO: Can we move this code into the block above? Not sure why it's down here */
    if (pCurrChar->GetGuildId() != 0)
    {
        /* Get guild based on what we set the player's guild to above */
        Guild* guild = sGuildMgr.GetGuildById(pCurrChar->GetGuildId());

        /* More checks to see if they're in a guild? I'm sure this is redundant */
        if (guild)
        {
            pCurrChar->SetGuildLevel(guild->GetLevel());
            /* Build MOTD packet and send it to the player */
            data.Initialize(SMSG_GUILD_EVENT, (1 + 1 + guild->GetMOTD().size() + 1));
            data << uint8(GE_MOTD);
            data << uint8(1);
            data << guild->GetMOTD();
            SendPacket(&data);
            DEBUG_LOG("WORLD: Sent guild-motd (SMSG_GUILD_EVENT)");

            guild->DisplayGuildBankTabsInfo(this);
            /* Let everyone in the guild know you've just signed in */
            guild->BroadcastEvent(GE_SIGNED_ON, pCurrChar->GetObjectGuid(), pCurrChar->GetName());
        }
        /* If the player is not in a guild */
        else
        {
            // remove wrong guild data
            sLog.outError("Player %s (GUID: %u) marked as member of nonexistent guild (id: %u), removing guild membership for player.", pCurrChar->GetName(), pCurrChar->GetGUIDLow(), pCurrChar->GetGuildId());
            pCurrChar->SetInGuild(0);
            pCurrChar->SetGuildLevel(0);
        }
    }

    data.Initialize(SMSG_LEARNED_DANCE_MOVES, 4 + 4);
    data << uint64(0);
    SendPacket(&data);

    pCurrChar->SendInitialPacketsBeforeAddToMap();

    // Show cinematic at the first time that player login (TODO: activate world grids first, then cinematic)
    // move this code past the "SendInitialPacketsAfterAddToMap();" line?
    if (!pCurrChar->getCinematic())
    {
        pCurrChar->setCinematic(1);

        if (ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(pCurrChar->getClass()))
        {
            if (cEntry->CinematicSequence)
            {
                pCurrChar->SendCinematicStart(cEntry->CinematicSequence);
            }
            else if (ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(pCurrChar->getRace()))
            {
                pCurrChar->SendCinematicStart(rEntry->CinematicSequence);
            }
        }
    }

    uint32 miscRequirement = 0;
    AreaLockStatus lockStatus = AREA_LOCKSTATUS_OK;
    if (AreaTrigger const* at = sObjectMgr.GetMapEntranceTrigger(pCurrChar->GetMapId()))
    {
        lockStatus = pCurrChar->GetAreaTriggerLockStatus(at, pCurrChar->GetDifficulty(pCurrChar->GetMap()->IsRaid()), miscRequirement);
    }
    else
    {
        // Some basic checks in case of a map without areatrigger
        MapEntry const* mapEntry = sMapStore.LookupEntry(pCurrChar->GetMapId());
        if (!mapEntry)
        {
            lockStatus = AREA_LOCKSTATUS_UNKNOWN_ERROR;
        }
        else if (pCurrChar->GetSession()->Expansion() < mapEntry->Expansion())
        {
            lockStatus = AREA_LOCKSTATUS_INSUFFICIENT_EXPANSION;
        }
    }

    /* This code is run if we can not add the player to the map for some reason */
    if (lockStatus != AREA_LOCKSTATUS_OK || !pCurrChar->GetMap()->Add(pCurrChar))
    {
        /* Attempt to find an areatrigger to teleport the player for us */
        AreaTrigger const* at = sObjectMgr.GetGoBackTrigger(pCurrChar->GetMapId());
        if (at)
        {
            lockStatus = pCurrChar->GetAreaTriggerLockStatus(at, pCurrChar->GetDifficulty(pCurrChar->GetMap()->IsRaid()), miscRequirement);
        }

        /* We couldn't find an areatrigger to teleport, so just move the player back to their home bind */
        if (!at || lockStatus != AREA_LOCKSTATUS_OK || !pCurrChar->TeleportTo(at->target_mapId, at->target_X, at->target_Y, at->target_Z, pCurrChar->GetOrientation()))
        {
            pCurrChar->TeleportToHomebind();
        }
    }

    sObjectAccessor.AddObject(pCurrChar);
    // DEBUG_LOG("Player %s added to Map.",pCurrChar->GetName());

    pCurrChar->SendInitialPacketsAfterAddToMap();

    /* Mark player as online in the database */
    static SqlStatementID updChars;
    static SqlStatementID updAccount;

    SqlStatement stmt = CharacterDatabase.CreateStatement(updChars, "UPDATE `characters` SET `online` = 1 WHERE `guid` = ?");
    stmt.PExecute(pCurrChar->GetGUIDLow());

    stmt = LoginDatabase.CreateStatement(updAccount, "UPDATE `account` SET `active_realm_id` = ? WHERE `id` = ?");
    stmt.PExecute(realmID, GetAccountId());

    /* Sync player's in-game time with server time */
    pCurrChar->SetInGameTime(GameTime::GetGameTimeMS());

    /* Send logon notification to player's group
     * This is sent after player is added to the world so that player receives it too */
    if (Group* group = pCurrChar->GetGroup())
    {
        group->SendUpdate();
    }

    /* Inform player's friends that player has come online */
    sSocialMgr.SendFriendStatus(pCurrChar, FRIEND_ONLINE, pCurrChar->GetObjectGuid(), true);

    /* Load the player's corpse if it exists, or resurrect the player if not */
    pCurrChar->LoadCorpse();

    /* If the player is dead, we need to set them as a ghost and increase movespeed */
    if (pCurrChar->m_deathState != ALIVE)
    {
        /* If player is a night elf, wisp racial should be applied */
        if (pCurrChar->getRace() == RACE_NIGHTELF)
        {
            pCurrChar->CastSpell(pCurrChar, 20584, true);   // auras SPELL_AURA_INCREASE_SPEED(+speed in wisp form), SPELL_AURA_INCREASE_SWIM_SPEED(+swim speed in wisp form), SPELL_AURA_TRANSFORM (to wisp form)
        }

        /* Apply ghost spell to player */
        pCurrChar->CastSpell(pCurrChar, 8326, true);        // auras SPELL_AURA_GHOST, SPELL_AURA_INCREASE_SPEED(why?), SPELL_AURA_INCREASE_SWIM_SPEED(why?)

        /* Allow player to walk on water */
        pCurrChar->SetWaterWalk(true);
    }

    /* If player is on a taxi, continue their flight */
    pCurrChar->ContinueTaxiFlight();

    // reset for all pets before pet loading
    if (pCurrChar->HasAtLoginFlag(AT_LOGIN_RESET_PET_TALENTS))
    {
        Pet::resetTalentsForAllPetsOf(pCurrChar);
    }

    // Load pet if any (if player not alive and in taxi flight or another then pet will remember as temporary unsummoned)
    pCurrChar->LoadPet();

    /* If we're running an FFA PvP realm and the player isn't a GM, mark them as PvP flagged */
    if (sWorld.IsFFAPvPRealm() && !pCurrChar->isGameMaster() && !pCurrChar->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_RESTING))
    {
        pCurrChar->SetFFAPvP(true);
    }

    if (pCurrChar->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP))
    {
        pCurrChar->SetContestedPvP();
    }

    /* Apply onLogon requests (such as talent resets) */
    if (pCurrChar->HasAtLoginFlag(AT_LOGIN_RESET_SPELLS))
    {
        pCurrChar->resetSpells();
        SendNotification(LANG_RESET_SPELLS);
    }

    if (pCurrChar->HasAtLoginFlag(AT_LOGIN_RESET_TALENTS))
    {
        pCurrChar->resetTalents(true, true);
        pCurrChar->SendTalentsInfoData(false);              // original talents send already in to SendInitialPacketsBeforeAddToMap, resend reset state
        SendNotification(LANG_RESET_TALENTS);               // we can use SMSG_TALENTS_INVOLUNTARILY_RESET here
    }

    // Used by Eluna
#ifdef ENABLE_ELUNA
    if (Eluna* e = pCurrChar->GetEluna())
    {
        if (pCurrChar->HasAtLoginFlag(AT_LOGIN_FIRST))
        {
            e->OnFirstLogin(pCurrChar);
        }
    }
#endif /* ENABLE_ELUNA */


    /* We've done what we need to, remove the flag */
    if (pCurrChar->HasAtLoginFlag(AT_LOGIN_FIRST))
    {
        pCurrChar->RemoveAtLoginFlag(AT_LOGIN_FIRST);
    }

    if (!sPlayerbotAIConfig.IsInRandomAccountList(GetAccountId()))
    {
        // add collector to all accounts if enabled
        if (sWorld.getConfig(CONFIG_BOOL_COLLECTORS_EDITION) && !HasAccountFlag(ACCOUNT_FLAG_COLLECTOR_CLASSIC | ACCOUNT_FLAG_COLLECTOR_TBC | ACCOUNT_FLAG_COLLECTOR_WRATH))
        {
            AddAccountFlag(ACCOUNT_FLAG_COLLECTOR_CLASSIC | ACCOUNT_FLAG_COLLECTOR_TBC | ACCOUNT_FLAG_COLLECTOR_WRATH);
            LoginDatabase.PExecute("UPDATE account SET flags = flags | 0x%x WHERE id = %u", (ACCOUNT_FLAG_COLLECTOR_CLASSIC | ACCOUNT_FLAG_COLLECTOR_TBC | ACCOUNT_FLAG_COLLECTOR_WRATH), GetAccountId());
        }
    }

    // create collector's edition reward (tbc)
    if (HasAccountFlag(ACCOUNT_FLAG_COLLECTOR_TBC) && !(pCurrChar->HasItemCount(25535, 1, true) || pCurrChar->HasSpell(32298)))
    {
        bool hasPetReward = false;
        // check if already has in mail
        for (PlayerMails::iterator itr = _player->GetMailBegin(); itr != _player->GetMailEnd(); ++itr)
        {
            // skip deleted mails
            if ((*itr)->state == MAIL_STATE_DELETED)
                continue;

            uint8 item_count = uint8((*itr)->items.size());
            for (uint8 i = 0; i < item_count; ++i)
            {
                Item* item = _player->GetMItem((*itr)->items[i].item_guid);
                if (item->GetEntry() == 25535)
                {
                    hasPetReward = true;
                    break;
                }
            }
        }

        if (!hasPetReward)
        {
            std::ostringstream body;
            body << "Hello, " << pCurrChar->GetName() << ",\n\n";
            body << "Welcome to the World of Warcraft!\n\n";
            body << "As special thanks for purchasing the World of Warcraft: The Burning Crusade Collector's Edition we send you a gift: a little companion to join you on your quest for adventure and glory.\n\n";
            body << "Thanks again, and enjoy your stay in the World of Warcraft!";

            MailDraft draft;
            draft.SetSubjectAndBody("Collector's Edition Gift", body.str());

            Item* gift = Item::CreateItem(25535, 1, nullptr);
            gift->SaveToDB();
            draft.AddItem(gift);

            MailSender sender(MAIL_NORMAL, (uint32)0, MAIL_STATIONERY_GM);
            draft.SendMailTo(MailReceiver(pCurrChar, pCurrChar->GetObjectGuid()), sender);
        }
    }

    // create collector's edition reward (tbc)
    if (HasAccountFlag(ACCOUNT_FLAG_COLLECTOR_WRATH) && !(pCurrChar->HasItemCount(39286, 1, true) || pCurrChar->HasSpell(52615)))
    {
        bool hasPetReward = false;
        // check if already has in mail
        for (PlayerMails::iterator itr = _player->GetMailBegin(); itr != _player->GetMailEnd(); ++itr)
        {
            // skip deleted mails
            if ((*itr)->state == MAIL_STATE_DELETED)
                continue;

            uint8 item_count = uint8((*itr)->items.size());
            for (uint8 i = 0; i < item_count; ++i)
            {
                Item* item = _player->GetMItem((*itr)->items[i].item_guid);
                if (item->GetEntry() == 39286)
                {
                    hasPetReward = true;
                    break;
                }
            }
        }

        if (!hasPetReward)
        {
            std::ostringstream body;
            body << "Hello, " << pCurrChar->GetName() << ",\n\n";
            body << "Welcome to the World of Warcraft!\n\n";
            body << "As special thanks for purchasing the World of Warcraft: Wrath of the Lich King Collector's Edition we send you a gift: a little companion to join you on your quest for adventure and glory.\n\n";
            body << "Thanks again, and enjoy your stay in the World of Warcraft!";

            MailDraft draft;
            draft.SetSubjectAndBody("Collector's Edition Gift", body.str());

            Item* gift = Item::CreateItem(39286, 1, nullptr);
            gift->SaveToDB();
            draft.AddItem(gift);

            MailSender sender(MAIL_NORMAL, (uint32)0, MAIL_STATIONERY_GM);
            draft.SendMailTo(MailReceiver(pCurrChar, pCurrChar->GetObjectGuid()), sender);
        }
    }

    // create collector's edition reward (vanilla)
    if (HasAccountFlag(ACCOUNT_FLAG_COLLECTOR_CLASSIC))
    {
        uint32 itemid = 0;
        uint32 questid = 0;
        switch (pCurrChar->getRace())
        {
        case RACE_HUMAN:
            itemid = 14646;
            questid = 5805;
            break;
        case RACE_ORC:
        case RACE_TROLL:
            itemid = 14649;
            questid = 5843;
            break;
        case RACE_DWARF:
        case RACE_GNOME:
            itemid = 14647;
            questid = 5841;
            break;
        case RACE_NIGHTELF:
            itemid = 14648;
            questid = 5842;
            break;
        case RACE_UNDEAD:
            itemid = 14651;
            questid = 5847;
            break;
        case RACE_TAUREN:
            itemid = 14650;
            questid = 5844;
            break;
        case RACE_DRAENEI:
            itemid = 22888;
            questid = 9278;
            break;
        case RACE_BLOODELF:
            itemid = 20938;
            questid = 8547;
            break;
        }

        if (itemid && questid)
        {
            if (!pCurrChar->HasQuest(questid) && !pCurrChar->HasItemCount(itemid, 1, true) && !pCurrChar->GetQuestRewardStatus(questid))
            {
                bool hasPetReward = false;
                // check if already has in mail
                for (PlayerMails::iterator itr = _player->GetMailBegin(); itr != _player->GetMailEnd(); ++itr)
                {
                    // skip deleted mails
                    if ((*itr)->state == MAIL_STATE_DELETED)
                        continue;

                    uint8 item_count = uint8((*itr)->items.size());
                    for (uint8 i = 0; i < item_count; ++i)
                    {
                        Item* item = _player->GetMItem((*itr)->items[i].item_guid);
                        if (item->GetEntry() == itemid)
                        {
                            hasPetReward = true;
                            break;
                        }
                    }
                }

                ItemPrototype const* pProto = ObjectMgr::GetItemPrototype(itemid);
                if (pProto && !hasPetReward)
                {
                    uint32 noSpaceForCount = 0;
                    ItemPosCountVec dest;
                    uint8 msg = pCurrChar->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemid, 1, &noSpaceForCount);
                    if (msg != EQUIP_ERR_OK)
                    {
                        std::ostringstream body;
                        body << "Hello, " << pCurrChar->GetName() << ",\n\n";
                        body << "Welcome to the World of Warcraft!\n\n";
                        body << "As special thanks for purchasing the World of Warcraft Collector's Edition we send you a gift: a little companion to join you on your quest for adventure and glory.\n\n";
                        body << "Thanks again, and enjoy your stay in the World of Warcraft!";

                        MailDraft draft;
                        draft.SetSubjectAndBody("Collector's Edition Gift", body.str());

                        Item* gift = Item::CreateItem(itemid, 1, nullptr);
                        gift->SaveToDB();
                        draft.AddItem(gift);

                        MailSender sender(MAIL_NORMAL, (uint32)0, MAIL_STATIONERY_GM);
                        draft.SendMailTo(MailReceiver(pCurrChar, pCurrChar->GetObjectGuid()), sender);
                    }
                    else
                        Item* item = pCurrChar->StoreNewItem(dest, itemid, true);
                }
            }
        }
    }

    /* If the server is shutting down, show shutdown time remaining */
    if (sWorld.IsShutdowning())
    {
        sWorld.ShutdownMsg(true, pCurrChar);
    }

    /* If player should have all taxi paths, give them to the player */
    if (sWorld.getConfig(CONFIG_BOOL_ALL_TAXI_PATHS))
    {
        pCurrChar->SetTaxiCheater(true);
    }

    /* Send GM notifications */
    if (pCurrChar->isGameMaster())
    {
        SendNotification(LANG_GM_ON);
    }

    if (!pCurrChar->isGMVisible())
    {
        SendNotification(LANG_INVISIBLE_INVISIBLE);
        SpellEntry const* invisibleAuraInfo = sSpellStore.LookupEntry(sWorld.getConfig(CONFIG_UINT32_GM_INVISIBLE_AURA));
        if (invisibleAuraInfo && IsSpellAppliesAura(invisibleAuraInfo))
        {
            pCurrChar->CastSpell(pCurrChar, invisibleAuraInfo, true);
        }
    }

    std::string IP_str = GetRemoteAddress();
    sLog.outChar("Account: %d (IP: %s) Login Character:[%s] (guid: %u)",
                 GetAccountId(), IP_str.c_str(), pCurrChar->GetName(), pCurrChar->GetGUIDLow());

    /* Make player stand up if they're not already stood up and not stunned */
    if (!pCurrChar->IsStandState() && !pCurrChar->hasUnitState(UNIT_STAT_STUNNED))
    {
        pCurrChar->SetStandState(UNIT_STAND_STATE_STAND);
    }

    m_playerLoading = false;

    // Used by Eluna
#ifdef ENABLE_ELUNA
    if (Eluna* e = sWorld.GetEluna())
    {
        e->OnLogin(pCurrChar);
    }
#endif /* ENABLE_ELUNA */

    bool SoloCraftEnable = sWorld.getConfig(CONFIG_BOOL_SOLOCRAFT_ENABLED);
    bool SoloCraftAnnounceModule = sWorld.getConfig(CONFIG_BOOL_SOLOCRAFT_ANNOUNCE);

    if (SoloCraftEnable)
    {
        if (SoloCraftAnnounceModule)
        {
            ChatHandler(pCurrChar->GetSession()).SendSysMessage("This server is running |cff4CFF00SPP SoloCraft Custom |rmodule.");
        }
    }

    // Handle Login-Achievements (should be handled after loading)
    pCurrChar->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ON_LOGIN, 1);

    delete holder;
}

void WorldSession::HandleSetFactionAtWarOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Received opcode CMSG_SET_FACTION_ATWAR");

    uint32 repListID;
    uint8  flag;

    recv_data >> repListID;
    recv_data >> flag;

    GetPlayer()->GetReputationMgr().SetAtWar(repListID, flag);
}

void WorldSession::HandleTutorialFlagOpcode(WorldPacket& recv_data)
{
    uint32 iFlag;
    recv_data >> iFlag;

    uint32 wInt = (iFlag / 32);
    if (wInt >= 8)
    {
        // sLog.outError("CHEATER? Account:[%d] Guid[%u] tried to send wrong CMSG_TUTORIAL_FLAG", GetAccountId(),GetGUID());
        return;
    }
    uint32 rInt = (iFlag % 32);

    uint32 tutflag = GetTutorialInt(wInt);
    tutflag |= (1 << rInt);
    SetTutorialInt(wInt, tutflag);

    // DEBUG_LOG("Received Tutorial Flag Set {%u}.", iFlag);
}

void WorldSession::HandleTutorialClearOpcode(WorldPacket & /*recv_data*/)
{
    for (int i = 0; i < 8; ++i)
    {
        SetTutorialInt(i, 0xFFFFFFFF);
    }
}

void WorldSession::HandleTutorialResetOpcode(WorldPacket & /*recv_data*/)
{
    for (int i = 0; i < 8; ++i)
    {
        SetTutorialInt(i, 0x00000000);
    }
}

void WorldSession::HandleSetWatchedFactionOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Received opcode CMSG_SET_WATCHED_FACTION");
    int32 repId;
    recv_data >> repId;
    GetPlayer()->SetInt32Value(PLAYER_FIELD_WATCHED_FACTION_INDEX, repId);
}

void WorldSession::HandleSetFactionInactiveOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Received opcode CMSG_SET_FACTION_INACTIVE");
    uint32 replistid;
    uint8 inactive;
    recv_data >> replistid >> inactive;

    _player->GetReputationMgr().SetInactive(replistid, inactive);
}

void WorldSession::HandleShowingHelmOpcode(WorldPacket & /*recv_data*/)
{
    DEBUG_LOG("CMSG_SHOWING_HELM for %s", _player->GetName());
    _player->ToggleFlag(PLAYER_FLAGS, PLAYER_FLAGS_HIDE_HELM);
}

void WorldSession::HandleShowingCloakOpcode(WorldPacket & /*recv_data*/)
{
    DEBUG_LOG("CMSG_SHOWING_CLOAK for %s", _player->GetName());
    _player->ToggleFlag(PLAYER_FLAGS, PLAYER_FLAGS_HIDE_CLOAK);
}

void WorldSession::HandleCharRenameOpcode(WorldPacket& recv_data)
{
    ObjectGuid guid;
    std::string newname;

    recv_data >> guid;
    recv_data >> newname;

    // prevent character rename to invalid name
    if (!normalizePlayerName(newname))
    {
        WorldPacket data(SMSG_CHAR_RENAME, 1);
        data << uint8(CHAR_NAME_NO_NAME);
        SendPacket(&data);
        return;
    }

    uint8 res = ObjectMgr::CheckPlayerName(newname, true);
    if (res != CHAR_NAME_SUCCESS)
    {
        WorldPacket data(SMSG_CHAR_RENAME, 1);
        data << uint8(res);
        SendPacket(&data);
        return;
    }

    // check name limitations
    if (GetSecurity() == SEC_PLAYER && sObjectMgr.IsReservedName(newname))
    {
        WorldPacket data(SMSG_CHAR_RENAME, 1);
        data << uint8(CHAR_NAME_RESERVED);
        SendPacket(&data);
        return;
    }

    std::string escaped_newname = newname;
    CharacterDatabase.escape_string(escaped_newname);

    // make sure that the character belongs to the current account, that rename at login is enabled
    // and that there is no character with the desired new name
    CharacterDatabase.AsyncPQuery(&WorldSession::HandleChangePlayerNameOpcodeCallBack,
                                  GetAccountId(), newname,
                                  "SELECT `guid`, `name` FROM `characters` WHERE `guid` = %u AND `account` = %u AND (`at_login` & %u) = %u AND NOT EXISTS (SELECT NULL FROM `characters` WHERE `name` = '%s')",
                                  guid.GetCounter(), GetAccountId(), AT_LOGIN_RENAME, AT_LOGIN_RENAME, escaped_newname.c_str()
                                 );
}

void WorldSession::HandleChangePlayerNameOpcodeCallBack(QueryResult* result, uint32 accountId, std::string newname)
{
    WorldSession* session = sWorld.FindSession(accountId);
    if (!session)
    {
        if (result)
        {
            delete result;
        }
        return;
    }

    if (!result)
    {
        WorldPacket data(SMSG_CHAR_RENAME, 1);
        data << uint8(CHAR_CREATE_ERROR);
        session->SendPacket(&data);
        return;
    }

    uint32 guidLow = result->Fetch()[0].GetUInt32();
    ObjectGuid guid = ObjectGuid(HIGHGUID_PLAYER, guidLow);
    std::string oldname = result->Fetch()[1].GetCppString();

    delete result;

    CharacterDatabase.BeginTransaction();
    CharacterDatabase.PExecute("UPDATE `characters` SET `name` = '%s', `at_login` = `at_login` & ~ %u WHERE `guid` ='%u'", newname.c_str(), uint32(AT_LOGIN_RENAME), guidLow);
    CharacterDatabase.PExecute("DELETE FROM `character_declinedname` WHERE `guid` ='%u'", guidLow);
    CharacterDatabase.CommitTransaction();

    sLog.outChar("Account: %d (IP: %s) Character:[%s] (guid:%u) Changed name to: %s", session->GetAccountId(), session->GetRemoteAddress().c_str(), oldname.c_str(), guidLow, newname.c_str());

    WorldPacket data(SMSG_CHAR_RENAME, 1 + 8 + (newname.size() + 1));
    data << uint8(RESPONSE_SUCCESS);
    data << guid;
    data << newname;
    session->SendPacket(&data);

    sWorld.InvalidatePlayerDataToAllClient(guid);
}

void WorldSession::HandleSetPlayerDeclinedNamesOpcode(WorldPacket& recv_data)
{
    ObjectGuid guid;

    recv_data >> guid;

    // not accept declined names for unsupported languages
    std::string name;
    if (!sObjectMgr.GetPlayerNameByGUID(guid, name))
    {
        WorldPacket data(SMSG_SET_PLAYER_DECLINED_NAMES_RESULT, 4 + 8);
        data << uint32(1);
        SendPacket(&data);
        return;
    }

    std::wstring wname;
    if (!Utf8toWStr(name, wname))
    {
        WorldPacket data(SMSG_SET_PLAYER_DECLINED_NAMES_RESULT, 4 + 8);
        data << uint32(1);
        SendPacket(&data);
        return;
    }

    if (!isCyrillicCharacter(wname[0]))                     // name already stored as only single alphabet using
    {
        WorldPacket data(SMSG_SET_PLAYER_DECLINED_NAMES_RESULT, 4 + 8);
        data << uint32(1);
        SendPacket(&data);
        return;
    }

    std::string name2;
    DeclinedName declinedname;

    recv_data >> name2;

    if (name2 != name)                                      // character have different name
    {
        WorldPacket data(SMSG_SET_PLAYER_DECLINED_NAMES_RESULT, 4 + 8);
        data << uint32(1);
        SendPacket(&data);
        return;
    }

    for (int i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
    {
        recv_data >> declinedname.name[i];
        if (!normalizePlayerName(declinedname.name[i]))
        {
            WorldPacket data(SMSG_SET_PLAYER_DECLINED_NAMES_RESULT, 4 + 8);
            data << uint32(1);
            SendPacket(&data);
            return;
        }
    }

    if (!ObjectMgr::CheckDeclinedNames(GetMainPartOfName(wname, 0), declinedname))
    {
        WorldPacket data(SMSG_SET_PLAYER_DECLINED_NAMES_RESULT, 4 + 8);
        data << uint32(1);
        SendPacket(&data);
        return;
    }

    for (int i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
    {
        CharacterDatabase.escape_string(declinedname.name[i]);
    }

    CharacterDatabase.BeginTransaction();
    CharacterDatabase.PExecute("DELETE FROM `character_declinedname` WHERE `guid` = '%u'", guid.GetCounter());
    CharacterDatabase.PExecute("INSERT INTO `character_declinedname` (`guid`, `genitive`, `dative`, `accusative`, `instrumental`, `prepositional`) VALUES ('%u','%s','%s','%s','%s','%s')",
                               guid.GetCounter(), declinedname.name[0].c_str(), declinedname.name[1].c_str(), declinedname.name[2].c_str(), declinedname.name[3].c_str(), declinedname.name[4].c_str());
    CharacterDatabase.CommitTransaction();

    WorldPacket data(SMSG_SET_PLAYER_DECLINED_NAMES_RESULT, 4 + 8);
    data << uint32(0);                                      // OK
    SendPacket(&data);
}

void WorldSession::HandleAlterAppearanceOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_ALTER_APPEARANCE");

    uint32 Hair, Color, FacialHair, skinTone;
    recv_data >> Hair >> Color >> FacialHair >> skinTone;

    uint32 skinTone_id = -1;
    if (_player->getRace() == RACE_TAUREN)
    {
        BarberShopStyleEntry const* bs_skinTone = sBarberShopStyleStore.LookupEntry(skinTone);
        if (!bs_skinTone || bs_skinTone->type != 3 || bs_skinTone->race != _player->getRace() || bs_skinTone->gender != _player->getGender())
        {
            return;
        }
        skinTone_id = bs_skinTone->hair_id;
    }

    BarberShopStyleEntry const* bs_hair = sBarberShopStyleStore.LookupEntry(Hair);

    if (!bs_hair || bs_hair->type != 0 || bs_hair->race != _player->getRace() || bs_hair->gender != _player->getGender())
    {
        return;
    }

    BarberShopStyleEntry const* bs_facialHair = sBarberShopStyleStore.LookupEntry(FacialHair);

    if (!bs_facialHair || bs_facialHair->type != 2 || bs_facialHair->race != _player->getRace() || bs_facialHair->gender != _player->getGender())
    {
        return;
    }

    uint32 Cost = _player->GetBarberShopCost(bs_hair->hair_id, Color, bs_facialHair->hair_id, skinTone_id);

    // 0 - ok
    // 1,3 - not enough money
    // 2 - you have to seat on barber chair
    if (_player->GetMoney() < Cost)
    {
        WorldPacket data(SMSG_BARBER_SHOP_RESULT, 4);
        data << uint32(1);                                  // no money
        SendPacket(&data);
        return;
    }
    else
    {
        WorldPacket data(SMSG_BARBER_SHOP_RESULT, 4);
        data << uint32(0);                                  // ok
        SendPacket(&data);
    }

    _player->ModifyMoney(-int64(Cost));                     // it isn't free
    _player->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_AT_BARBER, Cost);

    _player->SetByteValue(PLAYER_BYTES, 2, uint8(bs_hair->hair_id));
    _player->SetByteValue(PLAYER_BYTES, 3, uint8(Color));
    _player->SetByteValue(PLAYER_BYTES_2, 0, uint8(bs_facialHair->hair_id));
    if (_player->getRace() == RACE_TAUREN)
    {
        _player->SetByteValue(PLAYER_BYTES, 0, uint8(skinTone_id));
    }

    _player->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_VISIT_BARBER_SHOP, 1);

    _player->SetStandState(0);                              // stand up
}

void WorldSession::HandleRemoveGlyphOpcode(WorldPacket& recv_data)
{
    uint32 slot;
    recv_data >> slot;

    if (slot >= MAX_GLYPH_SLOT_INDEX)
    {
        DEBUG_LOG("Client sent wrong glyph slot number in opcode CMSG_REMOVE_GLYPH %u", slot);
        return;
    }

    if (_player->GetGlyph(slot))
    {
        _player->ApplyGlyph(slot, false);
        _player->SetGlyph(slot, 0);
        _player->SendTalentsInfoData(false);
    }
}

void WorldSession::HandleCharCustomizeOpcode(WorldPacket& recv_data)
{
    ObjectGuid guid;
    std::string newname;

    recv_data >> guid;
    recv_data >> newname;

    uint8 gender, skin, face, hairStyle, hairColor, facialHair;
    recv_data >> gender >> skin >> hairColor >> hairStyle >> facialHair >> face;

    QueryResult* result = CharacterDatabase.PQuery("SELECT `at_login` FROM `characters` WHERE `guid` = '%u'", guid.GetCounter());
    if (!result)
    {
        WorldPacket data(SMSG_CHAR_CUSTOMIZE, 1);
        data << uint8(CHAR_CREATE_ERROR);
        SendPacket(&data);
        return;
    }

    Field* fields = result->Fetch();
    uint32 at_loginFlags = fields[0].GetUInt32();
    delete result;

    if (!(at_loginFlags & AT_LOGIN_CUSTOMIZE))
    {
        WorldPacket data(SMSG_CHAR_CUSTOMIZE, 1);
        data << uint8(CHAR_CREATE_ERROR);
        SendPacket(&data);
        return;
    }

    // prevent character rename to invalid name
    if (!normalizePlayerName(newname))
    {
        WorldPacket data(SMSG_CHAR_CUSTOMIZE, 1);
        data << uint8(CHAR_NAME_NO_NAME);
        SendPacket(&data);
        return;
    }

    uint8 res = ObjectMgr::CheckPlayerName(newname, true);
    if (res != CHAR_NAME_SUCCESS)
    {
        WorldPacket data(SMSG_CHAR_CUSTOMIZE, 1);
        data << uint8(res);
        SendPacket(&data);
        return;
    }

    // check name limitations
    if (GetSecurity() == SEC_PLAYER && sObjectMgr.IsReservedName(newname))
    {
        WorldPacket data(SMSG_CHAR_CUSTOMIZE, 1);
        data << uint8(CHAR_NAME_RESERVED);
        SendPacket(&data);
        return;
    }

    // character with this name already exist
    ObjectGuid newguid = sObjectMgr.GetPlayerGuidByName(newname);
    if (newguid && newguid != guid)
    {
        WorldPacket data(SMSG_CHAR_CUSTOMIZE, 1);
        data << uint8(CHAR_CREATE_NAME_IN_USE);
        SendPacket(&data);
        return;
    }

    CharacterDatabase.escape_string(newname);
    Player::Customize(guid, gender, skin, face, hairStyle, hairColor, facialHair);
    CharacterDatabase.PExecute("UPDATE `characters` SET `name` = '%s', `at_login` = `at_login` & ~ %u WHERE `guid` ='%u'", newname.c_str(), uint32(AT_LOGIN_CUSTOMIZE), guid.GetCounter());
    CharacterDatabase.PExecute("DELETE FROM `character_declinedname` WHERE `guid` ='%u'", guid.GetCounter());

    std::string IP_str = GetRemoteAddress();
    sLog.outChar("Account: %d (IP: %s), Character %s customized to: %s", GetAccountId(), IP_str.c_str(), guid.GetString().c_str(), newname.c_str());

    WorldPacket data(SMSG_CHAR_CUSTOMIZE, 1 + 8 + (newname.size() + 1) + 6);
    data << uint8(RESPONSE_SUCCESS);
    data << ObjectGuid(guid);
    data << newname;
    data << uint8(gender);
    data << uint8(skin);
    data << uint8(face);
    data << uint8(hairStyle);
    data << uint8(hairColor);
    data << uint8(facialHair);
    SendPacket(&data);

    sWorld.InvalidatePlayerDataToAllClient(guid);
}

void WorldSession::HandleEquipmentSetSaveOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_EQUIPMENT_SET_SAVE");

    ObjectGuid setGuid;
    uint32 index;
    std::string name;
    std::string iconName;

    recv_data >> setGuid.ReadAsPacked();
    recv_data >> index;
    recv_data >> name;
    recv_data >> iconName;

    if (index >= MAX_EQUIPMENT_SET_INDEX)                   // client set slots amount
    {
        return;
    }

    EquipmentSet eqSet;

    eqSet.Guid      = setGuid.GetRawValue();
    eqSet.Name      = name;
    eqSet.IconName  = iconName;
    eqSet.state     = EQUIPMENT_SET_NEW;

    for (uint32 i = 0; i < EQUIPMENT_SLOT_END; ++i)
    {
        ObjectGuid itemGuid;

        recv_data >> itemGuid.ReadAsPacked();

        // equipment manager sends "1" (as raw GUID) for slots set to "ignore" (not touch slot at equip set)
        if (itemGuid.GetRawValue() == 1)
        {
            // ignored slots saved as bit mask because we have no free special values for Items[i]
            eqSet.IgnoreMask |= 1 << i;
            continue;
        }

        Item* item = _player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);

        if (!item && itemGuid)                              // cheating check 1
        {
            return;
        }

        if (item && item->GetObjectGuid() != itemGuid)      // cheating check 2
        {
            return;
        }

        eqSet.Items[i] = itemGuid.GetCounter();
    }

    _player->SetEquipmentSet(index, eqSet);
}

void WorldSession::HandleEquipmentSetDeleteOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_EQUIPMENT_SET_DELETE");

    ObjectGuid setGuid;

    recv_data >> setGuid.ReadAsPacked();

    _player->DeleteEquipmentSet(setGuid.GetRawValue());
}

void WorldSession::HandleEquipmentSetUseOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_EQUIPMENT_SET_USE");
    recv_data.hexlike();

    for (uint32 i = 0; i < EQUIPMENT_SLOT_END; ++i)
    {
        ObjectGuid itemGuid;
        uint8 srcbag, srcslot;

        recv_data >> itemGuid.ReadAsPacked();
        recv_data >> srcbag >> srcslot;

        DEBUG_LOG("Item (%s): srcbag %u, srcslot %u", itemGuid.GetString().c_str(), srcbag, srcslot);

        // check if item slot is set to "ignored" (raw value == 1), must not be unequipped then
        if (itemGuid.GetRawValue() == 1)
        {
            continue;
        }

        Item* item = _player->GetItemByGuid(itemGuid);

        uint16 dstpos = i | (INVENTORY_SLOT_BAG_0 << 8);

        if (!item)
        {
            Item* uItem = _player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
            if (!uItem)
            {
                continue;
            }

            ItemPosCountVec sDest;
            InventoryResult msg = _player->CanStoreItem(NULL_BAG, NULL_SLOT, sDest, uItem, false);
            if (msg == EQUIP_ERR_OK)
            {
                _player->RemoveItem(INVENTORY_SLOT_BAG_0, i, true);
                _player->StoreItem(sDest, uItem, true);
            }
            else
            {
                _player->SendEquipError(msg, uItem, NULL);
            }

            continue;
        }

        if (item->GetPos() == dstpos)
        {
            continue;
        }

        _player->SwapItem(item->GetPos(), dstpos);
    }

    WorldPacket data(SMSG_USE_EQUIPMENT_SET_RESULT, 1);
    data << uint8(0);                                       // 4 - equipment swap failed - inventory is full
    SendPacket(&data);
}

void WorldSession::HandleReorderCharactersOpcode(WorldPacket& recv_data)
{
    uint32 charCount = recv_data.ReadBits(10);

    if (charCount > sWorld.getConfig(CONFIG_UINT32_CHARACTERS_PER_REALM))
    {
        DEBUG_LOG("SESSION: received CMSG_REORDER_CHARACTERS, but characters count %u is beyond server limit.", charCount);
        recv_data.rfinish();
        return;
    }

    std::vector<ObjectGuid> guids;
    std::vector<uint8> slots;

    for (uint32 i = 0; i < charCount; ++i)
    {
        ObjectGuid guid;
        recv_data.ReadGuidMask<1, 4, 5, 3, 0, 7, 6, 2>(guid);
        guids.push_back(guid);
    }

    for (uint32 i = 0; i < charCount; ++i)
    {
        recv_data.ReadGuidBytes<6, 5, 1, 4, 0, 3>(guids[i]);
        slots.push_back(recv_data.ReadUInt8());
        recv_data.ReadGuidBytes<2, 7>(guids[i]);
    }

    CharacterDatabase.BeginTransaction();
    for (uint32 i = 0; i < charCount; ++i)
        CharacterDatabase.PExecute("UPDATE `characters` SET `slot` = '%u' WHERE `guid` = '%u' AND `account` = '%u'",
        slots[i], guids[i].GetCounter(), GetAccountId());
    CharacterDatabase.CommitTransaction();
}

void WorldSession::HandleSetCurrencyFlagsOpcode(WorldPacket& recv_data)
{
    uint32 currencyId, flags;
    recv_data >> flags >> currencyId;

    DEBUG_LOG("CMSG_SET_CURRENCY_FLAGS: currency: %u, flags: %u", currencyId, flags);

    if (flags & ~PLAYERCURRENCY_MASK_USED_BY_CLIENT)
    {
        DEBUG_LOG("CMSG_SET_CURRENCY_FLAGS: received unknown currency flags 0x%X from player %s account %u for currency %u",
            flags & ~PLAYERCURRENCY_MASK_USED_BY_CLIENT, GetPlayer()->GetGuidStr().c_str(), GetAccountId(), currencyId);
    }

    flags &= PLAYERCURRENCY_MASK_USED_BY_CLIENT;
    GetPlayer()->SetCurrencyFlags(currencyId, uint8(flags));
}

void WorldSession::HandleLoadScreenOpcode(WorldPacket& recvPacket)
{
    DEBUG_LOG("CMSG_LOAD_SCREEN");
    uint32 mapID;

    recvPacket >> mapID;
    recvPacket.ReadBit();
}
