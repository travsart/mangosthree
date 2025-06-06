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

/// \addtogroup u2w
/// @{
/// \file

#ifndef MANGOS_H_OPCODES
#define MANGOS_H_OPCODES

#include "Common.h"
#include "Policies/Singleton.h"

// Note: this include need for be sure have full definition of class WorldSession
//       if this class definition not complite then VS for x64 release use different size for
//       struct OpcodeHandler in this header and Opcode.cpp and get totally wrong data from
//       table opcodeTable in source when Opcode.h included but WorldSession.h not included
#include "WorldSession.h"

/**
 * This is a list of Opcodes that are known for the client/server communication, it is used
 * to tell the server to do something or the client to do something. Every opcode is handled
 * in some way, and you can find what functions handle what opcode in the implementation of
 * \ref Opcodes::BuildOpcodeList
 *
 * To send messages the following functions can be used: \ref WorldObject::SendMessageToSet,
 * \ref WorldObject::SendMessageToSetExcept, \ref WorldObject::SendMessageToSetInRange
 *
 * \see WorldPacket
 * \todo Replace the Pack GUID part with a packed GUID, ie: it's shorter than usual?
 */
enum OpcodesList
{
    MSG_WOW_CONNECTION                                    = 0x4F57, // 4.3.4 15595
    SMSG_AUTH_CHALLENGE                                   = 0x4542, // 4.3.4 15595
    CMSG_AUTH_SESSION                                     = 0x0449, // 4.3.4 15595
    SMSG_AUTH_RESPONSE                                    = 0x5DB6, // 4.3.4 15595
    MSG_NULL_ACTION                                       = 0x1001,
    CMSG_BOOTME                                           = 0x1002,
    CMSG_DBLOOKUP                                         = 0x1003,
    SMSG_DBLOOKUP                                         = 0x1004,
    CMSG_QUERY_OBJECT_POSITION                            = 0x1005,
    SMSG_QUERY_OBJECT_POSITION                            = 0x1006,
    CMSG_QUERY_OBJECT_ROTATION                            = 0x1007,
    SMSG_QUERY_OBJECT_ROTATION                            = 0x1008,
    CMSG_WORLD_TELEPORT                                   = 0x24B2, // 4.3.4 15595
    CMSG_TELEPORT_TO_UNIT                                 = 0x4206, // 4.3.4 15595
    CMSG_ZONE_MAP                                         = 0x100B,
    SMSG_ZONE_MAP                                         = 0x100C,
    CMSG_DEBUG_CHANGECELLZONE                             = 0x100D,
    CMSG_MOVE_CHARACTER_CHEAT                             = 0x100E,
    SMSG_MOVE_CHARACTER_CHEAT                             = 0x100F,
    CMSG_RECHARGE                                         = 0x1010,
    CMSG_LEARN_SPELL                                      = 0x1011,
    CMSG_CREATEMONSTER                                    = 0x1012,
    CMSG_DESTROYMONSTER                                   = 0x1013,
    CMSG_CREATEITEM                                       = 0x1014,
    CMSG_CREATEGAMEOBJECT                                 = 0x1015,
    SMSG_CHECK_FOR_BOTS                                   = 0x1016,
    CMSG_MAKEMONSTERATTACKGUID                            = 0x1017,
    CMSG_BOT_DETECTED2                                    = 0x1018,
    CMSG_FORCEACTION                                      = 0x1019,
    CMSG_FORCEACTIONONOTHER                               = 0x101A,
    CMSG_FORCEACTIONSHOW                                  = 0x101B,
    SMSG_FORCEACTIONSHOW                                  = 0x101C,
    CMSG_PETGODMODE                                       = 0x101D,
    SMSG_PETGODMODE                                       = 0x101E,
    SMSG_REFER_A_FRIEND_EXPIRED                           = 0x101F,
    CMSG_WEATHER_SPEED_CHEAT                              = 0x1020,
    CMSG_UNDRESSPLAYER                                    = 0x1021,
    CMSG_BEASTMASTER                                      = 0x1022,
    CMSG_GODMODE                                          = 0x1023,
    SMSG_GODMODE                                          = 0x1024,
    CMSG_CHEAT_SETMONEY                                   = 0x1025,
    CMSG_LEVEL_CHEAT                                      = 0x1026,
    CMSG_PET_LEVEL_CHEAT                                  = 0x1027,
    CMSG_SET_WORLDSTATE                                   = 0x1028,
    CMSG_COOLDOWN_CHEAT                                   = 0x1029,
    CMSG_USE_SKILL_CHEAT                                  = 0x102A,
    CMSG_FLAG_QUEST                                       = 0x102B,
    CMSG_FLAG_QUEST_FINISH                                = 0x102C,
    CMSG_CLEAR_QUEST                                      = 0x102D,
    CMSG_SEND_EVENT                                       = 0x102E,
    CMSG_DEBUG_AISTATE                                    = 0x102F,
    SMSG_DEBUG_AISTATE                                    = 0x1030,
    CMSG_DISABLE_PVP_CHEAT                                = 0x1031,
    CMSG_ADVANCE_SPAWN_TIME                               = 0x1032,
    SMSG_DESTRUCTIBLE_BUILDING_DAMAGE                     = 0x4825, // 4.3.4 15595
    CMSG_AUTH_SRP6_BEGIN                                  = 0x1034,
    CMSG_AUTH_SRP6_PROOF                                  = 0x1035,
    CMSG_AUTH_SRP6_RECODE                                 = 0x1036,
    CMSG_CHAR_CREATE                                      = 0x4A36, // 4.3.4 15595
    CMSG_CHAR_ENUM                                        = 0x0502, // 4.3.4 15595
    CMSG_CHAR_DELETE                                      = 0x6425, // 4.3.4 15595
    SMSG_AUTH_SRP6_RESPONSE                               = 0x103A,
    SMSG_CHAR_CREATE                                      = 0x2D05, // 4.3.4 15595
    SMSG_CHAR_ENUM                                        = 0x10B0, // 4.3.4 15595
    SMSG_CHAR_DELETE                                      = 0x0304, // 4.3.4 15595
    CMSG_PLAYER_LOGIN                                     = 0x05B1, // 4.3.4 15595
    SMSG_NEW_WORLD                                        = 0x79B1, // 4.3.4 15595
    SMSG_TRANSFER_PENDING                                 = 0x18A6, // 4.3.4 15595
    SMSG_TRANSFER_ABORTED                                 = 0x0537, // 4.3.4 15595
    SMSG_CHARACTER_LOGIN_FAILED                           = 0x4417, // 4.3.4 15595
    SMSG_LOGIN_SETTIMESPEED                               = 0x4D15, // 4.3.4 15595
    SMSG_GAMETIME_UPDATE                                  = 0x4127, // 4.3.4 15595
    CMSG_GAMETIME_SET                                     = 0x1045,
    SMSG_GAMETIME_SET                                     = 0x0014, // 4.3.4 15595
    CMSG_GAMESPEED_SET                                    = 0x1047,
    SMSG_GAMESPEED_SET                                    = 0x1048,
    CMSG_SERVERTIME                                       = 0x1049,
    SMSG_SERVERTIME                                       = 0x6327, // 4.3.4 15595
    CMSG_PLAYER_LOGOUT                                    = 0x104B,
    CMSG_LOGOUT_REQUEST                                   = 0x0A25, // 4.3.4 15595
    SMSG_LOGOUT_RESPONSE                                  = 0x0524, // 4.3.4 15595
    SMSG_LOGOUT_COMPLETE                                  = 0x2137, // 4.3.4 15595
    CMSG_LOGOUT_CANCEL                                    = 0x2324, // 4.3.4 15595
    SMSG_LOGOUT_CANCEL_ACK                                = 0x6514, // 4.3.4 15595
    CMSG_NAME_QUERY                                       = 0x2224, // 4.3.4 15595
    SMSG_NAME_QUERY_RESPONSE                              = 0x6E04, // 4.3.4 15595
    CMSG_PET_NAME_QUERY                                   = 0x6F24, // 4.3.4 15595
    SMSG_PET_NAME_QUERY_RESPONSE                          = 0x4C37, // 4.3.4 15595
    CMSG_GUILD_QUERY                                      = 0x4426, // 4.3.4 15595
    SMSG_GUILD_QUERY_RESPONSE                             = 0x0E06, // 4.3.4 15595
    CMSG_PAGE_TEXT_QUERY                                  = 0x6614, // 4.3.4 15595
    SMSG_PAGE_TEXT_QUERY_RESPONSE                         = 0x2B14, // 4.3.4 15595
    CMSG_QUEST_QUERY                                      = 0x0D06, // 4.3.4 15595
    SMSG_QUEST_QUERY_RESPONSE                             = 0x6936, // 4.3.4 15595
    CMSG_GAMEOBJECT_QUERY                                 = 0x4017, // 4.3.4 15595
    SMSG_GAMEOBJECT_QUERY_RESPONSE                        = 0x0915, // 4.3.4 15595
    CMSG_CREATURE_QUERY                                   = 0x2706, // 4.3.4 15595
    SMSG_CREATURE_QUERY_RESPONSE                          = 0x6024, // 4.3.4 15595
    CMSG_WHO                                              = 0x6C15, // 4.3.4 15595
    SMSG_WHO                                              = 0x6907, // 4.3.4 15595
    CMSG_WHOIS                                            = 0x6B05, // 4.3.4 15595
    SMSG_WHOIS                                            = 0x6917, // 4.3.4 15595
    CMSG_CONTACT_LIST                                     = 0x4534, // 4.3.4 15595
    SMSG_CONTACT_LIST                                     = 0x6017, // 4.3.4 15595
    SMSG_FRIEND_STATUS                                    = 0x0717, // 4.3.4 15595
    CMSG_ADD_FRIEND                                       = 0x6527, // 4.3.4 15595
    CMSG_DEL_FRIEND                                       = 0x6A15, // 4.3.4 15595
    CMSG_SET_CONTACT_NOTES                                = 0x6135, // 4.3.4 15595
    CMSG_ADD_IGNORE                                       = 0x4726, // 4.3.4 15595
    CMSG_DEL_IGNORE                                       = 0x6D26, // 4.3.4 15595
    CMSG_GROUP_INVITE                                     = 0x0513, // 4.3.4 15595
    SMSG_GROUP_INVITE                                     = 0x31B2, // 4.3.4 15595
    CMSG_GROUP_CANCEL                                     = 0x1071,
    SMSG_GROUP_CANCEL                                     = 0x4D25, // 4.3.4 15595
    CMSG_GROUP_INVITE_RESPONSE                            = 0x0410, // 4.3.4 15595
    SMSG_GROUP_DECLINE                                    = 0x6835, // 4.3.4 15595
    CMSG_GROUP_UNINVITE                                   = 0x1076,
    CMSG_GROUP_UNINVITE_GUID                              = 0x2E07, // 4.3.4 15595
    SMSG_GROUP_UNINVITE                                   = 0x0A07, // 4.3.4 15595
    CMSG_GROUP_SET_LEADER                                 = 0x4C17, // 4.3.4 15595
    SMSG_GROUP_SET_LEADER                                 = 0x0526, // 4.3.4 15595
    CMSG_LOOT_METHOD                                      = 0x2F24, // 4.3.4 15595
    CMSG_GROUP_DISBAND                                    = 0x2804, // 4.3.4 15595
    SMSG_GROUP_DESTROYED                                  = 0x2207, // 4.3.4 15595
    SMSG_GROUP_LIST                                       = 0x4C24, // 4.3.4 15595
    SMSG_PARTY_MEMBER_STATS                               = 0x2104, // 4.3.4 15595
    SMSG_PARTY_COMMAND_RESULT                             = 0x6E07, // 4.3.4 15595
    UMSG_UPDATE_GROUP_MEMBERS                             = 0x1081,
    CMSG_GUILD_CREATE                                     = 0x1082,
    CMSG_GUILD_INVITE                                     = 0x24B0, // 4.3.4 15595
    SMSG_GUILD_INVITE                                     = 0x14A2, // 4.3.4 15595
    CMSG_GUILD_ACCEPT                                     = 0x2531, // 4.3.4 15595
    CMSG_GUILD_DECLINE                                    = 0x3231, // 4.3.4 15595
    SMSG_GUILD_DECLINE                                    = 0x2C07, // 4.3.4 15595
    CMSG_GUILD_INFO                                       = 0x1088,
    SMSG_GUILD_INFO                                       = 0x1089,
    CMSG_GUILD_ROSTER                                     = 0x1226, // 4.3.4 15595
    SMSG_GUILD_ROSTER                                     = 0x3DA3, // 4.3.4 15595
    CMSG_GUILD_PROMOTE                                    = 0x1030, // 4.3.4 15595
    CMSG_GUILD_SET_RANK                                   = 0x3032, // 4.3.4 15595
    CMSG_GUILD_SWITCH_RANK                                = 0x1221, // 4.3.4 15595
    CMSG_GUILD_DEMOTE                                     = 0x1020, // 4.3.4 15595
    CMSG_GUILD_LEAVE                                      = 0x1021, // 4.3.4 15595
    CMSG_GUILD_REMOVE                                     = 0x1231, // 4.3.4 15595
    CMSG_GUILD_DISBAND                                    = 0x3226, // 4.3.4 15595
    CMSG_GUILD_LEADER                                     = 0x3034, // 4.3.4 15595
    CMSG_GUILD_MOTD                                       = 0x1035, // 4.3.4 15595
    SMSG_GUILD_EVENT                                      = 0x0705, // 4.3.4 15595
    SMSG_GUILD_COMMAND_RESULT                             = 0x7DB3, // 4.3.4 15595
    CMSG_GUILD_AUTO_DECLINE_TOGGLE                        = 0x2034, // 4.3.4 15595
    CMSG_GUILD_AUTO_DECLINE                               = 0x1234, // 4.3.4 15595
    CMSG_GUILD_QUERY_RANKS                                = 0x1026, // 4.3.4 15595
    SMSG_GUILD_QUERY_RANKS_RESULT                         = 0x30B4, // 4.3.4 15595
    UMSG_UPDATE_GUILD                                     = 0x1095,
    CMSG_MESSAGECHAT_ADDON_BATTLEGROUND                   = 0x0D46, // 4.3.4 15595
    CMSG_MESSAGECHAT_ADDON_GUILD                          = 0x0544, // 4.3.4 15595
    CMSG_MESSAGECHAT_ADDON_OFFICER                        = 0x3954, // 4.3.4 15595
    CMSG_MESSAGECHAT_ADDON_PARTY                          = 0x0546, // 4.3.4 15595
    CMSG_MESSAGECHAT_ADDON_RAID                           = 0x1D56, // 4.3.4 15595
    CMSG_MESSAGECHAT_ADDON_WHISPER                        = 0x2146, // 4.3.4 15595
    CMSG_MESSAGECHAT_AFK                                  = 0x0D44, // 4.3.4 15595
    CMSG_MESSAGECHAT_BATTLEGROUND                         = 0x2156, // 4.3.4 15595
    CMSG_MESSAGECHAT_CHANNEL                              = 0x1D44, // 4.3.4 15595
    CMSG_MESSAGECHAT_DND                                  = 0x2946, // 4.3.4 15595
    CMSG_MESSAGECHAT_EMOTE                                = 0x1156, // 4.3.4 15595
    CMSG_MESSAGECHAT_GUILD                                = 0x3956, // 4.3.4 15595
    CMSG_MESSAGECHAT_OFFICER                              = 0x1946, // 4.3.4 15595
    CMSG_MESSAGECHAT_PARTY                                = 0x1D46, // 4.3.4 15595
    CMSG_MESSAGECHAT_RAID                                 = 0x2D44, // 4.3.4 15595
    CMSG_MESSAGECHAT_RAID_WARNING                         = 0x0944, // 4.3.4 15595
    CMSG_MESSAGECHAT_SAY                                  = 0x1154, // 4.3.4 15595
    CMSG_MESSAGECHAT_WHISPER                              = 0x0D56, // 4.3.4 15595
    CMSG_MESSAGECHAT_YELL                                 = 0x3544, // 4.3.4 15595
    SMSG_MESSAGECHAT                                      = 0x2026, // 4.3.4 15595
    CMSG_JOIN_CHANNEL                                     = 0x0156, // 4.3.4 15595
    CMSG_LEAVE_CHANNEL                                    = 0x2D56, // 4.3.4 15595
    SMSG_CHANNEL_NOTIFY                                   = 0x0825, // 4.3.4 15595
    CMSG_CHANNEL_LIST                                     = 0x1556, // 4.3.4 15595
    SMSG_CHANNEL_LIST                                     = 0x2214, // 4.3.4 15595
    CMSG_CHANNEL_PASSWORD                                 = 0x2556, // 4.3.4 15595
    CMSG_CHANNEL_SET_OWNER                                = 0x3556, // 4.3.4 15595
    CMSG_CHANNEL_OWNER                                    = 0x3D44, // 4.3.4 15595
    CMSG_CHANNEL_MODERATOR                                = 0x0146, // 4.3.4 15595
    CMSG_CHANNEL_UNMODERATOR                              = 0x1954, // 4.3.4 15595
    CMSG_CHANNEL_MUTE                                     = 0x2554, // 4.3.4 15595
    CMSG_CHANNEL_UNMUTE                                   = 0x3554, // 4.3.4 15595
    CMSG_CHANNEL_INVITE                                   = 0x0144, // 4.3.4 15595
    CMSG_CHANNEL_KICK                                     = 0x3156, // 4.3.4 15595
    CMSG_CHANNEL_BAN                                      = 0x3D56, // 4.3.4 15595
    CMSG_CHANNEL_UNBAN                                    = 0x2D46, // 4.3.4 15595
    CMSG_CHANNEL_ANNOUNCEMENTS                            = 0x1146, // 4.3.4 15595
    CMSG_CHANNEL_MODERATE                                 = 0x2944, // 4.3.4 15595
    SMSG_UPDATE_OBJECT                                    = 0x4715, // 4.3.4 15595
    SMSG_DESTROY_OBJECT                                   = 0x4724, // 4.3.4 15595
    CMSG_USE_ITEM                                         = 0x2C06, // 4.3.4 15595
    CMSG_OPEN_ITEM                                        = 0x6A34, // 4.3.4 15595
    CMSG_READ_ITEM                                        = 0x2F16, // 4.3.4 15595
    SMSG_READ_ITEM_OK                                     = 0x2605, // 4.3.4 15595
    SMSG_READ_ITEM_FAILED                                 = 0x0F16, // 4.3.4 15595
    SMSG_ITEM_COOLDOWN                                    = 0x4D14, // 4.3.4 15595
    CMSG_GAMEOBJ_USE                                      = 0x4E17, // 4.3.4 15595
    CMSG_DESTROY_ITEMS                                    = 0x10B3,
    SMSG_GAMEOBJECT_CUSTOM_ANIM                           = 0x4936, // 4.3.4 15595
    CMSG_AREATRIGGER                                      = 0x0937, // 4.3.4 15595
    CMSG_MOVE_START_FORWARD                               = 0x7814, // 4.3.4 15595
    CMSG_MOVE_START_BACKWARD                              = 0x330A, // 4.3.4 15595
    CMSG_MOVE_STOP                                        = 0x320A, // 4.3.4 15595
    CMSG_MOVE_START_STRAFE_LEFT                           = 0x3A16, // 4.3.4 15595
    CMSG_MOVE_START_STRAFE_RIGHT                          = 0x3A02, // 4.3.4 15595
    CMSG_MOVE_STOP_STRAFE                                 = 0x3002, // 4.3.4 15595
    CMSG_MOVE_JUMP                                        = 0x7A06, // 4.3.4 15595
    CMSG_MOVE_START_TURN_LEFT                             = 0x700C, // 4.3.4 15595
    CMSG_MOVE_START_TURN_RIGHT                            = 0x7000, // 4.3.4 15595
    CMSG_MOVE_STOP_TURN                                   = 0x331E, // 4.3.4 15595
    CMSG_MOVE_START_PITCH_UP                              = 0x3304, // 4.3.4 15595
    CMSG_MOVE_START_PITCH_DOWN                            = 0x3908, // 4.3.4 15595
    CMSG_MOVE_STOP_PITCH                                  = 0x7216, // 4.3.4 15595
    CMSG_MOVE_SET_RUN_MODE                                = 0x791A, // 4.3.4 15595
    CMSG_MOVE_SET_WALK_MODE                               = 0x7002, // 4.3.4 15595
    MSG_MOVE_TOGGLE_LOGGING                               = 0x10C5,
    SMSG_MOVE_TELEPORT                                    = 0x55A0, // 4.3.4 15595
    MSG_MOVE_TELEPORT_CHEAT                               = 0x10C7,
    CMSG_MOVE_TELEPORT_ACK                                = 0x390C, // 4.3.4 15595
    MSG_MOVE_TOGGLE_FALL_LOGGING                          = 0x10C9,
    CMSG_MOVE_FALL_LAND                                   = 0x380A, // 4.3.4 15595
    CMSG_MOVE_START_SWIM                                  = 0x3206, // 4.3.4 15595
    CMSG_MOVE_STOP_SWIM                                   = 0x3802, // 4.3.4 15595
    MSG_MOVE_SET_RUN_SPEED_CHEAT                          = 0x10CD,
    SMSG_MOVE_SET_RUN_SPEED                               = 0x3DB5, // 4.3.4 15595
    MSG_MOVE_SET_RUN_BACK_SPEED_CHEAT                     = 0x10CF,
    SMSG_MOVE_SET_RUN_BACK_SPEED                          = 0x71B1, // 4.3.4 15595
    MSG_MOVE_SET_WALK_SPEED_CHEAT                         = 0x10D1,
    SMSG_MOVE_SET_WALK_SPEED                              = 0x1DA4, // 4.3.4 15595
    MSG_MOVE_SET_SWIM_SPEED_CHEAT                         = 0x10D3,
    SMSG_MOVE_SET_SWIM_SPEED                              = 0x15A7, // 4.3.4 15595
    MSG_MOVE_SET_SWIM_BACK_SPEED_CHEAT                    = 0x10D5,
    SMSG_MOVE_SET_SWIM_BACK_SPEED                         = 0x5CA6, // 4.3.4 15595
    MSG_MOVE_SET_ALL_SPEED_CHEAT                          = 0x10D7,
    MSG_MOVE_SET_TURN_RATE_CHEAT                          = 0x10D8,
    SMSG_MOVE_SET_TURN_RATE                               = 0x30A5, // 4.3.4 15595
    MSG_MOVE_TOGGLE_COLLISION_CHEAT                       = 0x10DA,
    CMSG_MOVE_SET_FACING                                  = 0x7914, // 4.3.4 15595
    CMSG_MOVE_SET_PITCH                                   = 0x7312, // 4.3.4 15595
    MSG_MOVE_WORLDPORT_ACK                                = 0x2411, // 4.3.4 15595
    SMSG_MONSTER_MOVE                                     = 0x6E17, // 4.3.4 15595
    SMSG_MOVE_WATER_WALK                                  = 0x75B1, // 4.3.4 15595
    SMSG_MOVE_LAND_WALK                                   = 0x34B7, // 4.3.4 15595
    CMSG_MOVE_CHARM_PORT_CHEAT                            = 0x10E1,
    CMSG_MOVE_SET_RAW_POSITION                            = 0x10E2,
    SMSG_FORCE_RUN_SPEED_CHANGE                           = 0x10E3,
    CMSG_FORCE_RUN_SPEED_CHANGE_ACK                       = 0x7818, // 4.3.4 15595
    SMSG_FORCE_RUN_BACK_SPEED_CHANGE                      = 0x10E5,
    CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK                  = 0x10E6,
    SMSG_FORCE_SWIM_SPEED_CHANGE                          = 0x10E7,
    CMSG_FORCE_SWIM_SPEED_CHANGE_ACK                      = 0x7A10, // 4.3.4 15595
    SMSG_FORCE_MOVE_ROOT                                  = 0x7DA0, // 4.3.4 15595
    CMSG_FORCE_MOVE_ROOT_ACK                              = 0x701E, // 4.3.4 15595
    SMSG_FORCE_MOVE_UNROOT                                = 0x7DB4, // 4.3.4 15595
    CMSG_FORCE_MOVE_UNROOT_ACK                            = 0x7808, // 4.3.4 15595
    MSG_MOVE_ROOT                                         = 0x10ED,
    MSG_MOVE_UNROOT                                       = 0x10EE,
    MSG_MOVE_HEARTBEAT                                    = 0x3914, // 4.3.4 15595
    SMSG_MOVE_KNOCK_BACK                                  = 0x5CB4, // 4.3.4 15595
    CMSG_MOVE_KNOCK_BACK_ACK                              = 0x721C, // 4.3.4 15595
    SMSG_MOVE_UPDATE_KNOCK_BACK                           = 0x3DB2, // 4.3.4 15595
    SMSG_MOVE_FEATHER_FALL                                = 0x79B0, // 4.3.4 15595
    SMSG_MOVE_NORMAL_FALL                                 = 0x51B6, // 4.3.4 15595
    SMSG_MOVE_SET_HOVER                                   = 0x5CB3, // 4.3.4 15595
    SMSG_MOVE_UNSET_HOVER                                 = 0x51B3, // 4.3.4 15595
    CMSG_MOVE_HOVER_ACK                                   = 0x3318, // 4.3.4 15595
    MSG_MOVE_HOVER                                        = 0x10F8,
    CMSG_TRIGGER_CINEMATIC_CHEAT                          = 0x10F9,
    CMSG_OPENING_CINEMATIC                                = 0x10FA,
    SMSG_TRIGGER_CINEMATIC                                = 0x6C27, // 4.3.4 15595
    CMSG_NEXT_CINEMATIC_CAMERA                            = 0x10FC,
    CMSG_COMPLETE_CINEMATIC                               = 0x2116, // 4.3.4 15595
    SMSG_TUTORIAL_FLAGS                                   = 0x0B35, // 4.3.4 15595
    CMSG_TUTORIAL_FLAG                                    = 0x6C26, // 4.3.4 15595
    CMSG_TUTORIAL_CLEAR                                   = 0x6515, // 4.3.4 15595
    CMSG_TUTORIAL_RESET                                   = 0x2726, // 4.3.4 15595
    CMSG_STANDSTATECHANGE                                 = 0x0535, // 4.3.4 15595
    CMSG_EMOTE                                            = 0x4C26, // 4.3.4 15595
    SMSG_EMOTE                                            = 0x0A34, // 4.3.4 15595
    CMSG_TEXT_EMOTE                                       = 0x2E24, // 4.3.4 15595
    SMSG_TEXT_EMOTE                                       = 0x0B05, // 4.3.4 15595
    CMSG_AUTOEQUIP_GROUND_ITEM                            = 0x1107,
    CMSG_AUTOSTORE_GROUND_ITEM                            = 0x1108,
    CMSG_AUTOSTORE_LOOT_ITEM                              = 0x0E34, // 4.3.4 15595
    CMSG_STORE_LOOT_IN_SLOT                               = 0x110A,
    CMSG_AUTOEQUIP_ITEM                                   = 0x4304, // 4.3.4 15595
    CMSG_AUTOSTORE_BAG_ITEM                               = 0x0236, // 4.3.4 15595
    CMSG_SWAP_ITEM                                        = 0x6326, // 4.3.4 15595
    CMSG_SWAP_INV_ITEM                                    = 0x2614, // 4.3.4 15595
    CMSG_SPLIT_ITEM                                       = 0x0F17, // 4.3.4 15595
    CMSG_AUTOEQUIP_ITEM_SLOT                              = 0x4A17, // 4.3.4 15595
    CMSG_UNCLAIM_LICENSE                                  = 0x1111,
    CMSG_DESTROYITEM                                      = 0x4A27, // 4.3.4 15595
    SMSG_INVENTORY_CHANGE_FAILURE                         = 0x2236, // 4.3.4 15595
    SMSG_OPEN_CONTAINER                                   = 0x4714, // 4.3.4 15595
    CMSG_INSPECT                                          = 0x0927, // 4.3.4 15595
    SMSG_INSPECT_RESULTS_UPDATE                           = 0x0C14, // 4.3.4 15595
    CMSG_INITIATE_TRADE                                   = 0x7916, // 4.3.4 15595
    CMSG_BEGIN_TRADE                                      = 0x721E, // 4.3.4 15595
    CMSG_BUSY_TRADE                                       = 0x331C, // 4.3.4 15595
    CMSG_IGNORE_TRADE                                     = 0x7112, // 4.3.4 15595
    CMSG_ACCEPT_TRADE                                     = 0x7110, // 4.3.4 15595
    CMSG_UNACCEPT_TRADE                                   = 0x391A, // 4.3.4 15595
    CMSG_CANCEL_TRADE                                     = 0x731E, // 4.3.4 15595
    CMSG_SET_TRADE_ITEM                                   = 0x7B0C, // 4.3.4 15595
    CMSG_CLEAR_TRADE_ITEM                                 = 0x7018, // 4.3.4 15595
    CMSG_SET_TRADE_GOLD                                   = 0x3008, // 4.3.4 15595
    SMSG_TRADE_STATUS                                     = 0x5CA3, // 4.3.4 15595
    SMSG_TRADE_STATUS_EXTENDED                            = 0x70A2, // 4.3.4 15595
    SMSG_INITIALIZE_FACTIONS                              = 0x4634, // 4.3.4 15595
    SMSG_SET_FACTION_VISIBLE                              = 0x2525, // 4.3.4 15595
    SMSG_SET_FACTION_STANDING                             = 0x0126, // 4.3.4 15595
    CMSG_SET_FACTION_ATWAR                                = 0x0706, // 4.3.4 15595
    CMSG_SET_FACTION_CHEAT                                = 0x1127,
    SMSG_SET_PROFICIENCY                                  = 0x6207, // 4.3.4 15595
    CMSG_SET_ACTION_BUTTON                                = 0x6F06, // 4.3.4 15595
    SMSG_ACTION_BUTTONS                                   = 0x38B5, // 4.3.4 15595
    SMSG_INITIAL_SPELLS                                   = 0x0104, // 4.3.4 15595
    SMSG_LEARNED_SPELL                                    = 0x58A2, // 4.3.4 15595
    SMSG_SUPERCEDED_SPELL                                 = 0x35B0, // 4.3.4 15595
    CMSG_NEW_SPELL_SLOT                                   = 0x112E,
    CMSG_CAST_SPELL                                       = 0x4C07, // 4.3.4 15595
    CMSG_CANCEL_CAST                                      = 0x0115, // 4.3.4 15595
    SMSG_CAST_FAILED                                      = 0x4D16, // 4.3.4 15595
    SMSG_SPELL_START                                      = 0x6415, // 4.3.4 15595
    SMSG_SPELL_GO                                         = 0x6E16, // 4.3.4 15595
    SMSG_SPELL_FAILURE                                    = 0x0C34, // 4.3.4 15595
    SMSG_SPELL_COOLDOWN                                   = 0x4B16, // 4.3.4 15595
    SMSG_COOLDOWN_EVENT                                   = 0x4F26, // 4.3.4 15595
    CMSG_CANCEL_AURA                                      = 0x0E26, // 4.3.4 15595
    SMSG_EQUIPMENT_SET_ID                                 = 0x2216, // 4.3.4 15595
    SMSG_PET_CAST_FAILED                                  = 0x2B15, // 4.3.4 15595
    SMSG_CHANNEL_START                                    = 0x0A15, // 4.3.4 15595
    SMSG_CHANNEL_UPDATE                                   = 0x2417, // 4.3.4 15595
    CMSG_CANCEL_CHANNELLING                               = 0x6C25, // 4.3.4 15595
    SMSG_AI_REACTION                                      = 0x0637, // 4.3.4 15595
    CMSG_SET_SELECTION                                    = 0x0506, // 4.3.4 15595
    CMSG_EQUIPMENT_SET_DELETE                              = 0x4D07, // 4.3.4 15595
    CMSG_INSTANCE_LOCK_RESPONSE                           = 0x1140,
    CMSG_DEBUG_PASSIVE_AURA                               = 0x1141,
    CMSG_ATTACKSWING                                      = 0x0926, // 4.3.4 15595
    CMSG_ATTACKSTOP                                       = 0x4106, // 4.3.4 15595
    SMSG_ATTACKSTART                                      = 0x2D15, // 4.3.4 15595
    SMSG_ATTACKSTOP                                       = 0x0934, // 4.3.4 15595
    SMSG_ATTACKSWING_NOTINRANGE                           = 0x0B36, // 4.3.4 15595
    SMSG_ATTACKSWING_BADFACING                            = 0x6C07, // 4.3.4 15595
    SMSG_PENDING_RAID_LOCK                                = 0x4F17, // 4.3.4 15595
    SMSG_ATTACKSWING_DEADTARGET                           = 0x2B26, // 4.3.4 15595
    SMSG_ATTACKSWING_CANT_ATTACK                          = 0x0016, // 4.3.4 15595
    SMSG_ATTACKERSTATEUPDATE                              = 0x0B25, // 4.3.4 15595
    SMSG_BATTLEFIELD_PORT_DENIED                          = 0x35A3, // 4.3.4 15595
    CMSG_PERFORM_ACTION_SET                               = 0x114D,
    SMSG_RESUME_CAST_BAR                                  = 0x114E,
    SMSG_CANCEL_COMBAT                                    = 0x4F04, // 4.3.4 15595
    SMSG_SPELLBREAKLOG                                    = 0x6B17, // 4.3.4 15595
    SMSG_SPELLHEALLOG                                     = 0x2816, // 4.3.4 15595
    SMSG_SPELLENERGIZELOG                                 = 0x0414, // 4.3.4 15595
    SMSG_BREAK_TARGET                                     = 0x0105, // 4.3.4 15595
    CMSG_SAVE_PLAYER                                      = 0x1154,
    CMSG_SETDEATHBINDPOINT                                = 0x1155,
    SMSG_BINDPOINTUPDATE                                  = 0x0527, // 4.3.4 15595
    CMSG_GETDEATHBINDZONE                                 = 0x1157,
    SMSG_BINDZONEREPLY                                    = 0x1158,
    SMSG_PLAYERBOUND                                      = 0x2516, // 4.3.4 15595
    SMSG_CLIENT_CONTROL_UPDATE                            = 0x2837, // 4.3.4 15595
    CMSG_REPOP_REQUEST                                    = 0x6235, // 4.3.4 15595
    SMSG_RESURRECT_REQUEST                                = 0x2905, // 4.3.4 15595
    CMSG_RESURRECT_RESPONSE                               = 0x6827, // 4.3.4 15595
    CMSG_RETURN_TO_GRAVEYARD                              = 0x301E, // 4.3.4 15595
    CMSG_LOOT                                             = 0x0127, // 4.3.4 15595
    CMSG_LOOT_CURRENCY                                    = 0x781C, // 4.3.4 15595
    CMSG_LOOT_MONEY                                       = 0x6227, // 4.3.4 15595
    CMSG_LOOT_RELEASE                                     = 0x2007, // 4.3.4 15595
    SMSG_LOOT_RESPONSE                                    = 0x4C16, // 4.3.4 15595
    SMSG_LOOT_RELEASE_RESPONSE                            = 0x6D25, // 4.3.4 15595
    SMSG_LOOT_REMOVED                                     = 0x6817, // 4.3.4 15595
    SMSG_LOOT_CURRENCY_REMOVED                            = 0x1DB4, // 4.3.4 15595
    SMSG_LOOT_MONEY_NOTIFY                                = 0x2836, // 4.3.4 15595
    SMSG_LOOT_ITEM_NOTIFY                                 = 0x6D15, // 4.3.4 15595
    SMSG_LOOT_CLEAR_MONEY                                 = 0x2B37, // 4.3.4 15595
    SMSG_ITEM_PUSH_RESULT                                 = 0x0E15, // 4.3.4 15595
    SMSG_DUEL_REQUESTED                                   = 0x4504, // 4.3.4 15595
    SMSG_DUEL_OUTOFBOUNDS                                 = 0x0C26, // 4.3.4 15595
    SMSG_DUEL_INBOUNDS                                    = 0x0A27, // 4.3.4 15595
    SMSG_DUEL_COMPLETE                                    = 0x2527, // 4.3.4 15595
    SMSG_DUEL_WINNER                                      = 0x2D36, // 4.3.4 15595
    CMSG_DUEL_ACCEPTED                                    = 0x2136, // 4.3.4 15595
    CMSG_DUEL_CANCELLED                                   = 0x6624, // 4.3.4 15595
    SMSG_MOUNTRESULT                                      = 0x2225, // 4.3.4 15595
    SMSG_DISMOUNTRESULT                                   = 0x0D25, // 4.3.4 15595
    SMSG_REMOVED_FROM_PVP_QUEUE                           = 0x1171,
    CMSG_MOUNTSPECIAL_ANIM                                = 0x2807, // 4.3.4 15595
    SMSG_MOUNTSPECIAL_ANIM                                = 0x0217, // 4.3.4 15595
    SMSG_PET_TAME_FAILURE                                 = 0x6B24, // 4.3.4 15595
    CMSG_PET_SET_ACTION                                   = 0x6904, // 4.3.4 15595
    CMSG_PET_ACTION                                       = 0x0226, // 4.3.4 15595
    CMSG_PET_ABANDON                                      = 0x0C24, // 4.3.4 15595
    CMSG_PET_RENAME                                       = 0x6406, // 4.3.4 15595
    SMSG_PET_NAME_INVALID                                 = 0x6007, // 4.3.4 15595
    SMSG_PET_SPELLS                                       = 0x4114, // 4.3.4 15595
    SMSG_PET_MODE                                         = 0x2235, // 4.3.4 15595
    CMSG_GOSSIP_HELLO                                     = 0x4525, // 4.3.4 15595
    CMSG_GOSSIP_SELECT_OPTION                             = 0x0216, // 4.3.4 15595
    SMSG_GOSSIP_MESSAGE                                   = 0x2035, // 4.3.4 15595
    SMSG_GOSSIP_COMPLETE                                  = 0x0806, // 4.3.4 15595
    CMSG_NPC_TEXT_QUERY                                   = 0x4E24, // 4.3.4 15595
    SMSG_NPC_TEXT_UPDATE                                  = 0x4436, // 4.3.4 15595
    SMSG_NPC_WONT_TALK                                    = 0x1182,
    CMSG_QUESTGIVER_STATUS_QUERY                          = 0x4407, // 4.3.4 15595
    SMSG_QUESTGIVER_STATUS                                = 0x2115, // 4.3.4 15595
    CMSG_QUESTGIVER_HELLO                                 = 0x0D17, // 4.3.4 15595
    SMSG_QUESTGIVER_QUEST_LIST                            = 0x0134, // 4.3.4 15595
    CMSG_QUESTGIVER_QUERY_QUEST                           = 0x2F14, // 4.3.4 15595
    CMSG_QUESTGIVER_QUEST_AUTOLAUNCH                      = 0x1188,
    SMSG_QUESTGIVER_QUEST_DETAILS                         = 0x2425, // 4.3.4 15595
    CMSG_QUESTGIVER_ACCEPT_QUEST                          = 0x6B37, // 4.3.4 15595
    CMSG_QUESTGIVER_COMPLETE_QUEST                        = 0x0114, // 4.3.4 15595
    SMSG_QUESTGIVER_REQUEST_ITEMS                         = 0x6236, // 4.3.4 15595
    CMSG_QUESTGIVER_REQUEST_REWARD                        = 0x2534, // 4.3.4 15595
    SMSG_QUESTGIVER_OFFER_REWARD                          = 0x2427, // 4.3.4 15595
    CMSG_QUESTGIVER_CHOOSE_REWARD                         = 0x2125, // 4.3.4 15595
    SMSG_QUESTGIVER_QUEST_INVALID                         = 0x4016, // 4.3.4 15595
    CMSG_QUESTGIVER_CANCEL                                = 0x1191,
    SMSG_QUESTGIVER_QUEST_COMPLETE                        = 0x55A4, // 4.3.4 15595
    SMSG_QUESTGIVER_QUEST_FAILED                          = 0x4236, // 4.3.4 15595
    CMSG_QUESTLOG_SWAP_QUEST                              = 0x1194,
    CMSG_QUESTLOG_REMOVE_QUEST                            = 0x0D16, // 4.3.4 15595
    SMSG_QUESTLOG_FULL                                    = 0x0E36, // 4.3.4 15595
    SMSG_QUESTUPDATE_FAILED                               = 0x6324, // 4.3.4 15595
    SMSG_QUESTUPDATE_FAILEDTIMER                          = 0x6427, // 4.3.4 15595
    SMSG_QUESTUPDATE_COMPLETE                             = 0x2937, // 4.3.4 15595
    SMSG_QUESTUPDATE_ADD_KILL                             = 0x0D27, // 4.3.4 15595
    SMSG_QUESTUPDATE_ADD_ITEM_OBSOLETE                    = 0x119B,
    CMSG_QUEST_CONFIRM_ACCEPT                             = 0x0D15, // 4.3.4 15595
    SMSG_QUEST_CONFIRM_ACCEPT                             = 0x6F07, // 4.3.4 15595
    CMSG_PUSHQUESTTOPARTY                                 = 0x4B14, // 4.3.4 15595
    CMSG_LIST_INVENTORY                                   = 0x2806, // 4.3.4 15595
    SMSG_LIST_INVENTORY                                   = 0x7CB0, // 4.3.4 15595
    CMSG_SELL_ITEM                                        = 0x4E15, // 4.3.4 15595
    SMSG_SELL_ITEM                                        = 0x6105, // 4.3.4 15595
    CMSG_BUY_ITEM                                         = 0x0736, // 4.3.4 15595
    SMSG_BUY_ITEM                                         = 0x0F26, // 4.3.4 15595
    SMSG_BUY_FAILED                                       = 0x6435, // 4.3.4 15595
    CMSG_TAXICLEARALLNODES                                = 0x11A7,
    CMSG_TAXIENABLEALLNODES                               = 0x11A8,
    CMSG_TAXISHOWNODES                                    = 0x11A9,
    SMSG_SHOWTAXINODES                                    = 0x2A36, // 4.3.4 15595
    CMSG_TAXINODE_STATUS_QUERY                            = 0x2F25, // 4.3.4 15595
    SMSG_TAXINODE_STATUS                                  = 0x2936, // 4.3.4 15595
    CMSG_TAXIQUERYAVAILABLENODES                          = 0x6C06, // 4.3.4 15595
    CMSG_ACTIVATETAXI                                     = 0x6E06, // 4.3.4 15595
    SMSG_ACTIVATETAXIREPLY                                = 0x6A37, // 4.3.4 15595
    SMSG_NEW_TAXI_PATH                                    = 0x4B35, // 4.3.4 15595
    CMSG_TRAINER_LIST                                     = 0x2336, // 4.3.4 15595
    SMSG_TRAINER_LIST                                     = 0x4414, // 4.3.4 15595
    CMSG_TRAINER_BUY_SPELL                                = 0x4415, // 4.3.4 15595
    SMSG_TRAINER_SERVICE                                  = 0x6A05, // 4.3.4 15595
    SMSG_TRAINER_BUY_FAILED                               = 0x0004, // 4.3.4 15595
    CMSG_BINDER_ACTIVATE                                  = 0x4006, // 4.3.4 15595
    SMSG_PLAYERBINDERROR                                  = 0x6A24, // 4.3.4 15595
    CMSG_BANKER_ACTIVATE                                  = 0x0005, // 4.3.4 15595
    SMSG_SHOW_BANK                                        = 0x2627, // 4.3.4 15595
    CMSG_BUY_BANK_SLOT                                    = 0x0425, // 4.3.4 15595
    CMSG_PETITION_SHOWLIST                                = 0x4617, // 4.3.4 15595
    SMSG_PETITION_SHOWLIST                                = 0x6405, // 4.3.4 15595
    CMSG_PETITION_BUY                                     = 0x4E05, // 4.3.4 15595
    CMSG_PETITION_SHOW_SIGNATURES                         = 0x4F15, // 4.3.4 15595
    SMSG_PETITION_SHOW_SIGNATURES                         = 0x0716, // 4.3.4 15595
    CMSG_PETITION_SIGN                                    = 0x0E04, // 4.3.4 15595
    SMSG_PETITION_SIGN_RESULTS                            = 0x6217, // 4.3.4 15595
    MSG_PETITION_DECLINE                                  = 0x4905, // 4.3.4 15595
    CMSG_OFFER_PETITION                                   = 0x4817, // 4.3.4 15595
    CMSG_TURN_IN_PETITION                                 = 0x0B27, // 4.3.4 15595
    SMSG_TURN_IN_PETITION_RESULTS                         = 0x0F07, // 4.3.4 15595
    CMSG_PETITION_QUERY                                   = 0x4424, // 4.3.4 15595
    SMSG_PETITION_QUERY_RESPONSE                          = 0x4B37, // 4.3.4 15595
    SMSG_FISH_NOT_HOOKED                                  = 0x0A17, // 4.3.4 15595
    SMSG_FISH_ESCAPED                                     = 0x2205, // 4.3.4 15595
    CMSG_BUG                                              = 0x4035, // 4.3.4 15595
    SMSG_NOTIFICATION                                     = 0x14A0, // 4.3.4 15595
    CMSG_PLAYED_TIME                                      = 0x0804, // 4.3.4 15595
    SMSG_PLAYED_TIME                                      = 0x6037, // 4.3.4 15595
    CMSG_QUERY_TIME                                       = 0x0A36, // 4.3.4 15595
    SMSG_QUERY_TIME_RESPONSE                              = 0x2124, // 4.3.4 15595
    SMSG_LOG_XPGAIN                                       = 0x4514, // 4.3.4 15595
    SMSG_AURACASTLOG                                      = 0x11D2,
    CMSG_RECLAIM_CORPSE                                   = 0x4036, // 4.3.4 15595
    CMSG_WRAP_ITEM                                        = 0x4F06, // 4.3.4 15595
    SMSG_LEVELUP_INFO                                     = 0x0435, // 4.3.4 15595
    MSG_MINIMAP_PING                                      = 0x6635, // 4.3.4 15595
    SMSG_RESISTLOG                                        = 0x11D7,
    SMSG_ENCHANTMENTLOG                                   = 0x6035, // 4.3.4 15595
    CMSG_SET_SKILL_CHEAT                                  = 0x11D9,
    SMSG_START_MIRROR_TIMER                               = 0x6824, // 4.3.4 15595
    SMSG_PAUSE_MIRROR_TIMER                               = 0x4015, // 4.3.4 15595
    SMSG_STOP_MIRROR_TIMER                                = 0x0B06, // 4.3.4 15595
    CMSG_PING                                             = 0x444D, // 4.3.4 15595
    SMSG_PONG                                             = 0x4D42, // 4.3.4 15595
    SMSG_CLEAR_COOLDOWNS                                  = 0x59B4, // 4.3.4 15595
    SMSG_GAMEOBJECT_PAGETEXT                              = 0x2925, // 4.3.4 15595
    CMSG_SETSHEATHED                                      = 0x4326, // 4.3.4 15595
    SMSG_COOLDOWN_CHEAT                                   = 0x4537, // 4.3.4 15595
    SMSG_SPELL_DELAYED                                    = 0x0715, // 4.3.4 15595
    CMSG_QUEST_POI_QUERY                                  = 0x4037, // 4.3.4 15595
    SMSG_QUEST_POI_QUERY_RESPONSE                         = 0x6304, // 4.3.4 15595
    CMSG_GHOST                                            = 0x11E6,
    CMSG_GM_INVIS                                         = 0x11E7,
    SMSG_INVALID_PROMOTION_CODE                           = 0x6F25, // 4.3.4 15595
    MSG_GM_BIND_OTHER                                     = 0x11E9,
    MSG_GM_SUMMON                                         = 0x11EA,
    SMSG_ITEM_TIME_UPDATE                                 = 0x2407, // 4.3.4 15595
    SMSG_ITEM_ENCHANT_TIME_UPDATE                         = 0x0F27, // 4.3.4 15595
    MSG_GM_SHOWLABEL                                      = 0x11F0,
    CMSG_PET_CAST_SPELL                                   = 0x6337, // 4.3.4 15595
    MSG_SAVE_GUILD_EMBLEM                                 = 0x2404, // 4.3.4 15595
    MSG_TABARDVENDOR_ACTIVATE                             = 0x6926, // 4.3.4 15595
    SMSG_PLAY_SPELL_VISUAL                                = 0x55A5, // 4.3.4 15595
    CMSG_ZONEUPDATE                                       = 0x4F37, // 4.3.4 15595
    SMSG_PARTYKILLLOG                                     = 0x4937, // 4.3.4 15595
    SMSG_COMPRESSED_UPDATE_OBJECT                         = 0x11F7,
    SMSG_EXPLORATION_EXPERIENCE                           = 0x6716, // 4.3.4 15595
    CMSG_GM_SET_SECURITY_GROUP                            = 0x11FA,
    CMSG_GM_NUKE                                          = 0x11FB,
    MSG_RANDOM_ROLL                                       = 0x0905, // 4.3.4 15595
    SMSG_ENVIRONMENTALDAMAGELOG                           = 0x6C05, // 4.3.4 15595
    CMSG_CHANGEPLAYER_DIFFICULTY                          = 0x6107, // 4.3.4 15595
    SMSG_RWHOIS                                           = 0x11FF,
    SMSG_LFG_PLAYER_REWARD                                = 0x1200,
    SMSG_LFG_TELEPORT_DENIED                              = 0x1201,
    CMSG_UNLEARN_SPELL                                    = 0x1202,
    CMSG_UNLEARN_SKILL                                    = 0x6106, // 4.3.4 15595
    SMSG_REMOVED_SPELL                                    = 0x4804, // 4.3.4 15595
    CMSG_DECHARGE                                         = 0x1205,
    CMSG_GMTICKET_CREATE                                  = 0x0137, // 4.3.4 15595
    SMSG_GMTICKET_CREATE                                  = 0x2107, // 4.3.4 15595
    CMSG_GMTICKET_UPDATETEXT                              = 0x0636, // 4.3.4 15595
    SMSG_GMTICKET_UPDATETEXT                              = 0x6535, // 4.3.4 15595
    SMSG_ACCOUNT_DATA_TIMES                               = 0x4B05, // 4.3.4 15595
    CMSG_REQUEST_ACCOUNT_DATA                             = 0x6505, // 4.3.4 15595
    CMSG_UPDATE_ACCOUNT_DATA                              = 0x4736, // 4.3.4 15595
    SMSG_UPDATE_ACCOUNT_DATA                              = 0x6837, // 4.3.4 15595
    SMSG_CLEAR_FAR_SIGHT_IMMEDIATE                        = 0x2A04, // 4.3.4 15595
    SMSG_CHANGEPLAYER_DIFFICULTY_RESULT                   = 0x2217, // 4.3.4 15595
    CMSG_GM_TEACH                                         = 0x1210,
    CMSG_GM_CREATE_ITEM_TARGET                            = 0x1211,
    CMSG_GMTICKET_GETTICKET                               = 0x0326, // 4.3.4 15595
    SMSG_GMTICKET_GETTICKET                               = 0x2C15, // 4.3.4 15595
    CMSG_UNLEARN_TALENTS                                  = 0x1214,
    SMSG_INSTANCE_ENCOUNTER                               = 0x1215,
    SMSG_GAMEOBJECT_DESPAWN_ANIM                          = 0x6735, // 4.3.4 15595
    MSG_CORPSE_QUERY                                      = 0x4336, // 4.3.4 15595
    CMSG_GMTICKET_DELETETICKET                            = 0x6B14, // 4.3.4 15595
    SMSG_GMTICKET_DELETETICKET                            = 0x6D17, // 4.3.4 15595
    SMSG_CHAT_WRONG_FACTION                               = 0x6724, // 4.3.4 15595
    CMSG_GMTICKET_SYSTEMSTATUS                            = 0x4205, // 4.3.4 15595
    SMSG_GMTICKET_SYSTEMSTATUS                            = 0x0D35, // 4.3.4 15595
    CMSG_SPIRIT_HEALER_ACTIVATE                           = 0x2E26, // 4.3.4 15595
    CMSG_SET_STAT_CHEAT                                   = 0x121E,
    SMSG_QUEST_FORCE_REMOVED                              = 0x121F,
    CMSG_SKILL_BUY_STEP                                   = 0x1220,
    CMSG_SKILL_BUY_RANK                                   = 0x1221,
    CMSG_XP_CHEAT                                         = 0x1222,
    SMSG_SPIRIT_HEALER_CONFIRM                            = 0x4917, // 4.3.4 15595
    CMSG_CHARACTER_POINT_CHEAT                            = 0x1224,
    SMSG_GOSSIP_POI                                       = 0x4316, // 4.3.4 15595
    CMSG_CHAT_IGNORED                                     = 0x0D54, // 4.3.4 15595
    CMSG_GM_VISION                                        = 0x1227,
    CMSG_SERVER_COMMAND                                   = 0x1228,
    CMSG_GM_SILENCE                                       = 0x1229,
    CMSG_GM_REVEALTO                                      = 0x122A,
    CMSG_GM_RESURRECT                                     = 0x122B,
    CMSG_GM_SUMMONMOB                                     = 0x122C,
    CMSG_GM_MOVECORPSE                                    = 0x122D,
    CMSG_GM_FREEZE                                        = 0x122E,
    CMSG_GM_UBERINVIS                                     = 0x122F,
    CMSG_GM_REQUEST_PLAYER_INFO                           = 0x1230,
    SMSG_GM_PLAYER_INFO                                   = 0x1231,
    CMSG_GUILD_RANK                                       = 0x1024, // 4.3.4 15595
    CMSG_GUILD_ADD_RANK                                   = 0x3030, // 4.3.4 15595
    CMSG_GUILD_DEL_RANK                                   = 0x3234, // 4.3.4 15595
    CMSG_GUILD_SET_NOTE                                   = 0x1233, // 4.3.4 15595
    SMSG_LOGIN_VERIFY_WORLD                               = 0x2005, // 4.3.4 15595
    CMSG_CLEAR_EXPLORATION                                = 0x1238,
    CMSG_SEND_MAIL                                        = 0x0523, // 4.3.4 15595
    SMSG_SEND_MAIL_RESULT                                 = 0x4927, // 4.3.4 15595
    CMSG_GET_MAIL_LIST                                    = 0x4D37, // 4.3.4 15595
    SMSG_MAIL_LIST_RESULT                                 = 0x4217, // 4.3.4 15595
    CMSG_BATTLEFIELD_LIST                                 = 0x3814, // 4.3.4 15595
    SMSG_BATTLEFIELD_LIST                                 = 0x71B5, // 4.3.4 15595
    CMSG_BATTLEFIELD_JOIN                                 = 0x123F,
    SMSG_FORCE_SET_VEHICLE_REC_ID                         = 0x1240,
    CMSG_SET_VEHICLE_REC_ID_ACK                           = 0x1241,
    CMSG_TAXICLEARNODE                                    = 0x1242,
    CMSG_TAXIENABLENODE                                   = 0x1243,
    CMSG_ITEM_TEXT_QUERY                                  = 0x2406, // 4.3.4 15595
    SMSG_ITEM_TEXT_QUERY_RESPONSE                         = 0x2725, // 4.3.4 15595
    CMSG_MAIL_TAKE_MONEY                                  = 0x4034, // 4.3.4 15595
    CMSG_MAIL_TAKE_ITEM                                   = 0x2B06, // 4.3.4 15595
    CMSG_MAIL_MARK_AS_READ                                = 0x0C07, // 4.3.4 15595
    CMSG_MAIL_RETURN_TO_SENDER                            = 0x0816, // 4.3.4 15595
    CMSG_MAIL_DELETE                                      = 0x6104, // 4.3.4 15595
    CMSG_MAIL_CREATE_TEXT_ITEM                            = 0x0B14, // 4.3.4 15595
    SMSG_SPELLLOGMISS                                     = 0x0625, // 4.3.4 15595
    SMSG_SPELLLOGEXECUTE                                  = 0x0626, // 4.3.4 15595
    SMSG_DEBUGAURAPROC                                    = 0x124E,
    SMSG_PERIODICAURALOG                                  = 0x0416, // 4.3.4 15595
    SMSG_SPELLDAMAGESHIELD                                = 0x2927, // 4.3.4 15595
    SMSG_SPELLNONMELEEDAMAGELOG                           = 0x4315, // 4.3.4 15595
    CMSG_LEARN_TALENT                                     = 0x0306, // 4.3.4 15595
    SMSG_RESURRECT_FAILED                                 = 0x1253,
    CMSG_TOGGLE_PVP                                       = 0x6815, // 4.3.4 15595
    SMSG_ZONE_UNDER_ATTACK                                = 0x0A06, // 4.3.4 15595
    MSG_AUCTION_HELLO                                     = 0x2307, // 4.3.4 15595
    CMSG_AUCTION_SELL_ITEM                                = 0x4A06, // 4.3.4 15595
    CMSG_AUCTION_REMOVE_ITEM                              = 0x6426, // 4.3.4 15595
    CMSG_AUCTION_LIST_ITEMS                               = 0x0324, // 4.3.4 15595
    CMSG_AUCTION_LIST_OWNER_ITEMS                         = 0x0206, // 4.3.4 15595
    CMSG_AUCTION_PLACE_BID                                = 0x2306, // 4.3.4 15595
    SMSG_AUCTION_COMMAND_RESULT                           = 0x4C25, // 4.3.4 15595
    SMSG_AUCTION_LIST_RESULT                              = 0x6637, // 4.3.4 15595
    SMSG_AUCTION_OWNER_LIST_RESULT                        = 0x6C34, // 4.3.4 15595
    SMSG_AUCTION_BIDDER_NOTIFICATION                      = 0x4E27, // 4.3.4 15595
    SMSG_AUCTION_OWNER_NOTIFICATION                       = 0x4116, // 4.3.4 15595
    SMSG_PROCRESIST                                       = 0x1261,
    SMSG_COMBAT_EVENT_FAILED                              = 0x1262,
    SMSG_DISPEL_FAILED                                    = 0x0307, // 4.3.4 15595
    SMSG_SPELLOGDAMAGE_IMMUNE                             = 0x4507, // 4.3.4 15595
    CMSG_AUCTION_LIST_BIDDER_ITEMS                        = 0x6937, // 4.3.4 15595
    SMSG_AUCTION_BIDDER_LIST_RESULT                       = 0x0027, // 4.3.4 15595
    SMSG_SET_FLAT_SPELL_MODIFIER                          = 0x2834, // 4.3.4 15595
    SMSG_SET_PCT_SPELL_MODIFIER                           = 0x0224, // 4.3.4 15595
    CMSG_SET_AMMO                                         = 0x1269,
    SMSG_CORPSE_RECLAIM_DELAY                             = 0x0D34, // 4.3.4 15595
    CMSG_SET_ACTIVE_MOVER                                 = 0x3314, // 4.3.4 15595
    CMSG_PET_CANCEL_AURA                                  = 0x4B25, // 4.3.4 15595
    CMSG_PLAYER_AI_CHEAT                                  = 0x126D,
    CMSG_CANCEL_AUTO_REPEAT_SPELL                         = 0x6C35, // 4.3.4 15595
    MSG_GM_ACCOUNT_ONLINE                                 = 0x126F,
    MSG_LIST_STABLED_PETS                                 = 0x0834, // 4.3.4 15595
    CMSG_STABLE_PET                                       = 0x1271,
    CMSG_UNSTABLE_PET                                     = 0x1272,
    CMSG_BUY_STABLE_SLOT                                  = 0x1273,
    SMSG_STABLE_RESULT                                    = 0x2204, // 4.3.4 15595
    CMSG_STABLE_REVIVE_PET                                = 0x1275,
    CMSG_STABLE_SWAP_PET                                  = 0x1276,
    MSG_QUEST_PUSH_RESULT                                 = 0x4515, // 4.3.4 15595
    SMSG_PLAY_MUSIC                                       = 0x4B06, // 4.3.4 15595
    SMSG_PLAY_OBJECT_SOUND                                = 0x2635, // 4.3.4 15595
    SMSG_PLAY_ONE_SHOT_ANIM_KIT                           = 0x4A35, // 4.3.4 15595
    CMSG_REQUEST_PET_INFO                                 = 0x4924, // 4.3.4 15595
    CMSG_FAR_SIGHT                                        = 0x4835, // 4.3.4 15595
    SMSG_SPELLDISPELLOG                                   = 0x4516, // 4.3.4 15595
    SMSG_DAMAGE_CALC_LOG                                  = 0x2436, // 4.3.4 15595
    CMSG_ENABLE_DAMAGE_LOG                                = 0x127E,
    CMSG_GROUP_CHANGE_SUB_GROUP                           = 0x4124, // 4.3.4 15595
    CMSG_REQUEST_PARTY_MEMBER_STATS                       = 0x0C04, // 4.3.4 15595
    CMSG_GROUP_SWAP_SUB_GROUP                             = 0x1281,
    CMSG_RESET_FACTION_CHEAT                              = 0x1282,
    CMSG_AUTOSTORE_BANK_ITEM                              = 0x0607, // 4.3.4 15595
    CMSG_AUTOBANK_ITEM                                    = 0x2537, // 4.3.4 15595
    MSG_QUERY_NEXT_MAIL_TIME                              = 0x0F04, // 4.3.4 15595
    SMSG_RECEIVED_MAIL                                    = 0x2924, // 4.3.4 15595
    SMSG_RAID_GROUP_ONLY                                  = 0x0837, // 4.3.4 15595
    CMSG_SET_DURABILITY_CHEAT                             = 0x1288,
    CMSG_SET_PVP_RANK_CHEAT                               = 0x1289,
    CMSG_ADD_PVP_MEDAL_CHEAT                              = 0x128A,
    CMSG_DEL_PVP_MEDAL_CHEAT                              = 0x128B,
    CMSG_SET_PVP_TITLE                                    = 0x128C,
    SMSG_PVP_CREDIT                                       = 0x6015, // 4.3.4 15595
    SMSG_AUCTION_REMOVED_NOTIFICATION                     = 0x2334, // 4.3.4 15595
    CMSG_GROUP_RAID_CONVERT                               = 0x6E27, // 4.3.4 15595
    CMSG_GROUP_REQUEST_JOIN_UPDATES                       = 0x2583, // 4.3.4 15595
    CMSG_GROUP_ASSISTANT_LEADER                           = 0x6025, // 4.3.4 15595
    CMSG_BUYBACK_ITEM                                     = 0x6C17, // 4.3.4 15595
    SMSG_SERVER_MESSAGE                                   = 0x6C04, // 4.3.4 15595
    CMSG_SET_SAVED_INSTANCE_EXTEND                        = 0x6706, // 4.3.4 15595
    SMSG_LFG_OFFER_CONTINUE                               = 0x1294,
    CMSG_TEST_DROP_RATE                                   = 0x1295,
    SMSG_TEST_DROP_RATE_RESULT                            = 0x1296,
    CMSG_LFG_GET_STATUS                                   = 0x2581, // 4.3.4 15595
    SMSG_SHOW_MAILBOX                                     = 0x2524, // 4.3.4 15595
    SMSG_RESET_RANGED_COMBAT_TIMER                        = 0x1299,
    SMSG_CHAT_NOT_IN_PARTY                                = 0x6A14, // 4.3.4 15595
    CMSG_GMTICKETSYSTEM_TOGGLE                            = 0x129B,
    CMSG_CANCEL_GROWTH_AURA                               = 0x0237, // 4.3.4 15595
    SMSG_CANCEL_AUTO_REPEAT                               = 0x6436, // 4.3.4 15595
    SMSG_STANDSTATE_UPDATE                                = 0x6F04, // 4.3.4 15595
    SMSG_LOOT_ALL_PASSED                                  = 0x6237, // 4.3.4 15595
    SMSG_LOOT_ROLL_WON                                    = 0x6617, // 4.3.4 15595
    CMSG_LOOT_ROLL                                        = 0x6934, // 4.3.4 15595
    SMSG_LOOT_START_ROLL                                  = 0x2227, // 4.3.4 15595
    SMSG_LOOT_ROLL                                        = 0x6507, // 4.3.4 15595
    CMSG_LOOT_MASTER_GIVE                                 = 0x4F35, // 4.3.4 15595
    SMSG_LOOT_MASTER_LIST                                 = 0x0325, // 4.3.4 15595
    SMSG_SET_FORCED_REACTIONS                             = 0x4615, // 4.3.4 15595
    SMSG_SPELL_FAILED_OTHER                               = 0x4535, // 4.3.4 15595
    SMSG_GAMEOBJECT_RESET_STATE                           = 0x2A16, // 4.3.4 15595
    CMSG_REPAIR_ITEM                                      = 0x2917, // 4.3.4 15595
    SMSG_CHAT_PLAYER_NOT_FOUND                            = 0x2526, // 4.3.4 15595
    MSG_TALENT_WIPE_CONFIRM                               = 0x0107, // 4.3.4 15595
    SMSG_SUMMON_REQUEST                                   = 0x2A07, // 4.3.4 15595
    CMSG_SUMMON_RESPONSE                                  = 0x6F27, // 4.3.4 15595
    MSG_DEV_SHOWLABEL                                     = 0x12AE,
    SMSG_MONSTER_MOVE_TRANSPORT                           = 0x2004, // 4.3.4 15595
    SMSG_PET_BROKEN                                       = 0x12B0,
    MSG_MOVE_FEATHER_FALL                                 = 0x12B1,
    MSG_MOVE_WATER_WALK                                   = 0x12B2,
    CMSG_SERVER_BROADCAST                                 = 0x12B3,
    CMSG_SELF_RES                                         = 0x6115, // 4.3.4 15595
    SMSG_FEIGN_DEATH_RESISTED                             = 0x12B5,
    CMSG_RUN_SCRIPT                                       = 0x12B6,
    SMSG_SCRIPT_MESSAGE                                   = 0x12B7,
    SMSG_DUEL_COUNTDOWN                                   = 0x4836, // 4.3.4 15595
    SMSG_AREA_TRIGGER_MESSAGE                             = 0x4505, // 4.3.4 15595
    CMSG_SHOWING_HELM                                     = 0x0735, // 4.3.4 15595
    CMSG_SHOWING_CLOAK                                    = 0x4135, // 4.3.4 15595
    SMSG_ROLE_CHOSEN                                      = 0x12BC,
    SMSG_PLAYER_SKINNED                                   = 0x0116, // 4.3.4 15595
    SMSG_DURABILITY_DAMAGE_DEATH                          = 0x4C27, // 4.3.4 15595
    CMSG_SET_EXPLORATION                                  = 0x12BF,
    CMSG_SET_ACTIONBAR_TOGGLES                            = 0x2506, // 4.3.4 15595
    UMSG_DELETE_GUILD_CHARTER                             = 0x12C1,
    MSG_PETITION_RENAME                                   = 0x4005, // 4.3.4 15595
    SMSG_INIT_WORLD_STATES                                = 0x4C15, // 4.3.4 15595
    SMSG_UPDATE_WORLD_STATE                               = 0x4816, // 4.3.4 15595
    SMSG_PET_ACTION_FEEDBACK                              = 0x0807, // 4.3.4 15595
    CMSG_CHAR_RENAME                                      = 0x2327, // 4.3.4 15595
    SMSG_CHAR_RENAME                                      = 0x2024, // 4.3.4 15595
    CMSG_MOVE_SPLINE_DONE                                 = 0x790E, // 4.3.4 15595
    CMSG_MOVE_FALL_RESET                                  = 0x310A, // 4.3.4 15595
    SMSG_INSTANCE_SAVE_CREATED                            = 0x0124, // 4.3.4 15595
    SMSG_RAID_INSTANCE_INFO                               = 0x6626, // 4.3.4 15595
    CMSG_REQUEST_RAID_INFO                                = 0x2F26, // 4.3.4 15595
    CMSG_MOVE_TIME_SKIPPED                                = 0x7A0A, // 4.3.4 15595
    CMSG_MOVE_FEATHER_FALL_ACK                            = 0x3110, // 4.3.4 15595
    CMSG_MOVE_WATER_WALK_ACK                              = 0x3B00, // 4.3.4 15595
    CMSG_MOVE_NOT_ACTIVE_MOVER                            = 0x7A1A, // 4.3.4 15595
    SMSG_PLAY_SOUND                                       = 0x2134, // 4.3.4 15595
    CMSG_BATTLEFIELD_STATUS                               = 0x2500, // 4.3.4 15595
    SMSG_BATTLEFIELD_STATUS                               = 0x7DA1, // 4.3.4 15595
    SMSG_BATTLEFIELD_STATUS_ACTIVE                        = 0x74A4, // 4.3.4 15595
    SMSG_BATTLEFIELD_STATUS_FAILED                        = 0x71A7, // 4.3.4 15595
    SMSG_BATTLEFIELD_STATUS_QUEUED                        = 0x35A1, // 4.3.4 15595
    SMSG_BATTLEFIELD_STATUS_NEEDCONFIRMATION              = 0x59A0, // 4.3.4 15595
    SMSG_BATTLEFIELD_STATUS_WAITFORGROUPS                 = 0x75A2, // 4.3.4 15595
    CMSG_BATTLEFIELD_PORT                                 = 0x711A, // 4.3.4 15595
    CMSG_INSPECT_HONOR_STATS                              = 0x791E, // 4.3.4 15595
    SMSG_INSPECT_HONOR_STATS                              = 0x79A5, // 4.3.4 15595
    CMSG_BATTLEMASTER_HELLO                               = 0x0234, // 4.3.4 15595
    CMSG_MOVE_START_SWIM_CHEAT                            = 0x12D9,
    CMSG_MOVE_STOP_SWIM_CHEAT                             = 0x12DA,
    SMSG_FORCE_WALK_SPEED_CHANGE                          = 0x12DB,
    CMSG_FORCE_WALK_SPEED_CHANGE_ACK                      = 0x12DC,
    SMSG_FORCE_SWIM_BACK_SPEED_CHANGE                     = 0x12DD,
    CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK                 = 0x12DE,
    SMSG_FORCE_TURN_RATE_CHANGE                           = 0x12DF,
    CMSG_FORCE_TURN_RATE_CHANGE_ACK                       = 0x12E0,
    CMSG_PVP_LOG_DATA                                     = 0x7308, // 4.3.4 15595
    SMSG_PVP_LOG_DATA                                     = 0x5CB2, // 4.3.4 15595
    CMSG_LEAVE_BATTLEFIELD                                = 0x3018, // 4.3.4 15595
    CMSG_AREA_SPIRIT_HEALER_QUERY                         = 0x4907, // 4.3.4 15595
    CMSG_AREA_SPIRIT_HEALER_QUEUE                         = 0x4815, // 4.3.4 15595
    SMSG_AREA_SPIRIT_HEALER_TIME                          = 0x0734, // 4.3.4 15595
    CMSG_GM_UNTEACH                                       = 0x12E6,
    SMSG_WARDEN_DATA                                      = 0x12E7,
    CMSG_WARDEN_DATA                                      = 0x12E8,
    CMSG_BATTLEGROUND_PLAYER_POSITIONS                    = 0x3902, // 4.3.4 15595
    SMSG_BATTLEGROUND_PLAYER_POSITIONS                    = 0x58B4, // 4.3.4 15595
    CMSG_PET_STOP_ATTACK                                  = 0x6C14, // 4.3.4 15595
    SMSG_BINDER_CONFIRM                                   = 0x2835, // 4.3.4 15595
    SMSG_BATTLEGROUND_PLAYER_JOINED                       = 0x50B0, // 4.3.4 15595
    SMSG_BATTLEGROUND_PLAYER_LEFT                         = 0x59A6, // 4.3.4 15595
    CMSG_BATTLEMASTER_JOIN                                = 0x7902, // 4.3.4 15595
    SMSG_ADDON_INFO                                       = 0x2C14, // 4.3.4 15595
    CMSG_PET_UNLEARN                                      = 0x12F1,
    SMSG_PET_UNLEARN_CONFIRM                              = 0x12F2,
    SMSG_PARTY_MEMBER_STATS_FULL                          = 0x0215, // 4.3.4 15595
    CMSG_PET_SPELL_AUTOCAST                               = 0x2514, // 4.3.4 15595
    SMSG_WEATHER                                          = 0x2904, // 4.3.4 15595
    SMSG_PLAY_TIME_WARNING                                = 0x12F6,
    SMSG_MINIGAME_SETUP                                   = 0x12F7,
    SMSG_MINIGAME_STATE                                   = 0x12F8,
    CMSG_MINIGAME_MOVE                                    = 0x12F9,
    SMSG_MINIGAME_MOVE_FAILED                             = 0x12FA,
    SMSG_RAID_INSTANCE_MESSAGE                            = 0x6E15, // 4.3.4 15595
    SMSG_COMPRESSED_MOVES                                 = 0x0517, // 4.3.4 15595
    CMSG_GUILD_INFO_TEXT                                  = 0x3227, // 4.3.4 15595
    SMSG_CHAT_RESTRICTED                                  = 0x6536, // 4.3.4 15595
    SMSG_SPLINE_SET_RUN_SPEED                             = 0x12FF,
    SMSG_SPLINE_SET_RUN_BACK_SPEED                        = 0x1300,
    SMSG_SPLINE_SET_SWIM_SPEED                            = 0x1301,
    SMSG_SPLINE_SET_WALK_SPEED                            = 0x1302,
    SMSG_SPLINE_SET_SWIM_BACK_SPEED                       = 0x1303,
    SMSG_SPLINE_SET_TURN_RATE                             = 0x1304,
    SMSG_SPLINE_MOVE_UNROOT                               = 0x75B6, // 4.3.4 15595
    SMSG_SPLINE_MOVE_FEATHER_FALL                         = 0x3DA5, // 4.3.4 15595
    SMSG_SPLINE_MOVE_NORMAL_FALL                          = 0x38B2, // 4.3.4 15595
    SMSG_SPLINE_MOVE_SET_HOVER                            = 0x14B6, // 4.3.4 15595
    SMSG_SPLINE_MOVE_UNSET_HOVER                          = 0x7DA5, // 4.3.4 15595
    SMSG_SPLINE_MOVE_WATER_WALK                           = 0x50A2, // 4.3.4 15595
    SMSG_SPLINE_MOVE_LAND_WALK                            = 0x3DA7, // 4.3.4 15595
    SMSG_SPLINE_MOVE_START_SWIM                           = 0x31A5, // 4.3.4 15595
    SMSG_SPLINE_MOVE_STOP_SWIM                            = 0x1DA2, // 4.3.4 15595
    SMSG_SPLINE_MOVE_SET_RUN_MODE                         = 0x75A7, // 4.3.4 15595
    SMSG_SPLINE_MOVE_SET_WALK_MODE                        = 0x54B6, // 4.3.4 15595
    CMSG_GM_NUKE_ACCOUNT                                  = 0x1310,
    MSG_GM_DESTROY_CORPSE                                 = 0x1311,
    CMSG_GM_DESTROY_ONLINE_CORPSE                         = 0x1312,
    CMSG_ACTIVATETAXIEXPRESS                              = 0x0515, // 4.3.4 15595
    SMSG_SET_FACTION_ATWAR                                = 0x1314,
    SMSG_SET_FACTION_NOT_VISIBLE = 0x0000, //TODO: Needs fixing up
    SMSG_GAMETIMEBIAS_SET = 0x1315,
    CMSG_DEBUG_ACTIONS_START                              = 0x1316,
    CMSG_DEBUG_ACTIONS_STOP                               = 0x1317,
    CMSG_SET_FACTION_INACTIVE                             = 0x0E37, // 4.3.4 15595
    CMSG_SET_WATCHED_FACTION                              = 0x2434, // 4.3.4 15595
    MSG_MOVE_TIME_SKIPPED                                 = 0x7A0A, // 4.3.4 15595
    SMSG_SPLINE_MOVE_ROOT                                 = 0x51B4, // 4.3.4 15595
    CMSG_SET_EXPLORATION_ALL                              = 0x131C,
    SMSG_INVALIDATE_PLAYER                                = 0x6325, // 4.3.4 15595
    CMSG_RESET_INSTANCES                                  = 0x6E14, // 4.3.4 15595
    SMSG_INSTANCE_RESET                                   = 0x6F05, // 4.3.4 15595
    SMSG_INSTANCE_RESET_FAILED                            = 0x4725, // 4.3.4 15595
    SMSG_UPDATE_LAST_INSTANCE                             = 0x0437, // 4.3.4 15595
    MSG_RAID_TARGET_UPDATE                                = 0x2C36, // 4.3.4 15595
    MSG_RAID_READY_CHECK                                  = 0x2304, // 4.3.4 15595
    CMSG_LUA_USAGE                                        = 0x1324,
    SMSG_PET_ACTION_SOUND                                 = 0x4324, // 4.3.4 15595
    SMSG_PET_DISMISS_SOUND                                = 0x2B05, // 4.3.4 15595
    SMSG_GHOSTEE_GONE                                     = 0x1327,
    CMSG_GM_UPDATE_TICKET_STATUS                          = 0x1328,
    SMSG_GM_TICKET_STATUS_UPDATE                          = 0x2C25, // 4.3.4 15595
    MSG_SET_DUNGEON_DIFFICULTY                            = 0x4925, // 4.3.4 15595
    CMSG_GMSURVEY_SUBMIT                                  = 0x2724, // 4.3.4 15595
    SMSG_UPDATE_INSTANCE_OWNERSHIP                        = 0x4915, // 4.3.4 15595
    CMSG_IGNORE_KNOCKBACK_CHEAT                           = 0x132D,
    SMSG_CHAT_PLAYER_AMBIGUOUS                            = 0x2F34, // 4.3.4 15595
    MSG_DELAY_GHOST_TELEPORT                              = 0x132F,
    SMSG_SPELLINSTAKILLLOG                                = 0x6216, // 4.3.4 15595
    SMSG_SPELL_UPDATE_CHAIN_TARGETS                       = 0x6006, // 4.3.4 15595
    CMSG_CHAT_FILTERED                                    = 0x0946, // 4.3.4 15595
    SMSG_EXPECTED_SPAM_RECORDS                            = 0x4D36, // 4.3.4 15595
    SMSG_SPELLSTEALLOG                                    = 0x4E26, // 4.3.4 15595
    CMSG_LOTTERY_QUERY_OBSOLETE                           = 0x1335,
    SMSG_LOTTERY_QUERY_RESULT_OBSOLETE                    = 0x1336,
    CMSG_BUY_LOTTERY_TICKET_OBSOLETE                      = 0x1337,
    SMSG_LOTTERY_RESULT_OBSOLETE                          = 0x1338,
    SMSG_CHARACTER_PROFILE                                = 0x1339,
    SMSG_CHARACTER_PROFILE_REALM_CONNECTED                = 0x133A,
    SMSG_DEFENSE_MESSAGE                                  = 0x0314, // 4.3.4 15595
    SMSG_WORLD_SERVER_INFO                                = 0x31A2, // 4.3.4 15595
    MSG_GM_RESETINSTANCELIMIT                             = 0x133D,
    SMSG_MOTD                                             = 0x0A35, // 4.3.4 15595
    SMSG_MOVE_SET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY     = 0x133F,
    SMSG_MOVE_UNSET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY   = 0x1340,
    CMSG_MOVE_SET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY_ACK = 0x1341,
    MSG_MOVE_START_SWIM_CHEAT                             = 0x1342,
    MSG_MOVE_STOP_SWIM_CHEAT                              = 0x1343,
    SMSG_MOVE_SET_CAN_FLY                                 = 0x3DA1, // 4.3.4 15595
    SMSG_MOVE_UNSET_CAN_FLY                               = 0x15A2, // 4.3.4 15595
    CMSG_MOVE_SET_CAN_FLY                                 = 0x720E, // 4.3.4 15595
    CMSG_MOVE_SET_CAN_FLY_ACK                             = 0x790C, // 4.3.4 15595
    CMSG_MOVE_SET_FLY                                     = 0x1347,
    CMSG_SOCKET_GEMS                                      = 0x2F04, // 4.3.4 15595
    CMSG_ARENA_TEAM_CREATE                                = 0x04A1, // 4.3.4 15595
    SMSG_ARENA_TEAM_COMMAND_RESULT                        = 0x39B3, // 4.3.4 15595
    MSG_MOVE_UPDATE_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY   = 0x134B,
    CMSG_ARENA_TEAM_QUERY                                 = 0x0514, // 4.3.4 15595
    SMSG_ARENA_TEAM_QUERY_RESPONSE                        = 0x6336, // 4.3.4 15595
    CMSG_ARENA_TEAM_ROSTER                                = 0x6F37, // 4.3.4 15595
    SMSG_ARENA_TEAM_ROSTER                                = 0x2717, // 4.3.4 15595
    CMSG_ARENA_TEAM_INVITE                                = 0x2F27, // 4.3.4 15595
    SMSG_ARENA_TEAM_INVITE                                = 0x0F36, // 4.3.4 15595
    CMSG_ARENA_TEAM_ACCEPT                                = 0x2A25, // 4.3.4 15595
    CMSG_ARENA_TEAM_DECLINE                               = 0x6925, // 4.3.4 15595
    CMSG_ARENA_TEAM_LEAVE                                 = 0x0E16, // 4.3.4 15595
    CMSG_ARENA_TEAM_REMOVE                                = 0x2F05, // 4.3.4 15595
    CMSG_ARENA_TEAM_DISBAND                               = 0x6504, // 4.3.4 15595
    CMSG_ARENA_TEAM_LEADER                                = 0x4204, // 4.3.4 15595
    SMSG_ARENA_TEAM_EVENT                                 = 0x0617, // 4.3.4 15595
    CMSG_BATTLEMASTER_JOIN_ARENA                          = 0x701C, // 4.3.4 15595
    CMSG_MOVE_START_ASCEND                                = 0x390A,
    CMSG_MOVE_STOP_ASCEND                                 = 0x7B00, // 4.3.4 15595
    SMSG_ARENA_TEAM_STATS                                 = 0x4425, // 4.3.4 15595
    CMSG_LFG_JOIN                                         = 0x2430, // 4.3.4 15595
    CMSG_LFG_LEAVE                                        = 0x135E,
    CMSG_LFG_SEARCH_JOIN                                  = 0x135F,
    CMSG_LFG_SEARCH_LEAVE                                 = 0x1360,
    SMSG_LFG_SEARCH_RESULTS                               = 0x1361,
    SMSG_LFG_PROPOSAL_UPDATE                              = 0x1362,
    CMSG_LFG_PROPOSAL_RESPONSE                            = 0x1363,
    SMSG_LFG_ROLE_CHECK_UPDATE                            = 0x1364,
    SMSG_LFG_JOIN_RESULT                                  = 0x1365,
    SMSG_LFG_QUEUE_STATUS                                 = 0x1366,
    CMSG_SET_LFG_COMMENT                                  = 0x0530, // 4.3.4 15595
    SMSG_LFG_UPDATE_PLAYER                                = 0x1368,
    SMSG_LFG_UPDATE_PARTY                                 = 0x1369,
    SMSG_LFG_UPDATE_SEARCH                                = 0x136A,
    CMSG_LFG_SET_ROLES                                    = 0x136B,
    CMSG_LFG_SET_NEEDS                                    = 0x136C,
    CMSG_LFG_BOOT_PLAYER_VOTE                             = 0x136D,
    SMSG_LFG_BOOT_PLAYER                                  = 0x136E,
    CMSG_LFG_GET_PLAYER_INFO                              = 0x136F,
    SMSG_LFG_PLAYER_INFO                                  = 0x1370,
    CMSG_LFG_TELEPORT                                     = 0x1371,
    CMSG_LFG_GET_PARTY_INFO                               = 0x1372,
    SMSG_LFG_PARTY_INFO                                   = 0x1373,
    SMSG_TITLE_EARNED                                     = 0x2426, // 4.3.4 15595
    CMSG_SET_TITLE                                        = 0x2117, // 4.3.4 15595
    CMSG_CANCEL_MOUNT_AURA                                = 0x0635, // 4.3.4 15595
    SMSG_ARENA_ERROR                                      = 0x2D17, // 4.3.4 15595
    MSG_INSPECT_ARENA_TEAMS                               = 0x2704, // 4.3.4 15595
    SMSG_DEATH_RELEASE_LOC                                = 0x2F07, // 4.3.4 15595
    CMSG_CANCEL_TEMP_ENCHANTMENT                          = 0x6C37, // 4.3.4 15595
    SMSG_FORCED_DEATH_UPDATE                              = 0x2606, // 4.3.4 15595
    CMSG_CHEAT_SET_HONOR_CURRENCY                         = 0x137C,
    CMSG_CHEAT_SET_ARENA_CURRENCY                         = 0x137D,
    MSG_MOVE_SET_FLIGHT_SPEED_CHEAT                       = 0x137E,
    SMSG_MOVE_SET_FLIGHT_SPEED                            = 0x71A6, // 4.3.4 15595
    MSG_MOVE_SET_FLIGHT_BACK_SPEED_CHEAT                  = 0x1380,
    SMSG_MOVE_SET_FLIGHT_BACK_SPEED                       = 0x30A2, // 4.3.4 15595
    SMSG_FORCE_FLIGHT_SPEED_CHANGE                        = 0x1382,
    CMSG_FORCE_FLIGHT_SPEED_CHANGE_ACK                    = 0x7314, // 4.3.4 15595
    SMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE                   = 0x1384,
    CMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK               = 0x1385,
    SMSG_SPLINE_SET_FLIGHT_SPEED                          = 0x1386,
    SMSG_SPLINE_SET_FLIGHT_BACK_SPEED                     = 0x1387,
    CMSG_MAELSTROM_INVALIDATE_CACHE                       = 0x1388,
    SMSG_FLIGHT_SPLINE_SYNC                               = 0x1389,
    CMSG_SET_TAXI_BENCHMARK_MODE                          = 0x4314, // 4.3.4 15595
    SMSG_JOINED_BATTLEGROUND_QUEUE                        = 0x138B,
    SMSG_REALM_SPLIT                                      = 0x2714, // 4.3.4 15595
    CMSG_REALM_SPLIT                                      = 0x2906, // 4.3.4 15595
    CMSG_MOVE_CHNG_TRANSPORT                              = 0x3102, // 4.3.4 15595
    MSG_PARTY_ASSIGNMENT                                  = 0x0424, // 4.3.4 15595
    SMSG_OFFER_PETITION_ERROR                             = 0x2716, // 4.3.4 15595
    SMSG_TIME_SYNC_REQ                                    = 0x3CA4, // 4.3.4 15595
    CMSG_TIME_SYNC_RESP                                   = 0x3B0C, // 4.3.4 15595
    CMSG_SEND_LOCAL_EVENT                                 = 0x1393,
    CMSG_SEND_GENERAL_TRIGGER                             = 0x1394,
    CMSG_SEND_COMBAT_TRIGGER                              = 0x1395,
    CMSG_MAELSTROM_GM_SENT_MAIL                           = 0x1396,
    SMSG_RESET_FAILED_NOTIFY                              = 0x4616, // 4.3.4 15595
    SMSG_REAL_GROUP_UPDATE                                = 0x0F34, // 4.3.4 15595
    SMSG_LFG_DISABLED                                     = 0x0815, // 4.3.4 15595
    CMSG_ACTIVE_PVP_CHEAT                                 = 0x139A,
    CMSG_CHEAT_DUMP_ITEMS_DEBUG_ONLY                      = 0x139B,
    SMSG_CHEAT_DUMP_ITEMS_DEBUG_ONLY_RESPONSE             = 0x139C,
    SMSG_CHEAT_DUMP_ITEMS_DEBUG_ONLY_RESPONSE_WRITE_FILE  = 0x139D,
    SMSG_UPDATE_COMBO_POINTS                              = 0x6B34, // 4.3.4 15595
    SMSG_VOICE_SESSION_ROSTER_UPDATE                      = 0x139F,
    SMSG_VOICE_SESSION_LEAVE                              = 0x13A0,
    SMSG_VOICE_SESSION_ADJUST_PRIORITY                    = 0x13A1,
    CMSG_VOICE_SET_TALKER_MUTED_REQUEST                   = 0x13A2,
    SMSG_VOICE_SET_TALKER_MUTED                           = 0x13A3,
    SMSG_INIT_EXTRA_AURA_INFO_OBSOLETE                    = 0x13A4,
    SMSG_SET_EXTRA_AURA_INFO_OBSOLETE                     = 0x13A5,
    SMSG_SET_EXTRA_AURA_INFO_NEED_UPDATE_OBSOLETE         = 0x13A6,
    SMSG_CLEAR_EXTRA_AURA_INFO_OBSOLETE                   = 0x13A7,
    CMSG_MOVE_START_DESCEND                               = 0x3800, // 4.3.4 15595
    CMSG_IGNORE_REQUIREMENTS_CHEAT                        = 0x13A9,
    SMSG_IGNORE_REQUIREMENTS_CHEAT                        = 0x13AA,
    SMSG_SPELL_CHANCE_PROC_LOG                            = 0x13AB,
    CMSG_MOVE_SET_RUN_SPEED                               = 0x13AC,
    SMSG_DISMOUNT                                         = 0x2135, // 4.3.4 15595
    SMSG_MOVE_UPDATE_CAN_FLY                              = 0x3DA1, // 4.3.4 15595
    MSG_RAID_READY_CHECK_CONFIRM                          = 0x4F05, // 4.3.4 15595
    CMSG_VOICE_SESSION_ENABLE                             = 0x2314, // 4.3.4 15595
    SMSG_VOICE_SESSION_ENABLE                             = 0x13B1,
    SMSG_VOICE_PARENTAL_CONTROLS                          = 0x13B2,
    CMSG_GM_WHISPER                                       = 0x13B3,
    SMSG_GM_MESSAGECHAT                                   = 0x6434, // 4.3.4 15595
    MSG_GM_GEARRATING                                     = 0x13B5,
    CMSG_COMMENTATOR_ENABLE                               = 0x0B07, // 4.3.4 15595
    SMSG_COMMENTATOR_STATE_CHANGED                        = 0x0737, // 4.3.4 15595
    CMSG_COMMENTATOR_GET_MAP_INFO                         = 0x0026, // 4.3.4 15595
    SMSG_COMMENTATOR_MAP_INFO                             = 0x0327, // 4.3.4 15595
    CMSG_COMMENTATOR_GET_PLAYER_INFO                      = 0x0D14, // 4.3.4 15595
    SMSG_COMMENTATOR_GET_PLAYER_INFO                      = 0x13BB,
    SMSG_COMMENTATOR_PLAYER_INFO                          = 0x2F36, // 4.3.4 15595
    CMSG_COMMENTATOR_ENTER_INSTANCE                       = 0x4105, // 4.3.4 15595
    CMSG_COMMENTATOR_EXIT_INSTANCE                        = 0x6136, // 4.3.4 15595
    CMSG_COMMENTATOR_INSTANCE_COMMAND                     = 0x0917, // 4.3.4 15595
    SMSG_CLEAR_TARGET                                     = 0x4B26, // 4.3.4 15595
    CMSG_BOT_DETECTED                                     = 0x13C1,
    SMSG_CROSSED_INEBRIATION_THRESHOLD                    = 0x2036, // 4.3.4 15595
    CMSG_CHEAT_PLAYER_LOGIN                               = 0x13C3,
    CMSG_CHEAT_PLAYER_LOOKUP                              = 0x13C4,
    SMSG_CHEAT_PLAYER_LOOKUP                              = 0x13C5,
    SMSG_KICK_REASON                                      = 0x13C6,
    MSG_RAID_READY_CHECK_FINISHED                         = 0x2E15, // 4.3.4 15595
    CMSG_COMPLAIN                                         = 0x0427, // 4.3.4 15595
    SMSG_COMPLAIN_RESULT                                  = 0x6D24, // 4.3.4 15595
    SMSG_FEATURE_SYSTEM_STATUS                            = 0x3DB7, // 4.3.4 15595
    CMSG_GM_SHOW_COMPLAINTS                               = 0x13CB,
    CMSG_GM_UNSQUELCH                                     = 0x13CC,
    CMSG_CHANNEL_SILENCE_VOICE                            = 0x2D54, // 4.3.4 15595
    CMSG_CHANNEL_SILENCE_ALL                              = 0x2154, // 4.3.4 15595
    CMSG_CHANNEL_UNSILENCE_VOICE                          = 0x3146, // 4.3.4 15595
    CMSG_CHANNEL_UNSILENCE_ALL                            = 0x2546, // 4.3.4 15595
    CMSG_TARGET_CAST                                      = 0x13D1,
    CMSG_TARGET_SCRIPT_CAST                               = 0x13D2,
    CMSG_CHANNEL_DISPLAY_LIST                             = 0x2144, // 4.3.4 15595
    CMSG_SET_ACTIVE_VOICE_CHANNEL                         = 0x4305, // 4.3.4 15595
    CMSG_GET_CHANNEL_MEMBER_COUNT                         = 0x13D5,
    SMSG_CHANNEL_MEMBER_COUNT                             = 0x6414, // 4.3.4 15595
    CMSG_CHANNEL_VOICE_ON                                 = 0x1144, // 4.3.4 15595
    CMSG_CHANNEL_VOICE_OFF                                = 0x13D8,
    CMSG_DEBUG_LIST_TARGETS                               = 0x13D9,
    SMSG_DEBUG_LIST_TARGETS                               = 0x13DA,
    SMSG_AVAILABLE_VOICE_CHANNEL                          = 0x13DB,
    CMSG_ADD_VOICE_IGNORE                                 = 0x13DC,
    CMSG_DEL_VOICE_IGNORE                                 = 0x13DD,
    CMSG_PARTY_SILENCE                                    = 0x6B26, // 4.3.4 15595
    CMSG_PARTY_UNSILENCE                                  = 0x4D24, // 4.3.4 15595
    MSG_NOTIFY_PARTY_SQUELCH                              = 0x4D06, // 4.3.4 15595
    SMSG_COMSAT_RECONNECT_TRY                             = 0x4D35, // 4.3.4 15595
    SMSG_COMSAT_DISCONNECT                                = 0x0316, // 4.3.4 15595
    SMSG_COMSAT_CONNECT_FAIL                              = 0x6317, // 4.3.4 15595
    SMSG_VOICE_CHAT_STATUS                                = 0x0F15, // 4.3.4 15595
    CMSG_REPORT_PVP_AFK                                   = 0x6734, // 4.3.4 15595
    SMSG_REPORT_PVP_AFK_RESULT                            = 0x2D06, // 4.3.4 15595
    CMSG_GUILD_BANKER_ACTIVATE                            = 0x2E37, // 4.3.4 15595
    CMSG_GUILD_BANK_QUERY_TAB                             = 0x2E35, // 4.3.4 15595
    SMSG_GUILD_BANK_LIST                                  = 0x78A5, // 4.3.4 15595
    CMSG_GUILD_BANK_SWAP_ITEMS                            = 0x2315, // 4.3.4 15595
    CMSG_GUILD_BANK_BUY_TAB                               = 0x0C37, // 4.3.4 15595
    CMSG_GUILD_BANK_UPDATE_TAB                            = 0x0106, // 4.3.4 15595
    CMSG_GUILD_BANK_DEPOSIT_MONEY                         = 0x0707, // 4.3.4 15595
    CMSG_GUILD_BANK_WITHDRAW_MONEY                        = 0x0037, // 4.3.4 15595
    CMSG_GUILD_BANK_LOG_QUERY                             = 0x3224, // 4.3.4 15595
    SMSG_GUILD_BANK_LOG_QUERY_RESULT                      = 0x30B2, // 4.3.4 15595
    CMSG_SET_CHANNEL_WATCH                                = 0x4517, // 4.3.4 15595
    SMSG_USERLIST_ADD                                     = 0x0F37, // 4.3.4 15595
    SMSG_USERLIST_REMOVE                                  = 0x2006, // 4.3.4 15595
    SMSG_USERLIST_UPDATE                                  = 0x0135, // 4.3.4 15595
    CMSG_CLEAR_CHANNEL_WATCH                              = 0x2604, // 4.3.4 15595
    SMSG_INSPECT_RESULTS                                  = 0x4014, // 4.3.4 15595
    SMSG_GOGOGO_OBSOLETE                                  = 0x13F6,
    SMSG_ECHO_PARTY_SQUELCH                               = 0x0814, // 4.3.4 15595
    CMSG_SET_TITLE_SUFFIX                                 = 0x13F8,
    CMSG_SPELLCLICK                                       = 0x0805, // 4.3.4 15595
    SMSG_LOOT_LIST                                        = 0x6807, // 4.3.4 15595
    CMSG_GM_CHARACTER_RESTORE                             = 0x13FB,
    CMSG_GM_CHARACTER_SAVE                                = 0x13FC,
    SMSG_VOICESESSION_FULL                                = 0x6225, // 4.3.4 15595
    CMSG_GUILD_PERMISSIONS                                = 0x3022, // 4.3.4 15595
    SMSG_GUILD_PERMISSIONS                                = 0x34A3, // 4.3.4 15595
    CMSG_GUILD_BANK_MONEY_WITHDRAWN                       = 0x1225, // 4.3.4 15595
    SMSG_GUILD_BANK_MONEY_WITHDRAWN                       = 0x5DB4, // 4.3.4 15595
    CMSG_GUILD_EVENT_LOG_QUERY                            = 0x1220, // 4.3.4 15595
    SMSG_GUILD_EVENT_LOG                                  = 0x10B2, // 4.3.4 15595
    CMSG_MAELSTROM_RENAME_GUILD                           = 0x1401,
    CMSG_GET_MIRRORIMAGE_DATA                             = 0x0C25, // 4.3.4 15595
    SMSG_MIRRORIMAGE_DATA                                 = 0x2634, // 4.3.4 15595
    SMSG_FORCE_DISPLAY_UPDATE                             = 0x1404,
    SMSG_SPELL_CHANCE_RESIST_PUSHBACK                     = 0x1405,
    CMSG_IGNORE_DIMINISHING_RETURNS_CHEAT                 = 0x1406,
    SMSG_IGNORE_DIMINISHING_RETURNS_CHEAT                 = 0x0125, // 4.3.4 15595
    CMSG_KEEP_ALIVE                                       = 0x0015, // 4.3.4 15595
    SMSG_RAID_READY_CHECK_ERROR                           = 0x1409,
    CMSG_OPT_OUT_OF_LOOT                                  = 0x6B16, // 4.3.4 15595
    CMSG_QUERY_GUILD_BANK_TEXT                            = 0x3220, // 4.3.4 15595
    SMSG_GUILD_BANK_TEXT                                  = 0x75A3, // 4.3.4 15595
    CMSG_SET_GUILD_BANK_TEXT                              = 0x3023, // 4.3.4 15595
    CMSG_SET_GRANTABLE_LEVELS                             = 0x140D,
    CMSG_GRANT_LEVEL                                      = 0x6D16, // 4.3.4 15595
    CMSG_REFER_A_FRIEND                                   = 0x140F,
    MSG_GM_CHANGE_ARENA_RATING                            = 0x1410,
    CMSG_DECLINE_CHANNEL_INVITE                           = 0x1411,
    SMSG_GROUPACTION_THROTTLED                            = 0x6524, // 4.3.4 15595
    SMSG_OVERRIDE_LIGHT                                   = 0x4225, // 4.3.4 15595
    SMSG_TOTEM_CREATED                                    = 0x2414, // 4.3.4 15595
    CMSG_TOTEM_DESTROYED                                  = 0x4207, // 4.3.4 15595
    CMSG_EXPIRE_RAID_INSTANCE                             = 0x1416,
    CMSG_NO_SPELL_VARIANCE                                = 0x1417,
    CMSG_QUESTGIVER_STATUS_MULTIPLE_QUERY                 = 0x6305, // 4.3.4 15595
    SMSG_QUESTGIVER_STATUS_MULTIPLE                       = 0x4F25, // 4.3.4 15595
    CMSG_SET_PLAYER_DECLINED_NAMES                        = 0x6316, // 4.3.4 15595
    SMSG_SET_PLAYER_DECLINED_NAMES_RESULT                 = 0x2B25, // 4.3.4 15595
    CMSG_QUERY_SERVER_BUCK_DATA                           = 0x141C,
    CMSG_CLEAR_SERVER_BUCK_DATA                           = 0x141D,
    SMSG_SERVER_BUCK_DATA                                 = 0x141E,
    SMSG_SEND_UNLEARN_SPELLS                              = 0x4E25, // 4.3.4 15595
    SMSG_PROPOSE_LEVEL_GRANT                              = 0x6114, // 4.3.4 15595
    CMSG_ACCEPT_LEVEL_GRANT                               = 0x0205, // 4.3.4 15595
    SMSG_REFER_A_FRIEND_FAILURE                           = 0x2037, // 4.3.4 15595
    SMSG_SPLINE_MOVE_SET_FLYING                           = 0x31B5, // 4.3.4 15595
    SMSG_SPLINE_MOVE_UNSET_FLYING                         = 0x58A6, // 4.3.4 15595
    SMSG_SUMMON_CANCEL                                    = 0x0B34, // 4.3.4 15595
    CMSG_CHANGE_PERSONAL_ARENA_RATING                     = 0x1426,
    CMSG_ALTER_APPEARANCE                                 = 0x0914, // 4.3.4 15595
    SMSG_ENABLE_BARBER_SHOP                               = 0x2D16, // 4.3.4 15595
    SMSG_BARBER_SHOP_RESULT                               = 0x6125, // 4.3.4 15595
    CMSG_CALENDAR_GET_CALENDAR                            = 0x2814, // 4.3.4 15595
    CMSG_CALENDAR_GET_EVENT                               = 0x142B,
    CMSG_CALENDAR_GUILD_FILTER                            = 0x142C,
    CMSG_CALENDAR_ARENA_TEAM                              = 0x142D,
    CMSG_CALENDAR_ADD_EVENT                               = 0x0726, // 4.3.4 15595
    CMSG_CALENDAR_UPDATE_EVENT                            = 0x142F,
    CMSG_CALENDAR_REMOVE_EVENT                            = 0x1430,
    CMSG_CALENDAR_COPY_EVENT                              = 0x1431,
    CMSG_CALENDAR_EVENT_INVITE                            = 0x2435, // 4.3.4 15595
    CMSG_CALENDAR_EVENT_RSVP                              = 0x1433,
    CMSG_CALENDAR_EVENT_REMOVE_INVITE                     = 0x1434,
    CMSG_CALENDAR_EVENT_STATUS                            = 0x1435,
    CMSG_CALENDAR_EVENT_MODERATOR_STATUS                  = 0x1436,
    SMSG_CALENDAR_SEND_CALENDAR                           = 0x6805, // 4.3.4 15595
    SMSG_CALENDAR_SEND_EVENT                              = 0x1438, // 4.3.4 15595
    SMSG_CALENDAR_FILTER_GUILD                            = 0x1439,
    SMSG_CALENDAR_ARENA_TEAM                              = 0x143A,
    SMSG_CALENDAR_EVENT_INVITE                            = 0x143B,
    SMSG_CALENDAR_EVENT_INVITE_REMOVED                    = 0x143C,
    SMSG_CALENDAR_EVENT_STATUS                            = 0x143D,
    SMSG_CALENDAR_COMMAND_RESULT                          = 0x143E,
    SMSG_CALENDAR_RAID_LOCKOUT_ADDED                      = 0x143F,
    SMSG_CALENDAR_RAID_LOCKOUT_REMOVED                    = 0x1440,
    SMSG_CALENDAR_EVENT_INVITE_ALERT                      = 0x1441, // 4.3.4 15595
    SMSG_CALENDAR_EVENT_INVITE_REMOVED_ALERT              = 0x1442,
    SMSG_CALENDAR_EVENT_INVITE_STATUS_ALERT               = 0x1443,
    SMSG_CALENDAR_EVENT_REMOVED_ALERT                     = 0x1444,
    SMSG_CALENDAR_EVENT_UPDATED_ALERT                     = 0x1445,
    SMSG_CALENDAR_EVENT_MODERATOR_STATUS_ALERT            = 0x1446,
    CMSG_CALENDAR_COMPLAIN                                = 0x1447,
    CMSG_CALENDAR_GET_NUM_PENDING                         = 0x4D05, // 4.3.4 15595
    SMSG_CALENDAR_SEND_NUM_PENDING                        = 0x0C17, // 4.3.4 15595
    CMSG_SAVE_DANCE                                       = 0x144A,
    SMSG_NOTIFY_DANCE                                     = 0x4904, // 4.3.4 15595
    CMSG_PLAY_DANCE                                       = 0x6914, // 4.3.4 15595
    SMSG_PLAY_DANCE                                       = 0x4704, // 4.3.4 15595
    CMSG_LOAD_DANCES                                      = 0x144E,
    CMSG_STOP_DANCE                                       = 0x2907, // 4.3.4 15595
    SMSG_STOP_DANCE                                       = 0x4637, // 4.3.4 15595
    CMSG_SYNC_DANCE                                       = 0x0036, // 4.3.4 15595
    CMSG_DANCE_QUERY                                      = 0x4E07, // 4.3.4 15595
    SMSG_DANCE_QUERY_RESPONSE                             = 0x2F06, // 4.3.4 15595
    SMSG_INVALIDATE_DANCE                                 = 0x0E27, // 4.3.4 15595
    CMSG_DELETE_DANCE                                     = 0x1455,
    SMSG_LEARNED_DANCE_MOVES                              = 0x0E05, // 4.3.4 15595
    CMSG_LEARN_DANCE_MOVE                                 = 0x1457,
    CMSG_UNLEARN_DANCE_MOVE                               = 0x1458,
    CMSG_SET_RUNE_COUNT                                   = 0x1459,
    CMSG_SET_RUNE_COOLDOWN                                = 0x145A,
    MSG_MOVE_SET_PITCH_RATE_CHEAT                         = 0x145B,
    SMSG_MOVE_SET_PITCH_RATE                              = 0x75B0, // 4.3.4 15595
    SMSG_FORCE_PITCH_RATE_CHANGE                          = 0x145D,
    CMSG_FORCE_PITCH_RATE_CHANGE_ACK                      = 0x145E,
    SMSG_SPLINE_SET_PITCH_RATE                            = 0x145F,
    CMSG_CALENDAR_EVENT_INVITE_NOTES                      = 0x1460,
    SMSG_CALENDAR_EVENT_INVITE_NOTES                      = 0x1461,
    SMSG_CALENDAR_EVENT_INVITE_NOTES_ALERT                = 0x1462,
    CMSG_UPDATE_MISSILE_TRAJECTORY                        = 0x781E, // 4.3.4 15595
    SMSG_UPDATE_ACCOUNT_DATA_COMPLETE                     = 0x2015, // 4.3.4 15595
    SMSG_TRIGGER_MOVIE                                    = 0x4625, // 4.3.4 15595
    CMSG_COMPLETE_MOVIE                                   = 0x4136, // 4.3.4 15595
    CMSG_SET_GLYPH_SLOT                                   = 0x1467,
    CMSG_SET_GLYPH                                        = 0x1468,
    SMSG_ACHIEVEMENT_EARNED                               = 0x4405, // 4.3.4 15595
    SMSG_DYNAMIC_DROP_ROLL_RESULT                         = 0x146A,
    SMSG_CRITERIA_UPDATE                                  = 0x6E37, // 4.3.4 15595
    CMSG_QUERY_INSPECT_ACHIEVEMENTS                       = 0x4D27, // 4.3.4 15595
    SMSG_RESPOND_INSPECT_ACHIEVEMENTS                     = 0x15B0, // 4.3.4 15595
    CMSG_DISMISS_CONTROLLED_VEHICLE                       = 0x3218, // 4.3.4 15595
    CMSG_COMPLETE_ACHIEVEMENT_CHEAT                       = 0x146F,
    SMSG_QUESTUPDATE_ADD_PVP_KILL                         = 0x4416, // 4.3.4 15595
    CMSG_SET_CRITERIA_CHEAT                               = 0x1471,
    SMSG_CALENDAR_RAID_LOCKOUT_UPDATED                    = 0x1472,
    CMSG_UNITANIMTIER_CHEAT                               = 0x1473,
    CMSG_CHAR_CUSTOMIZE                                   = 0x2C34, // 4.3.4 15595
    SMSG_CHAR_CUSTOMIZE                                   = 0x4F16, // 4.3.4 15595
    SMSG_PET_RENAMEABLE                                   = 0x2B27, // 4.3.4 15595
    CMSG_REQUEST_VEHICLE_EXIT                             = 0x2B35, // 4.3.4 15595
    CMSG_REQUEST_VEHICLE_PREV_SEAT                        = 0x4C04, // 4.3.4 15595
    CMSG_REQUEST_VEHICLE_NEXT_SEAT                        = 0x4434, // 4.3.4 15595
    CMSG_REQUEST_VEHICLE_SWITCH_SEAT                      = 0x4C14, // 4.3.4 15595
    CMSG_PET_LEARN_TALENT                                 = 0x6725, // 4.3.4 15595
    CMSG_PET_UNLEARN_TALENTS                              = 0x147C,
    SMSG_SET_PHASE_SHIFT                                  = 0x70A0, // 4.3.4 15595
    SMSG_ALL_ACHIEVEMENT_DATA                             = 0x58B1, // 4.3.4 15595
    CMSG_FORCE_SAY_CHEAT                                  = 0x147F,
    SMSG_HEALTH_UPDATE                                    = 0x4734, // 4.3.4 15595
    SMSG_POWER_UPDATE                                     = 0x4A07, // 4.3.4 15595
    CMSG_GAMEOBJ_REPORT_USE                               = 0x4827, // 4.3.4 15595
    SMSG_HIGHEST_THREAT_UPDATE                            = 0x4104, // 4.3.4 15595
    SMSG_THREAT_UPDATE                                    = 0x4735, // 4.3.4 15595
    SMSG_THREAT_REMOVE                                    = 0x2E05, // 4.3.4 15595
    SMSG_THREAT_CLEAR                                     = 0x6437, // 4.3.4 15595
    SMSG_CONVERT_RUNE                                     = 0x4F14, // 4.3.4 15595
    SMSG_RESYNC_RUNES                                     = 0x6224, // 4.3.4 15595
    SMSG_ADD_RUNE_POWER                                   = 0x6915, // 4.3.4 15595
    CMSG_START_QUEST                                      = 0x148A,
    CMSG_REMOVE_GLYPH                                     = 0x148B,
    CMSG_DUMP_OBJECTS                                     = 0x148C,
    SMSG_DUMP_OBJECTS_DATA                                = 0x148D,
    CMSG_DISMISS_CRITTER                                  = 0x4227, // 4.3.4 15595
    SMSG_NOTIFY_DEST_LOC_SPELL_CAST                       = 0x148F,
    CMSG_AUCTION_LIST_PENDING_SALES                       = 0x2C17, // 4.3.4 15595
    SMSG_AUCTION_LIST_PENDING_SALES                       = 0x6A27, // 4.3.4 15595
    SMSG_MODIFY_COOLDOWN                                  = 0x6016, // 4.3.4 15595
    SMSG_PET_UPDATE_COMBO_POINTS                          = 0x4325, // 4.3.4 15595
    CMSG_ENABLETAXI                                       = 0x0C16, // 4.3.4 15595
    SMSG_PRE_RESURRECT                                    = 0x6C36, // 4.3.4 15595
    SMSG_AURA_UPDATE_ALL                                  = 0x6916, // 4.3.4 15595
    SMSG_AURA_UPDATE                                      = 0x4707, // 4.3.4 15595
    CMSG_FLOOD_GRACE_CHEAT                                = 0x1498,
    SMSG_SERVER_FIRST_ACHIEVEMENT                         = 0x6424, // 4.3.4 15595
    SMSG_PET_LEARNED_SPELL                                = 0x0507, // 4.3.4 15595
    SMSG_PET_REMOVED_SPELL                                = 0x6A04, // 4.3.4 15595
    CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE               = 0x7310, // 4.3.4 15595
    CMSG_HEARTH_AND_RESURRECT                             = 0x4B34, // 4.3.4 15595
    SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA             = 0x4D34, // 4.3.4 15595
    SMSG_CRITERIA_DELETED                                 = 0x2915, // 4.3.4 15595
    SMSG_ACHIEVEMENT_DELETED                              = 0x6A16, // 4.3.4 15595
    CMSG_SERVER_INFO_QUERY                                = 0x14A1,
    SMSG_SERVER_INFO_RESPONSE                             = 0x14A2,
    CMSG_CHECK_LOGIN_CRITERIA                             = 0x14A3,
    SMSG_SERVER_BUCK_DATA_START                           = 0x14A4,
    CMSG_SET_BREATH                                       = 0x14A5,
    CMSG_QUERY_VEHICLE_STATUS                             = 0x14A6,
    SMSG_BATTLEGROUND_INFO_THROTTLED                      = 0x14A7,
    SMSG_SET_VEHICLE_REC_ID                               = 0x4115, // 4.3.4 15595
    CMSG_RIDE_VEHICLE_INTERACT                            = 0x2705, // 4.3.4 15595
    CMSG_CONTROLLER_EJECT_PASSENGER                       = 0x6927, // 4.3.4 15595
    SMSG_PET_GUIDS                                        = 0x2D26, // 4.3.4 15595
    SMSG_CLIENTCACHE_VERSION                              = 0x2734, // 4.3.4 15595
    CMSG_CHANGE_GDF_ARENA_RATING                          = 0x14AD,
    CMSG_SET_ARENA_TEAM_RATING_BY_INDEX                   = 0x14AE,
    CMSG_SET_ARENA_TEAM_WEEKLY_GAMES                      = 0x14AF,
    CMSG_SET_ARENA_TEAM_SEASON_GAMES                      = 0x14B0,
    CMSG_SET_ARENA_MEMBER_WEEKLY_GAMES                    = 0x14B1,
    CMSG_SET_ARENA_MEMBER_SEASON_GAMES                    = 0x14B2,
    SMSG_SET_ITEM_PURCHASE_DATA                           = 0x15A3, // 4.3.4 15595
    CMSG_GET_ITEM_PURCHASE_DATA                           = 0x2206, // 4.3.4 15595
    CMSG_ITEM_PURCHASE_REFUND                             = 0x6134, // 4.3.4 15595
    SMSG_ITEM_PURCHASE_REFUND_RESULT                      = 0x5DB1, // 4.3.4 15595
    CMSG_CORPSE_TRANSPORT_QUERY                           = 0x6205, // 4.3.4 15595
    SMSG_CORPSE_TRANSPORT_QUERY                           = 0x0E35, // 4.3.4 15595
    CMSG_UNUSED5                                          = 0x14B9,
    CMSG_UNUSED6                                          = 0x14BA,
    CMSG_CALENDAR_EVENT_SIGNUP                            = 0x14BB,
    SMSG_CALENDAR_CLEAR_PENDING_ACTION                    = 0x14BC,
    SMSG_LOAD_EQUIPMENT_SET                               = 0x2E04, // 4.3.4 15595
    CMSG_SAVE_EQUIPMENT_SET                               = 0x4F27, // 4.3.4 15595
    CMSG_ON_MISSILE_TRAJECTORY_COLLISION                  = 0x14BF,
    SMSG_NOTIFY_MISSILE_TRAJECTORY_COLLISION              = 0x14C0,
    SMSG_TALENT_UPDATE                                    = 0x6F26, // 4.3.4 15595
    CMSG_LEARN_TALENT_GROUP                               = 0x2415, // 4.3.4 15595
    CMSG_PET_LEARN_TALENT_GROUP                           = 0x6E24, // 4.3.4 15595
    CMSG_SET_ACTIVE_TALENT_GROUP_OBSOLETE                 = 0x14C4,
    CMSG_GM_GRANT_ACHIEVEMENT                             = 0x14C5,
    CMSG_GM_REMOVE_ACHIEVEMENT                            = 0x14C6,
    CMSG_GM_SET_CRITERIA_FOR_PLAYER                       = 0x14C7,
    SMSG_DESTROY_ARENA_UNIT                               = 0x2637, // 4.3.4 15595
    SMSG_ARENA_TEAM_CHANGE_FAILED                         = 0x6E34, // 4.3.4 15595
    CMSG_PROFILEDATA_REQUEST                              = 0x14CA,
    SMSG_PROFILEDATA_RESPONSE                             = 0x14CB,
    CMSG_START_BATTLEFIELD_CHEAT                          = 0x14CC,
    CMSG_END_BATTLEFIELD_CHEAT                            = 0x14CD,
    SMSG_COMPOUND_MOVE                                    = 0x14CE,
    SMSG_MOVE_GRAVITY_DISABLE                             = 0x75B2, // 4.3.4 15595
    CMSG_MOVE_GRAVITY_DISABLE_ACK                         = 0x3118, // 4.3.4 15595
    SMSG_MOVE_GRAVITY_ENABLE                              = 0x30B3, // 4.3.4 15595
    CMSG_MOVE_GRAVITY_ENABLE_ACK                          = 0x700A, // 4.3.4 15595
    MSG_MOVE_GRAVITY_CHNG                                 = 0x14D3,
    SMSG_SPLINE_MOVE_GRAVITY_DISABLE                      = 0x5DB5, // 4.3.4 15595
    SMSG_SPLINE_MOVE_GRAVITY_ENABLE                       = 0x3CA6, // 4.3.4 15595
    CMSG_USE_EQUIPMENT_SET                                = 0x0417, // 4.3.4 15595
    SMSG_USE_EQUIPMENT_SET_RESULT                         = 0x2424, // 4.3.4 15595
    CMSG_FORCE_ANIM                                       = 0x14D8,
    SMSG_FORCE_ANIM                                       = 0x4C05, // 4.3.4 15595
    CMSG_CHAR_FACTION_CHANGE                              = 0x2735, // 4.3.4 15595
    SMSG_CHAR_FACTION_CHANGE                              = 0x4C06, // 4.3.4 15595
    CMSG_PVP_QUEUE_STATS_REQUEST                          = 0x14DC,
    SMSG_PVP_QUEUE_STATS                                  = 0x14DD,
    CMSG_SET_PAID_SERVICE_CHEAT                           = 0x14DE,
    SMSG_BATTLEFIELD_MANAGER_ENTRY_INVITE                 = 0x14DF,
    CMSG_BATTLEFIELD_MANAGER_ENTRY_INVITE_RESPONSE        = 0x14E0,
    SMSG_BATTLEFIELD_MANAGER_ENTERING                     = 0x14E1,
    SMSG_BATTLEFIELD_MANAGER_QUEUE_INVITE                 = 0x14E2,
    CMSG_BATTLEFIELD_MANAGER_QUEUE_INVITE_RESPONSE        = 0x14E3,
    CMSG_BATTLEFIELD_MANAGER_QUEUE_REQUEST                = 0x710C, // 4.3.4 15595
    SMSG_BATTLEFIELD_MANAGER_QUEUE_REQUEST_RESPONSE       = 0x14E5,
    SMSG_BATTLEFIELD_MANAGER_EJECT_PENDING                = 0x14E6,
    SMSG_BATTLEFIELD_MANAGER_EJECTED                      = 0x14E7,
    CMSG_BATTLEFIELD_MANAGER_EXIT_REQUEST                 = 0x14E8,
    SMSG_BATTLEFIELD_MANAGER_STATE_CHANGED                = 0x14E9,
    CMSG_BATTLEFIELD_MANAGER_ADVANCE_STATE                = 0x14EA,
    CMSG_BATTLEFIELD_MANAGER_SET_NEXT_TRANSITION_TIME     = 0x14EB,
    MSG_SET_RAID_DIFFICULTY                               = 0x0614, // 4.3.4 15595
    CMSG_XPGAIN                                           = 0x14ED,
    SMSG_XPGAIN                                           = 0x14EE,
    SMSG_GMTICKET_RESPONSE_ERROR                          = 0x14EF,
    SMSG_GMTICKET_GET_RESPONSE                            = 0x2E34, // 4.3.4 15595
    CMSG_GMTICKET_RESOLVE_RESPONSE                        = 0x6506, // 4.3.4 15595
    SMSG_GMTICKET_RESOLVE_RESPONSE                        = 0x0A04, // 4.3.4 15595
    SMSG_GMTICKET_CREATE_RESPONSE_TICKET                  = 0x14F3,
    CMSG_GM_CREATE_TICKET_RESPONSE                        = 0x14F4,
    CMSG_SERVERINFO                                       = 0x14F5,
    SMSG_SERVERINFO                                       = 0x14F6,
    CMSG_UI_TIME_REQUEST                                  = 0x4605, // 4.3.4 15595
    SMSG_UI_TIME                                          = 0x4A14, // 4.3.4 15595
    CMSG_CHAR_RACE_CHANGE                                 = 0x0D24, // 4.3.4 15595
    MSG_VIEW_PHASE_SHIFT                                  = 0x14FA,
    SMSG_TALENTS_INVOLUNTARILY_RESET                      = 0x14FB,
    CMSG_DEBUG_SERVER_GEO                                 = 0x14FC,
    SMSG_DEBUG_SERVER_GEO                                 = 0x14FD,
    SMSG_LOOT_UPDATE                                      = 0x14FE,
    UMSG_UPDATE_GROUP_INFO                                = 0x14FF,
    CMSG_READY_FOR_ACCOUNT_DATA_TIMES                     = 0x2B16, // 4.3.4 15595
    CMSG_QUERY_GET_ALL_QUESTS                             = 0x2317, // 4.3.4 15595
    SMSG_ALL_QUESTS_COMPLETED                             = 0x6314, // 4.3.4 15595
    CMSG_GMLAGREPORT_SUBMIT                               = 0x1503,
    CMSG_AFK_MONITOR_INFO_REQUEST                         = 0x1504,
    SMSG_AFK_MONITOR_INFO_RESPONSE                        = 0x1505,
    CMSG_AFK_MONITOR_INFO_CLEAR                           = 0x1506,
    SMSG_AREA_TRIGGER_NO_CORPSE                           = 0x2A14, // 4.3.4 15595
    CMSG_GM_NUKE_CHARACTER                                = 0x1508,
    CMSG_LOW_LEVEL_RAID                                   = 0x4435, // 4.3.4 15595
    CMSG_LOW_LEVEL_RAID_USER                              = 0x0536, // 4.3.4 15595
    SMSG_CAMERA_SHAKE                                     = 0x4214, // 4.3.4 15595
    SMSG_SOCKET_GEMS                                      = 0x6014, // 4.3.4 15595
    CMSG_SET_CHARACTER_MODEL                              = 0x150D,
    SMSG_CONNECT_TO                                       = 0x0942, // 4.3.4 15595
    CMSG_CONNECT_TO_FAILED                                = 0x2533, // 4.3.4 15595
    SMSG_SUSPEND_COMMS                                    = 0x4140, // 4.3.4 15595
    CMSG_SUSPEND_COMMS_ACK                                = 0x1511,
    SMSG_RESUME_COMMS                                     = 0x1512,
    CMSG_AUTH_CONTINUED_SESSION                           = 0x1513,
    CMSG_DROP_NEW_CONNECTION                              = 0x1514,
    SMSG_SEND_ALL_COMBAT_LOG                              = 0x1515,
    SMSG_OPEN_LFG_DUNGEON_FINDER                          = 0x0412, // 4.3.4 15595
    SMSG_MOVE_SET_COLLISION_HGT                           = 0x11B0, // 4.3.4 15595
    CMSG_MOVE_SET_COLLISION_HGT_ACK                       = 0x7114, // 4.3.4 15595
    MSG_MOVE_SET_COLLISION_HGT                            = 0x1519,
    CMSG_CLEAR_RANDOM_BG_WIN_TIME                         = 0x151A,
    CMSG_CLEAR_HOLIDAY_BG_WIN_TIME                        = 0x151B,
    CMSG_COMMENTATOR_SKIRMISH_QUEUE_COMMAND               = 0x0025, // 4.3.4 15595
    SMSG_COMMENTATOR_SKIRMISH_QUEUE_RESULT1               = 0x2126, // 4.3.4 15595
    SMSG_COMMENTATOR_SKIRMISH_QUEUE_RESULT2               = 0x6814, // 4.3.4 15595
    SMSG_COMPRESSED_UNKNOWN_1310                          = 0x151F,
    SMSG_UNKNOWN_1311                                     = 0x1520,
    SMSG_UNKNOWN_1312                                     = 0x1521,
    UMSG_UNKNOWN_1313                                     = 0x1522,
    SMSG_UNKNOWN_1314                                     = 0x1523,
    SMSG_UNKNOWN_1315                                     = 0x1524,
    SMSG_UNKNOWN_1316                                     = 0x1525,
    SMSG_UNKNOWN_1317                                     = 0x1526,
    UMSG_UNKNOWN_1318                                     = 0x1527,
    UMSG_UNKNOWN_1319                                     = 0x1528,
    CMSG_UNKNOWN_1320                                     = 0x1529,
    UMSG_UNKNOWN_1321                                     = 0x152A,
    UMSG_UNKNOWN_1322                                     = 0x152B,
    UMSG_UNKNOWN_1323                                     = 0x152C,
    UMSG_UNKNOWN_1324                                     = 0x152D,
    UMSG_UNKNOWN_1325                                     = 0x152E,
    UMSG_UNKNOWN_1326                                     = 0x152F,
    UMSG_UNKNOWN_1327                                     = 0x1530,
    UMSG_UNKNOWN_1328                                     = 0x1531,
    SMSG_UNKNOWN_1329                                     = 0x1532,
    UMSG_UNKNOWN_1330                                     = 0x1533,
    UMSG_UNKNOWN_1331                                     = 0x1534,
    UMSG_UNKNOWN_1332                                     = 0x1535,
    UMSG_UNKNOWN_1333                                     = 0x1536,
    UMSG_UNKNOWN_1334                                     = 0x1537,
    SMSG_PLAYER_MOVE                                      = 0x79A2, // 4.3.4 15595
    SMSG_SPLINE_MOVE_SET_FLIGHT_BACK_SPEED                = 0x38B3, // 4.3.4 15595
    SMSG_SPLINE_MOVE_SET_FLIGHT_SPEED                     = 0x39A0, // 4.3.4 15595
    SMSG_SPLINE_MOVE_SET_PITCH_RATE                       = 0x14B0, // 4.3.4 15595
    SMSG_SPLINE_MOVE_SET_RUN_BACK_SPEED                   = 0x3DB3, // 4.3.4 15595
    SMSG_SPLINE_MOVE_SET_RUN_SPEED                        = 0x51B7, // 4.3.4 15595
    SMSG_SPLINE_MOVE_SET_SWIM_SPEED                       = 0x39A4, // 4.3.4 15595
    SMSG_SPLINE_MOVE_SET_SWIM_BACK_SPEED                  = 0x59A1, // 4.3.4 15595
    SMSG_SPLINE_MOVE_SET_TURN_RATE                        = 0x78B5, // 4.3.4 15595
    SMSG_SPLINE_MOVE_SET_WALK_SPEED                       = 0x34A5, // 4.3.4 15595
    CMSG_REORDER_CHARACTERS                               = 0x0593, // 4.3.4 15595
    SMSG_SET_CURRENCY_WEEK_LIMIT                          = 0x70A7, // 4.3.4 15595
    SMSG_SET_CURRENCY                                     = 0x59B0, // 4.3.4 15595
    SMSG_SEND_CURRENCIES                                  = 0x15A5, // 4.3.4 15595
    CMSG_SET_CURRENCY_FLAGS                               = 0x7306, // 4.3.4 15595
    SMSG_WEEKLY_RESET_CURRENCIES                          = 0x3CA1, // 4.3.4 15595
    CMSG_INSPECT_RATED_BG_STATS                           = 0x3010, // 4.3.4 15595
    CMSG_REQUEST_RATED_BG_INFO                            = 0x2423, // 4.3.4 15595
    CMSG_REQUEST_RATED_BG_STATS                           = 0x05B3, // 4.3.4 15595
    SMSG_RATED_BG_STATS                                   = 0x34A1, // 4.3.4 15595
    CMSG_REQUEST_PVP_REWARDS                              = 0x780C, // 4.3.4 15595
    SMSG_PVP_REWARDS                                      = 0x5DA4, // 4.3.4 15595
    CMSG_REQUEST_PVP_OPTIONS_ENABLED                      = 0x24A1, // 4.3.4 15595
    SMSG_PVP_OPTIONS_ENABLED                              = 0x50A1, // 4.3.4 15595
    CMSG_REQUEST_HOTFIX                                   = 0x2401, // 4.3.4 15595
    SMSG_DB_REPLY                                         = 0x38A4, // 4.3.4 15595
    CMSG_OBJECT_UPDATE_FAILED                             = 0x3808, // 4.3.4 15595
    CMSG_REFORGE_ITEM                                     = 0x331A, // 4.3.4 15595
    SMSG_REFORGE_RESULT                                   = 0x58A4, // 4.3.4 15595
    CMSG_LOAD_SCREEN                                      = 0x2422, // 4.3.4 15595
    SMSG_START_TIMER                                      = 0x59A5, // 4.3.4 15595
};

// Don't forget to change this value and add opcode name to Opcodes.cpp when you add new opcode!
#define NUM_MSG_TYPES 0xFFFF

extern void InitializeOpcodes();

/// Player state
enum SessionStatus
{
    STATUS_AUTHED = 0,                     ///< Player authenticated (_player==NULL, m_playerRecentlyLogout = false or will be reset before handler call)
    STATUS_LOGGEDIN,                       ///< Player in game (_player!=NULL, inWorld())
    STATUS_TRANSFER,                       ///< Player transferring to another map (_player!=NULL, !inWorld())
    STATUS_LOGGEDIN_OR_RECENTLY_LOGGEDOUT, ///< _player!= NULL or _player==NULL && m_playerRecentlyLogout)
    STATUS_NEVER,                          ///< Opcode not accepted from client (deprecated or server side only)
    STATUS_UNHANDLED                       ///< We don' handle this opcode yet
};

/**
 * This determines how a \ref WorldPacket is handled by MaNGOS. This can be either in the
 * same function as we received it in, this is unusual, or it can be in:
 * - \ref World::UpdateSessions if it's not thread safe
 * - \ref Map::Update if it is thread safe
 */
enum PacketProcessing
{
    PROCESS_INPLACE = 0,   ///< process packet whenever we receive it - mostly for non-handled or non-implemented packets
    PROCESS_THREADUNSAFE,  ///< packet is not thread-safe - process it in \ref World::UpdateSessions
    PROCESS_THREADSAFE     ///< packet is thread-safe - process it in \ref Map::Update
};

class WorldPacket;

/**
 * A structure containing some of the necessary info to handle a \ref WorldPacket when it comes in.
 * The most interesting thing in here is the \ref OpcodeHandler::handler that actually does
 * something with one of the opcodes (see \ref Opcodes) that came in.
 */
struct OpcodeHandler
{
    ///A string representation of the name of this opcode (see \ref Opcodes)
    char const* name;
    ///The status for this handler, it tells whether or not we will handle the packet at all and
    ///when we will handle it.
    SessionStatus status;
    ///This tells where the packet should be processed, ie: is it thread un/safe, which in turn
    ///determines where it will be processed
    PacketProcessing packetProcessing;
    ///The callback called for this opcode which will work some magic
    void (WorldSession::*handler)(WorldPacket& recvPacket);
};

extern OpcodeHandler opcodeTable[NUM_MSG_TYPES];

/// Lookup opcode name for human understandable logging
inline const char* LookupOpcodeName(uint16 id)
{
    if (id >= NUM_MSG_TYPES)
    {
        return "Received unknown opcode, it's more than max!";
    }
    return opcodeTable[id].name;
}
#endif

/**
 * \var OpcodesList::SMSG_PERIODICAURALOG
 * This opcode is used to send data for the combat log when you receive either periodic damage or
 * buffs from a \ref Aura in some way, ie  you gain 10 life every second, you increase your regen
 * of power or something along those lines. The data that needs to be sent is a little different
 * depending on the \ref Modifier for the \ref Aura, what should always be included though is:
 * - The victims Pack GUID (see \ref Object::GetPackGUID)
 * - The casting \ref Player s Pack GUID (see \ref Object::GetPackGUID)
 * - The spellid for the \ref Aura (see \ref Aura::GetId) as a \ref uint32
 * - A 1 as a \ref uint32 this is the count of something (what)
 * - The id of the aura see \ref Modifier::m_auraname as a \ref uint32
 *
 * Now comes different parts depending on what value the \ref Modifier::m_auraname has, if it
 * is \ref AuraType::SPELL_AURA_PERIODIC_DAMAGE or
 * \ref AuraType::SPELL_AURA_PERIODIC_DAMAGE_PERCENT then this is sent:
 * - Damage done as a \ref uint32 from \ref SpellPeriodicAuraLogInfo::damage
 * - The \ref SpellSchools of the \ref SpellEntry for the \ref Aura as a \ref uint32 (see
 * \ref SpellEntry::School)
 * - How much that was absorbed as a \ref uint32
 * - How mcuh that was resisted as a \ref uint32
 *
 * If the \ref Modifier::m_auraname has one of the values of:
 * \ref AuraType::SPELL_AURA_PERIODIC_HEAL or \ref AuraType::SPELL_AURA_OBS_MOD_HEALTH then
 * this should be sent:
 * - Damage/healing (in this case) done as a \ref uint32
 *
 * If the \ref Modifier::m_auraname has one of the values of:
 * \ref AuraType::SPELL_AURA_OBS_MOD_MANA or \ref AuraType::SPELL_AURA_PERIODIC_ENERGIZE then
 * this should be sent:
 * - The \ref Modifier::m_miscvalue as a \ref uint32, in this case it's a power type from the
 * \ref Powers
 * - The damage/mana earned (in this case) as a \ref uint32
 *
 * If the \ref Modifier::m_auraname has one of the values of:
 * \ref AuraType::SPELL_AURA_PERIODIC_MANA_LEECH then this should be sent:
 * - The \ref Modifier::m_miscvalue as a \ref uint32, in this case it's a power type from the
 * \ref Powers
 * - The damage/amount of mana drained (in this case) as a \ref uint32
 * - The gain multiplier as a \ref float from the which probably increases how much power was
 * drained
 *
 * To not create this packet and send it all the time you need it you can use
 * \ref Unit::SendPeriodicAuraLog
 *
 * Also, this should be sent with \ref Object::SendMessageToSet so that all nearby (in
 * the same \ref Cell) \ref Player s get the information. To do this with an \ref Aura
 * one could use \ref Aura::GetTarget and then use the \ref Unit::SendMessageToSet
 * \todo Is it actually for the combat log?
 * \todo Is it in the same \ref Cell?
 * \todo What is the count that is sent as a uint32?
 * \todo Document the multiplier in some way?
 */

/**
 * \var OpcodesList::SMSG_SPELLNONMELEEDAMAGELOG
 * This opcode is used to send data for the combat log when you damage someone with a non melee
 * spell, ie frostbolt.
 * The data that needs to be sent is the following in the same order:
 * - The victims Pack GUID (see \ref Object::GetPackGUID)
 * - The \ref Player s Pack GUID (see \ref Object::GetPackGUID)
 * - Id of the spell that was used as a \ref uint32
 * - The amount of damage that was done (not including resisted damage etc) as a \ref uint32
 * - The \ref SpellSchoolMask of the \ref Spell as a \ref uint8, should be from the representation
 * in \ref SpellSchools though, to do this one can use \ref GetFirstSchoolInMask
 * - The amount of absorbed damage as a \ref uint32
 * - The amount of resisted damage as a \ref uint32
 * - A \ref uint8 which if it is 1 shows the spell name for the client, ie: "%s's ranged shot
 * hit %s for %d damage" (taken from source) and if it's 0 no message is shown
 * - A \ref uint8 value that seems to be unused
 * - The amount of blocked damage as a \ref uint32
 * - The \ref HitInfo as a \ref uint32 which tells what happened it would seem
 * - A \ref uint8 that's usually 0 and is used as a flag to use extended data (taken from source)
 *
 * To not create this packet and send it all the time you need it you can use
 * \ref Unit::SendSpellNonMeleeDamageLog
 *
 * Also, this should be sent with \ref Object::SendMessageToSet so that all nearby (in
 * the same \ref Cell) \ref Player s get the information.
 * \todo Is it actually for the combat log?
 * \todo Is it in the same \ref Cell?
 */

/**
 * \var OpcodesList::SMSG_SPELLENERGIZELOG
 * This opcode is used to send data for the combat log when you gain energy in some way.
 * The data that needs to be sent is the following in the same order:
 * - The victims Pack GUID (see \ref Object::GetPackGUID)
 * - The \ref Player s Pack GUID (see \ref Object::GetPackGUID)
 * - the spellid as a \ref uint32
 * - the powertype as a \ref uint32, see \ref Powers for the available power types
 * - the damage or in this case gain as a \ref uint32
 *
 * To not create this packet and send it all the time you need it you can use
 * \ref Unit::SendEnergizeSpellLog
 * Also, this should be sent with \ref Object::SendMessageToSet so that all nearby (in
 * the same \ref Cell) \ref Player s get the information.
 * \todo Is it actually for the combat log?
 * \todo Is it in the same \ref Cell?
 */

/**
 * \var OpcodesList::SMSG_SPELLHEALLOG
 * This opcode is used to send data for the combat log when healing is done. The data
 * that needs to be sent is the following in the same order:
 * - The victims Pack GUID (see \ref Object::GetPackGUID)
 * - The \ref Player s Pack GUID (see \ref Object::GetPackGUID)
 * - The spellid as a \ref uint32
 * - The damage/healing done as a \ref uint32
 * - If it was critical or not as a \ref uint8 (1 meaning critical, 0 meaning normal)
 * - And a \ref uint8 with the value 0 which doesn't seem to be used in the client
 *
 * To not create this packet and send it all the time you need it you can use
 * \ref Unit::SendHealSpellLog
 * Also, this should be sent with \ref Object::SendMessageToSet so that all nearby (in
 * the same \ref Cell) \ref Player s get the information.
 * \todo Is it actually for the combat log?
 * \todo Is it in the same \ref Cell?
 */

/**
 * \var OpcodesList::SMSG_ATTACKERSTATEUPDATE
 * This opcode is used to send information about a recent hit, who it hit, how
 * much damage it did and so forth. See the \ref CalcDamageInfo structure for more
 * info on what will be sent. The data that needs to be sent is the following in
 * the same order:
 * - The \ref CalcDamageInfo::HitInfo as a \ref uint32
 * - The \ref Unit s Pack GUID (see \ref Object::GetPackGUID)
 * - The targets Pack GUID (see \ref Object::GetPackGUID)
 * - The full damage that was done as a \ref uint32
 * - A 1 as a \ref uint8, this acts as the subdamage count (could it be higher?)
 * - A \ref uint32 of \code{.cpp} GetFirstSchoolInMask(damageInfo->damageSchoolMask) \endcode
 * Need to find out what this does
 * - A float representation of the damage (seen as sub damage from comments)
 * - A \ref uint32 representation of the same damage
 * - A \ref uint32 representation of how much was absorbed (see \ref CalcDamageInfo::absorb)
 * - A \ref uint32 representation of how much was resisted (see \ref CalcDamageInfo::resist)
 * - The targets state as a \ref uint32 (see \ref CalcDamageInfo::TargetState)
 * - If the absorbed part is zero add a 0 as an \ref uint32 otherwise add a -1 as an \ref uint32
 * - The spell id as a \ref uint32 if a spell was used, although in
 * \ref Unit::SendAttackStateUpdate it is always 0.
 * - The blocked amount as a \ref uint32 (see \ref CalcDamageInfo::blocked_amount) this is
 * normally \ref HitInfo::HITINFO_NOACTION according to comments in \ref Unit::SendAttackStateUpdate
 *
 * It appears this should also be sent with \ref Object::SendMessageToSet to that all nearby (in
 * the same \ref Cell) \ref Player s can get take part of the info
 * \see VictimState
 * \todo Is this correct? Is it really about a recent hit?
 */


/// @}
