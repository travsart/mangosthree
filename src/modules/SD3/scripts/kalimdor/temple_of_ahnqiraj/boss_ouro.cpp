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
 * SDName:      Boss_Ouro
 * SD%Complete: 90
 * SDComment:   Some minor adjustments may be required
 * SDCategory:  Temple of Ahn'Qiraj
 * EndScriptData
 */

#include "precompiled.h"
#include "temple_of_ahnqiraj.h"

enum
{
    // ground spells
    SPELL_SWEEP             = 26103,
    SPELL_SANDBLAST         = 26102,
    SPELL_BOULDER           = 26616,
    SPELL_BERSERK           = 26615,

    // emerge spells
    SPELL_BIRTH             = 26262,                        // The Birth Animation
    SPELL_GROUND_RUPTURE    = 26100,                        // spell not confirmed
    SPELL_SUMMON_BASE       = 26133,                        // summons gameobject 180795

    // submerge spells
    SPELL_SUBMERGE_VISUAL   = 26063,
#if defined (CLASSIC)
    SPELL_SUMMON_OURO_MOUNDS = 26058,                       // summons 5 dirt mounds
#else
    SPELL_SUMMON_OURO_MOUND = 26058,                        // summons 5 dirt mounds
#endif

    SPELL_SUMMON_TRIGGER    = 26284,

    SPELL_SUMMON_OURO       = 26642,
#if defined (TBC) || defined (WOTLK) || defined (CATA) || defined(MISTS)
    SPELL_QUAKE             = 26093,
#endif

    // other spells - not used TODO check this "synced mess"
    // SPELL_SUMMON_SCARABS    = 26060,                     // triggered after 30 secs - cast by the Dirt Mounds
#if defined (CLASSIC)
    SPELL_DIRTMOUND_PASSIVE = 26092,                        // casts 26093 every 1 sec
#endif
    // SPELL_SET_OURO_HEALTH   = 26075,                     // removed from DBC
    // SPELL_SAVE_OURO_HEALTH  = 26076,                     // removed from DBC
    // SPELL_TELEPORT_TRIGGER  = 26285,                     // removed from DBC
    // SPELL_SUBMERGE_TRIGGER  = 26104,                     // removed from DBC
#if defined (CLASSIC)
    SPELL_SUMMON_OURO_MOUND = 26617,
#endif
    // SPELL_SCARABS_PERIODIC  = 26619,                     // cast by the Dirt Mounds in order to spawn the scarabs - removed from DBC

    // summoned npcs
    NPC_OURO                = 15517,
    // NPC_OURO_SCARAB       = 15718,                       // summoned by Dirt Mounds
    NPC_OURO_TRIGGER        = 15717,
    NPC_DIRT_MOUND          = 15712,                        // summoned also by missing spell 26617
};

struct boss_ouro : public CreatureScript
{
    boss_ouro() : CreatureScript("boss_ouro") {}

    struct boss_ouroAI : public Scripted_NoMovementAI
    {
        boss_ouroAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
        {
            m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        }

        ScriptedInstance* m_pInstance;

        uint32 m_uiSweepTimer;
        uint32 m_uiSandBlastTimer;
        uint32 m_uiSubmergeTimer;
        uint32 m_uiSummonBaseTimer;

        uint32 m_uiSummonMoundTimer;

        bool m_bEnraged;
        bool m_bSubmerged;

        ObjectGuid m_ouroTriggerGuid;

        void Reset() override
        {
            m_uiSweepTimer = urand(35000, 40000);
            m_uiSandBlastTimer = urand(30000, 45000);
            m_uiSubmergeTimer = 90000;
            m_uiSummonBaseTimer = 2000;

            m_uiSummonMoundTimer = 10000;

            m_bEnraged = false;
            m_bSubmerged = false;
        }

        void Aggro(Unit* /*pWho*/) override
        {
            if (m_pInstance)
            {
                m_pInstance->SetData(TYPE_OURO, IN_PROGRESS);
            }
        }

        void JustReachedHome() override
        {
            if (m_pInstance)
            {
                m_pInstance->SetData(TYPE_OURO, FAIL);
            }

            m_creature->ForcedDespawn();
        }

        void JustDied(Unit* /*pKiller*/) override
        {
            if (m_pInstance)
            {
                m_pInstance->SetData(TYPE_OURO, DONE);
            }
        }

        void JustSummoned(Creature* pSummoned) override
        {
            switch (pSummoned->GetEntry())
            {
            case NPC_OURO_TRIGGER:
                m_ouroTriggerGuid = pSummoned->GetObjectGuid();
                // no break;
            case NPC_DIRT_MOUND:
                pSummoned->GetMotionMaster()->MoveRandomAroundPoint(pSummoned->GetPositionX(), pSummoned->GetPositionY(), pSummoned->GetPositionZ(), 40.0f);
                break;
            }
        }

        void UpdateAI(const uint32 uiDiff) override
        {
            // Return since we have no pTarget
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            {
                return;
            }

            if (!m_bSubmerged)
            {
                // Summon sandworm base
                if (m_uiSummonBaseTimer)
                {
                    if (m_uiSummonBaseTimer <= uiDiff)
                    {
                        // Note: server side spells should be cast directly
                        m_creature->CastSpell(m_creature, SPELL_SUMMON_BASE, true);
                        m_uiSummonBaseTimer = 0;
                    }
                    else
                    {
                        m_uiSummonBaseTimer -= uiDiff;
                    }
                }

                // Sweep
                if (m_uiSweepTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SWEEP) == CAST_OK)
                    {
                        m_uiSweepTimer = 20000;
                    }
                }
                else
                {
                    m_uiSweepTimer -= uiDiff;
                }

                // Sand Blast
                if (m_uiSandBlastTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SANDBLAST) == CAST_OK)
                    {
                        m_uiSandBlastTimer = 22000;
                    }
                }
                else
                {
                    m_uiSandBlastTimer -= uiDiff;
                }

                if (!m_bEnraged)
                {
                    // Enrage at 20% HP
                    if (m_creature->GetHealthPercent() < 20.0f)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                        {
                            m_bEnraged = true;
                            return;
                        }
                    }

                    // Submerge
                    if (m_uiSubmergeTimer < uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_SUBMERGE_VISUAL) == CAST_OK)
                        {
#if defined (CLASSIC)
                            DoCastSpellIfCan(m_creature, SPELL_SUMMON_OURO_MOUNDS, CAST_TRIGGERED);
#else
                            DoCastSpellIfCan(m_creature, SPELL_SUMMON_OURO_MOUND, CAST_TRIGGERED);
#endif
                            DoCastSpellIfCan(m_creature, SPELL_SUMMON_TRIGGER, CAST_TRIGGERED);

                            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                            m_bSubmerged = true;
                            m_uiSubmergeTimer = 30000;
                        }
                    }
                    else
                    {
                        m_uiSubmergeTimer -= uiDiff;
                    }
                }
                else
                {
                    // Summon 1 mound every 10 secs when enraged
                    if (m_uiSummonMoundTimer < uiDiff)
                    {

#if defined (CLASSIC)
                        if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_OURO_MOUND) == CAST_OK)
#else
                    DoSpawnCreature(NPC_DIRT_MOUND, 0, 0, 0, 0, TEMPSPAWN_CORPSE_DESPAWN, 0);
#endif
                        {
                            m_uiSummonMoundTimer = 10000;
                        }
                    }
                    else
                    {
                        m_uiSummonMoundTimer -= uiDiff;
                    }
                }

                // If we are within range melee the target
                if (m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
                {
                    DoMeleeAttackIfReady();
                }
                // Spam Boulder spell when enraged and not tanked
                else if (m_bEnraged)
                {
                    if (!m_creature->IsNonMeleeSpellCasted(false))
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        {
                            DoCastSpellIfCan(pTarget, SPELL_BOULDER);
                        }
                    }
                }
            }
            else
            {
                // Resume combat
                if (m_uiSubmergeTimer < uiDiff)
                {
                    // Teleport to the trigger in order to get a new location
                    if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_ouroTriggerGuid))
                    {
                        m_creature->NearTeleportTo(pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ(), 0);
                    }

                    if (DoCastSpellIfCan(m_creature, SPELL_BIRTH) == CAST_OK)
                    {
                        m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE_VISUAL);
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                        m_bSubmerged = false;
                        m_uiSummonBaseTimer = 2000;
                        m_uiSubmergeTimer = 90000;
                    }
                }
                else
                {
                    m_uiSubmergeTimer -= uiDiff;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* pCreature) override
    {
        return new boss_ouroAI(pCreature);
    }
};

struct npc_ouro_spawner : public CreatureScript
{
    npc_ouro_spawner() : CreatureScript("npc_ouro_spawner") {}

    struct npc_ouro_spawnerAI : public Scripted_NoMovementAI
    {
        npc_ouro_spawnerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { }


#if defined (TBC) || defined (WOTLK) || defined (CATA) || defined(MISTS)
    uint32 m_uiQuakeTimer;
#endif
        bool m_bHasSummoned;

        void Reset() override
        {

#if defined (TBC) || defined (WOTLK) || defined (CATA) || defined(MISTS)
        m_uiQuakeTimer = 1000;
#endif
            m_bHasSummoned = false;
        }

        void MoveInLineOfSight(Unit* pWho) override
        {
            // Spawn Ouro on LoS check
            if (!m_bHasSummoned && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*)pWho)->isGameMaster() && m_creature->IsWithinDistInMap(pWho, 50.0f))
            {

#if defined (CLASSIC)
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_OURO, CAST_TRIGGERED) == CAST_OK)
#else
                if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_OURO) == CAST_OK)
#endif
                {
#if defined (CLASSIC)
                    DoCastSpellIfCan(m_creature, SPELL_DIRTMOUND_PASSIVE, CAST_TRIGGERED);
#endif
                    m_bHasSummoned = true;
                }
            }

            ScriptedAI::MoveInLineOfSight(pWho);
        }

        void JustSummoned(Creature* pSummoned) override
        {
            // Despanw when Ouro is spawned
            if (pSummoned->GetEntry() == NPC_OURO)
            {
                pSummoned->CastSpell(pSummoned, SPELL_BIRTH, false);
                pSummoned->SetInCombatWithZone();
                m_creature->ForcedDespawn();
            }
        }

    void UpdateAI(const uint32 uiDiff) override
    {
#if defined (TBC) || defined (WOTLK) || defined (CATA) || defined(MISTS)
        if (m_bHasSummoned)
        {
            if (m_uiQuakeTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_QUAKE) == CAST_OK)
                {
                    m_uiQuakeTimer = 1000;
                }
            }
            else
            {
                m_uiQuakeTimer -= uiDiff;
            }
        }
#endif
    }
    };

    CreatureAI* GetAI(Creature* pCreature) override
    {
        return new npc_ouro_spawnerAI(pCreature);
    }
};

void AddSC_boss_ouro()
{
    Script* s;
    s = new boss_ouro();
    s->RegisterSelf();
    s = new npc_ouro_spawner();
    s->RegisterSelf();

    //pNewScript = new Script;
    //pNewScript->Name = "boss_ouro";
    //pNewScript->GetAI = &GetAI_boss_ouro;
    //pNewScript->RegisterSelf();

    //pNewScript = new Script;
    //pNewScript->Name = "npc_ouro_spawner";
    //pNewScript->GetAI = &GetAI_npc_ouro_spawner;
    //pNewScript->RegisterSelf();
}
