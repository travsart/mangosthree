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

#include "CreatureAI.h"
#include "Creature.h"
#include "DBCStores.h"
#include "Spell.h"
#include "SpellMgr.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"

static_assert(MAXIMAL_AI_EVENT_EVENTAI <= 32, "Maximal 32 AI_EVENTs supported with EventAI");

CreatureAI::~CreatureAI()
{
}

void CreatureAI::AttackedBy(Unit* attacker)
{
    if (!m_creature->getVictim())
    {
        AttackStart(attacker);
    }
}

CanCastResult CreatureAI::CanCastSpell(Unit* pTarget, const SpellEntry* pSpell, bool isTriggered)
{
    // If not triggered, we check
    if (!isTriggered)
    {
        // State does not allow
        if (m_creature->hasUnitState(UNIT_STAT_CAN_NOT_REACT_OR_LOST_CONTROL))
        {
            return CAST_FAIL_STATE;
        }

        if (pSpell->GetPreventionType() == SPELL_PREVENTION_TYPE_SILENCE && m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
        {
            return CAST_FAIL_STATE;
        }

        if (pSpell->GetPreventionType() == SPELL_PREVENTION_TYPE_PACIFY && m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED))
        {
            return CAST_FAIL_STATE;
        }

        // Check for power (also done by Spell::CheckCast())
        if (m_creature->GetPower((Powers)pSpell->powerType) < Spell::CalculatePowerCost(pSpell, m_creature))
        {
            return CAST_FAIL_POWER;
        }

        if (!pSpell->HasAttribute(SPELL_ATTR_EX2_IGNORE_LOS) && !m_creature->IsWithinLOSInMap(pTarget) && m_creature != pTarget)
        {
            return CAST_FAIL_NOT_IN_LOS;
        }
    }

    if (const SpellRangeEntry* pSpellRange = sSpellRangeStore.LookupEntry(pSpell->rangeIndex))
    {
        if (pTarget != m_creature)
        {
            // pTarget is out of range of this spell (also done by Spell::CheckCast())
            float fDistance = m_creature->GetCombatDistance(pTarget, pSpell->rangeIndex == SPELL_RANGE_IDX_COMBAT);

            if (fDistance > (m_creature->IsHostileTo(pTarget) ? pSpellRange->maxRange : pSpellRange->maxRangeFriendly))
            {
                return CAST_FAIL_TOO_FAR;
            }

            float fMinRange = m_creature->IsHostileTo(pTarget) ? pSpellRange->minRange : pSpellRange->minRangeFriendly;

            if (fMinRange && fDistance < fMinRange)
            {
                return CAST_FAIL_TOO_CLOSE;
            }
        }

        return CAST_OK;
    }
    else
    {
        return CAST_FAIL_OTHER;
    }
}

CanCastResult CreatureAI::DoCastSpellIfCan(Unit* pTarget, uint32 uiSpell, uint32 uiCastFlags, ObjectGuid uiOriginalCasterGUID)
{
    Unit* pCaster = m_creature;

    if (uiCastFlags & CAST_FORCE_TARGET_SELF)
    {
        pCaster = pTarget;
    }

    // Allowed to cast only if not casting (unless we interrupt ourself) or if spell is triggered
    if (!pCaster->IsNonMeleeSpellCasted(false) || (uiCastFlags & (CAST_TRIGGERED | CAST_INTERRUPT_PREVIOUS)))
    {
        if (const SpellEntry* pSpell = sSpellStore.LookupEntry(uiSpell))
        {
            // If cast flag CAST_AURA_NOT_PRESENT is active, check if target already has aura on them
            if (uiCastFlags & CAST_AURA_NOT_PRESENT)
            {
                if (pTarget->HasAura(uiSpell))
                {
                    return CAST_FAIL_TARGET_AURA;
                }
            }

            // Check if can not cast spell
            if (!(uiCastFlags & (CAST_FORCE_TARGET_SELF | CAST_FORCE_CAST)))
            {
                CanCastResult castResult = CanCastSpell(pTarget, pSpell, uiCastFlags & CAST_TRIGGERED);

                if (castResult != CAST_OK)
                {
                    return castResult;
                }
            }

            // Interrupt any previous spell
            if ((uiCastFlags & CAST_INTERRUPT_PREVIOUS) && pCaster->IsNonMeleeSpellCasted(false))
            {
                pCaster->InterruptNonMeleeSpells(false);
            }

            // Creature should always stop before it will cast a non-instant spell
            if (GetSpellCastTime(pSpell))
            {
                pCaster->StopMoving();
            }

            // Creature should interrupt any current melee spell
            pCaster->InterruptSpell(CURRENT_MELEE_SPELL);

            // Creature should stop wielding weapon while casting
            pCaster->SetSheath(SHEATH_STATE_UNARMED);

            pCaster->CastSpell(pTarget, pSpell, uiCastFlags & CAST_TRIGGERED, NULL, NULL, uiOriginalCasterGUID);
            return CAST_OK;
        }
        else
        {
            sLog.outErrorDb("DoCastSpellIfCan by creature entry %u attempt to cast spell %u but spell does not exist.", m_creature->GetEntry(), uiSpell);
            return CAST_FAIL_OTHER;
        }
    }
    else
    {
        return CAST_FAIL_IS_CASTING;
    }
}

bool CreatureAI::DoMeleeAttackIfReady()
{
    return m_creature->UpdateMeleeAttackingState();
}

void CreatureAI::SetCombatMovement(bool enable, bool stopOrStartMovement /*=false*/)
{
    m_isCombatMovement = enable;

    if (enable)
    {
        m_creature->clearUnitState(UNIT_STAT_NO_COMBAT_MOVEMENT);
    }
    else
    {
        m_creature->addUnitState(UNIT_STAT_NO_COMBAT_MOVEMENT);
    }

    if (stopOrStartMovement && m_creature->getVictim())     // Only change current movement while in combat
    {
        if (enable)
        {
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim(), m_attackDistance, m_attackAngle);
        }
        else if (!enable && m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
        {
            m_creature->StopMoving();
        }
    }
}

void CreatureAI::HandleMovementOnAttackStart(Unit* victim)
{
    MotionMaster* creatureMotion = m_creature->GetMotionMaster();
    if (m_isCombatMovement)
    {
        creatureMotion->MoveChase(victim, m_attackDistance, m_attackAngle);
    }
    // TODO - adapt this to only stop OOC-MMGens when MotionMaster rewrite is finished
    else if (creatureMotion->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE || creatureMotion->GetCurrentMovementGeneratorType() == RANDOM_MOTION_TYPE)
    {
        creatureMotion->MoveIdle();
        m_creature->StopMoving();
    }
}

// ////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Event system
// ////////////////////////////////////////////////////////////////////////////////////////////////

class AiDelayEventAround : public BasicEvent
{
    public:
        AiDelayEventAround(AIEventType eventType, ObjectGuid invokerGuid, Creature& owner, std::list<Creature*> const& receivers, uint32 miscValue) :
            BasicEvent(),
            m_eventType(eventType),
            m_invokerGuid(invokerGuid),
            m_owner(owner),
            m_miscValue(miscValue)
        {
            // Pushing guids because in delay can happen some creature gets despawned => invalid pointer
            m_receiverGuids.reserve(receivers.size());
            for (std::list<Creature*>::const_iterator itr = receivers.begin(); itr != receivers.end(); ++itr)
            {
                m_receiverGuids.push_back((*itr)->GetObjectGuid());
            }
        }

        bool Execute(uint64 /*e_time*/, uint32 /*p_time*/) override
        {
            Unit* pInvoker = m_owner.GetMap()->GetUnit(m_invokerGuid);

            for (GuidVector::const_reverse_iterator itr = m_receiverGuids.rbegin(); itr != m_receiverGuids.rend(); ++itr)
            {
                if (Creature* pReceiver = m_owner.GetMap()->GetAnyTypeCreature(*itr))
                {
                    pReceiver->AI()->ReceiveAIEvent(m_eventType, &m_owner, pInvoker, m_miscValue);
                    // Special case for type 0 (call-assistance)
                    if (m_eventType == AI_EVENT_CALL_ASSISTANCE && pInvoker && pReceiver->CanAssistTo(&m_owner, pInvoker))
                    {
                        pReceiver->SetNoCallAssistance(true);
                        pReceiver->AI()->AttackStart(pInvoker);
                    }
                }
            }
            m_receiverGuids.clear();

            return true;
        }

    private:
        AiDelayEventAround();

        AIEventType m_eventType;
        ObjectGuid m_invokerGuid;
        Creature&  m_owner;
        uint32 m_miscValue;

        GuidVector m_receiverGuids;
};

void CreatureAI::SendAIEventAround(AIEventType eventType, Unit* pInvoker, uint32 uiDelay, float fRadius, uint32 miscValue /*=0*/) const
{
    if (fRadius > 0)
    {
        std::list<Creature*> receiverList;

        // Allow sending custom AI events to all units in range
        if (eventType >= AI_EVENT_CUSTOM_EVENTAI_A && eventType <= AI_EVENT_CUSTOM_EVENTAI_F && eventType != AI_EVENT_GOT_CCED)
        {
            MaNGOS::AnyUnitInObjectRangeCheck u_check(m_creature, fRadius);
            MaNGOS::CreatureListSearcher<MaNGOS::AnyUnitInObjectRangeCheck> searcher(receiverList, u_check);
            Cell::VisitGridObjects(m_creature, searcher, fRadius);
        }
        else
        {
            // Use this check here to collect only assitable creatures in case of CALL_ASSISTANCE, else be less strict
            MaNGOS::AnyAssistCreatureInRangeCheck u_check(m_creature, eventType == AI_EVENT_CALL_ASSISTANCE ? pInvoker : NULL, fRadius);
            MaNGOS::CreatureListSearcher<MaNGOS::AnyAssistCreatureInRangeCheck> searcher(receiverList, u_check);
            Cell::VisitGridObjects(m_creature, searcher, fRadius);
        }

        if (!receiverList.empty())
        {
            AiDelayEventAround* e = new AiDelayEventAround(eventType, pInvoker ? pInvoker->GetObjectGuid() : ObjectGuid(), *m_creature, receiverList, miscValue);
            m_creature->m_Events.AddEvent(e, m_creature->m_Events.CalculateTime(uiDelay));
        }
    }
}

void CreatureAI::SendAIEvent(AIEventType eventType, Unit* pInvoker, Creature* pReceiver, uint32 miscValue /*=0*/) const
{
    MANGOS_ASSERT(pReceiver);
    pReceiver->AI()->ReceiveAIEvent(eventType, m_creature, pInvoker, miscValue);
}
