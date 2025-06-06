/**
 * ScriptDev3 is an extension for mangos providing enhanced features for
 * area triggers, creatures, game objects, instances, items, and spells beyond
 * the default database scripting in mangos.
 *
 * Copyright (C) 2006-2013 ScriptDev2 <http://www.scriptdev2.com/>
 * Copyright (C) 2014-2025 MaNGOS <https://www.getmangos.eu>
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

/**
 * ScriptData
 * SDName:      boss_zumrah
 * SD%Complete: 100
 * SDComment:   None
 * SDCategory:  Zul'Farrak
 * EndScriptData
 */

#include "precompiled.h"
#include "zulfarrak.h"

enum
{
    SAY_INTRO                   = -1209000,
    SAY_AGGRO                   = -1209001,
    SAY_KILL                    = -1209002,
    SAY_SUMMON                  = -1209003,

    SPELL_SHADOW_BOLT           = 12739,
    SPELL_SHADOW_BOLT_VOLLEY    = 15245,
    SPELL_WARD_OF_ZUMRAH        = 11086,
    SPELL_HEALING_WAVE          = 12491,
    SPELL_SUMMON_ZOMBIES        = 10247,            // spell should be triggered by missing trap 128972

    // NPC_WARD_OF_ZUMRAH       = 7785,
    // NPC_SKELETON_OF_ZUMRAH   = 7786,
    NPC_ZULFARRAK_ZOMBIE        = 7286,             // spawned by the graves
    NPC_ZULFARRAK_DEAD_HERO     = 7276,             // spawned by the graves

    FACTION_HOSTILE             = 14,
};

struct boss_zumrah : public CreatureScript
{
    boss_zumrah() : CreatureScript("boss_zumrah") {}

    struct boss_zumrahAI : public ScriptedAI
    {
        boss_zumrahAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
            m_bHasTurnedHostile = false;
        }

        ScriptedInstance* m_pInstance;

        uint32 m_uiShadowBoltTimer;
        uint32 m_uiShadowBoltVolleyTimer;
        uint32 m_uiWardOfZumrahTimer;
        uint32 m_uHealingWaveTimer;
        uint32 m_uiSpawnZombieTimer;

        bool m_bHasTurnedHostile;

        void Reset() override
        {
            m_uiShadowBoltTimer = 1000;
            m_uiShadowBoltVolleyTimer = urand(6000, 30000);
            m_uiWardOfZumrahTimer = urand(7000, 20000);
            m_uHealingWaveTimer = urand(10000, 15000);
            m_uiSpawnZombieTimer = 1000;
        }

        void Aggro(Unit* /*pWho*/) override
        {
            DoScriptText(SAY_AGGRO, m_creature);
        }

        void KilledUnit(Unit* /*pVictim*/) override
        {
            DoScriptText(SAY_KILL, m_creature);
        }

        void AttackStart(Unit* pWho) override
        {
            if (m_creature->Attack(pWho, true))
            {
                m_creature->AddThreat(pWho);
                m_creature->SetInCombatWith(pWho);
                pWho->SetInCombatWith(m_creature);
                DoStartMovement(pWho, 10.0f);
            }
        }

        void MoveInLineOfSight(Unit* pWho) override
        {
            if (!m_bHasTurnedHostile && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 9.0f) && m_creature->IsWithinLOSInMap(pWho))
            {
                m_creature->SetFactionTemporary(FACTION_HOSTILE, TEMPFACTION_NONE);
                DoScriptText(SAY_INTRO, m_creature);
                m_bHasTurnedHostile = true;
                AttackStart(pWho);
            }

            ScriptedAI::MoveInLineOfSight(pWho);
        }

        void JustSummoned(Creature* pSummoned) override
        {
            if (pSummoned->GetEntry() == NPC_ZULFARRAK_ZOMBIE || pSummoned->GetEntry() == NPC_ZULFARRAK_DEAD_HERO)
            {
                pSummoned->AI()->AttackStart(m_creature->getVictim());
            }
        }

        void UpdateAI(const uint32 uiDiff) override
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            {
                return;
            }

            if (m_uiSpawnZombieTimer)
            {
                if (m_uiSpawnZombieTimer <= uiDiff)
                {
                    // Use a nearby grave to spawn zombies
                    m_pInstance->SetData64(TYPE_SIGNAL, m_creature->GetObjectGuid().GetRawValue());
                    if (GameObject* pGrave = m_pInstance->instance->GetGameObject(ObjectGuid(m_pInstance->GetData64(TYPE_SIGNAL))))
                    {
                        m_creature->CastSpell(pGrave->GetPositionX(), pGrave->GetPositionY(), pGrave->GetPositionZ(), SPELL_SUMMON_ZOMBIES, true, nullptr, nullptr, pGrave->GetObjectGuid());
                        pGrave->SetLootState(GO_JUST_DEACTIVATED);

                        if (roll_chance_i(30))
                        {
                            DoScriptText(SAY_SUMMON, m_creature);
                        }

                        m_uiSpawnZombieTimer = 20000;
                    }
                    else                                        // No Grave usable any more
                    {
                        m_uiSpawnZombieTimer = 0;
                    }
                }
                else
                {
                    m_uiSpawnZombieTimer -= uiDiff;
                }
            }

            if (m_uiShadowBoltTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_SHADOW_BOLT) == CAST_OK)
                    {
                        m_uiShadowBoltTimer = urand(3500, 5000);
                    }
                }
            }
            else
            {
                m_uiShadowBoltTimer -= uiDiff;
            }

            if (m_uiShadowBoltVolleyTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SHADOW_BOLT_VOLLEY) == CAST_OK)
                {
                    m_uiShadowBoltVolleyTimer = urand(10000, 18000);
                }
            }
            else
            {
                m_uiShadowBoltVolleyTimer -= uiDiff;
            }

            if (m_uiWardOfZumrahTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_WARD_OF_ZUMRAH) == CAST_OK)
                {
                    m_uiWardOfZumrahTimer = urand(15000, 32000);
                }
            }
            else
            {
                m_uiWardOfZumrahTimer -= uiDiff;
            }

            if (m_uHealingWaveTimer < uiDiff)
            {
                if (Unit* pTarget = DoSelectLowestHpFriendly(40.0f))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_HEALING_WAVE) == CAST_OK)
                    {
                        m_uHealingWaveTimer = urand(15000, 23000);
                    }
                }
            }
            else
            {
                m_uHealingWaveTimer -= uiDiff;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* pCreature) override
    {
        return new boss_zumrahAI(pCreature);
    }
};

void AddSC_boss_zumrah()
{
    Script* s;

    s = new boss_zumrah();
    s->RegisterSelf();

    //pNewScript = new Script;
    //pNewScript->Name = "boss_zumrah";
    //pNewScript->GetAI = &GetAI_boss_zumrah;
    //pNewScript->RegisterSelf();
}
