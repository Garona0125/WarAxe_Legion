/*
░██╗░░░░░░░██╗░█████╗░██████╗░░█████╗░██╗░░██╗███████╗░█████╗░░█████╗░██████╗░███████╗
░██║░░██╗░░██║██╔══██╗██╔══██╗██╔══██╗╚██╗██╔╝██╔════╝██╔══██╗██╔══██╗██╔══██╗██╔════╝
░╚██╗████╗██╔╝███████║██████╔╝███████║░╚███╔╝░█████╗░░██║░░╚═╝██║░░██║██████╔╝█████╗░░
░░████╔═████║░██╔══██║██╔══██╗██╔══██║░██╔██╗░██╔══╝░░██║░░██╗██║░░██║██╔══██╗██╔══╝░░
░░╚██╔╝░╚██╔╝░██║░░██║██║░░██║██║░░██║██╔╝╚██╗███████╗╚█████╔╝╚█████╔╝██║░░██║███████╗
░░░╚═╝░░░╚═╝░░╚═╝░░╚═╝╚═╝░░╚═╝╚═╝░░╚═╝╚═╝░░╚═╝╚══════╝░╚════╝░░╚════╝░╚═╝░░╚═╝╚══════╝

(c) Copyright WarAxeCore_Legion 2020
*/

/*
Issues:
Missing the intro for killing all enemies
*/

#include "hellfire_citadel.h"

enum HFAssault_Information
{
	NPC_SIEGEMASTER_MARTOK = 93023,
};

enum HFAssault_Events
{
	EVENT_START_ASSAULT = 1,
	EVENT_SHOCKWAVE,
	EVENT_HOWLING_AXE,
};

Position const ironDragoonSpawns[] =
{
	{3989.79f, -697.32f, 30.94f},
	{3994.62f, -702.86f, 30.996f}
};

class boss_siegemaster_martak : public CreatureScript
{
public:
	boss_siegemaster_martak() : CreatureScript("boss_siegemaster_martak") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_siegemaster_martakAI(creature);
	}

	struct boss_siegemaster_martakAI : public BossAI
	{
		boss_siegemaster_martakAI(Creature* creature) : BossAI(creature, DATA_HELLFIRE_ASSAULT) { }

		bool _phase2;

		void Reset()
		{
			_phase2 = false;

			//Inspiring Presence
			if (!me->HasAura(185090))
			{
				me->CastSpell(me, 185090, true);
			}

			events.Reset();
			_Reset();
		}

		void EnterCombat(Unit* /*attacker*/) override
		{
			_EnterCombat();

			Talk(4); //So, you wish to test the might of the Fel Horde? Come warriors, show them how we treat unwanted guests.
			Creature* hellfire_door = me->FindNearestCreature(90019, 600.0f);
			instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
			if (hellfire_door)
			instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, hellfire_door);

			events.RescheduleEvent(EVENT_SHOCKWAVE, urand(5000, 7000));
			events.RescheduleEvent(EVENT_HOWLING_AXE, urand(3000, 5000));
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (who->GetTypeId() == TYPEID_PLAYER)
			{
				if (who->GetDistance(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()) <= 160.0f)
				{
					Talk(0); //Stand proud, warriors. Our enemies march upon the foot of this mighty citadel. Be on guard.
					me->AddDelayedEvent(6000, [this]() -> void { me->SetFlag(UNIT_FIELD_FLAGS, 0); }); // Make attackable.
					me->GetMotionMaster()->MoveCharge(3947.89f, -664.75f, 30.88f);
				}
			}
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			// Phase 2 Assault Start
			if (me->GetHealthPct() <= 50 && _phase2 == false)
			{
				Talk(6); // I'll be back!
				me->SetReactState(REACT_PASSIVE);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				me->GetMotionMaster()->MoveCharge(3859.93f, -580.85f, 53.40f);
				_phase2 = true;
				events.RescheduleEvent(EVENT_START_ASSAULT, 5000);
			}

			if (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_START_ASSAULT:
					events.RescheduleEvent(EVENT_START_ASSAULT, urand(9000, 12000));
					break;
				case EVENT_HOWLING_AXE:
					break;
				case EVENT_SHOCKWAVE:
					break;
				default:
					break;
				}
			}

			DoMeleeAttackIfReady();
		}
	private:
		InstanceScript* instance;
		EventMap events;
	};
};

void AddSC_hellfire_assault()
{
}