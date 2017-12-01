/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Azshara
SD%Complete: 90
SDComment: Quest support: 2744, 3141, 9364, 10994
SDCategory: Azshara
EndScriptData */

/* ContentData
mobs_spitelashes
npc_loramus_thalipedes
mob_rizzle_sprysprocket
mob_depth_charge
EndContentData */


#include "World.h"
#include "WorldPacket.h"

/*######
## mobs_spitelashes
######*/

class mobs_spitelashes : public CreatureScript
{
public:
    mobs_spitelashes() : CreatureScript("mobs_spitelashes")
    { }

    class mobs_spitelashesAI : public ScriptedAI
    {
        public:
        mobs_spitelashesAI(Creature *c) : ScriptedAI(c) {}
    
        uint32 morphtimer;
        bool spellhit;
    
        void Reset() override
        {
            morphtimer = 0;
            spellhit = false;
        }
    
        void SpellHit(Unit *Hitter, const SpellInfo *Spellkind) override
        {
            if( !spellhit &&
                Hitter->GetTypeId() == TYPEID_PLAYER &&
                (Hitter->ToPlayer())->GetQuestStatus(9364) == QUEST_STATUS_INCOMPLETE &&
                (Spellkind->Id==118 || Spellkind->Id== 12824 || Spellkind->Id== 12825 || Spellkind->Id== 12826) )
            {
                spellhit=true;
                DoCast(me,29124);                       //become a sheep
            }
        }
    
        void UpdateAI(const uint32 diff) override
        {
            // we mustn't remove the creature in the same round in which we cast the summon spell, otherwise there will be no summons
            if( spellhit && morphtimer>=5000 )
            {
                me->DespawnOrUnsummon();
                EnterEvadeMode();                               //spellhit will be set to false
                return;
            }
            // walk 5 seconds before summoning
            if( spellhit && morphtimer<5000 )
            {
                morphtimer+=diff;
                if( morphtimer>=5000 )
                {
                    DoCast(me,28406);                   //summon copies
                    DoCast(me,6924);                    //visual explosion
                }
            }
            if (!UpdateVictim() )
                return;
    
            //TODO: add abilities for the different creatures
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mobs_spitelashesAI(creature);
    }
};


/*######
## npc_loramus_thalipedes
######*/

class npc_loramus_thalipedes : public CreatureScript
{
public:
    npc_loramus_thalipedes() : CreatureScript("npc_loramus_thalipedes")
    { }

    class npc_loramus_thalipedesAI : public ScriptedAI
    {
    public:
        npc_loramus_thalipedesAI(Creature* creature) : ScriptedAI(creature)
        {}


        virtual bool GossipHello(Player* pPlayer) override
        {
            if (me->IsQuestGiver())
                pPlayer->PrepareQuestMenu(me->GetGUID());

            //TODO translate
            if (pPlayer->GetQuestStatus(2744) == QUEST_STATUS_INCOMPLETE)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Pouvez-vous m'aider ?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

            if (pPlayer->GetQuestStatus(3141) == QUEST_STATUS_INCOMPLETE)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Racontez-moi votre histoire", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            
            SEND_PREPARED_GOSSIP_MENU(pPlayer, me);

            return true;

        }


        virtual bool GossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            //TODO translate
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF+1:
                    player->CLOSE_GOSSIP_MENU();
                    player->AreaExploredOrEventHappens(2744);
                    break;

                case GOSSIP_ACTION_INFO_DEF+2:
                    player->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, "Continuez", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 21);
                    player->SEND_GOSSIP_MENU_TEXTID(1813, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+21:
                    player->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, "Je ne comprends pas", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 22);
                    player->SEND_GOSSIP_MENU_TEXTID(1814, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+22:
                    player->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, "En effet", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 23);
                    player->SEND_GOSSIP_MENU_TEXTID(1815, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+23:
                    player->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, "Je ferai ceci avec ou sans votre aide, Loramus", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 24);
                    player->SEND_GOSSIP_MENU_TEXTID(1816, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+24:
                    player->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, "Oui", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 25);
                    player->SEND_GOSSIP_MENU_TEXTID(1817, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+25:
                    player->CLOSE_GOSSIP_MENU();
                    player->AreaExploredOrEventHappens(3141);
                    break;
            }
            return true;

        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_loramus_thalipedesAI(creature);
    }
};



/*####
# mob_rizzle_sprysprocket
####*/

enum eRizzleSpyrocket
{
MOB_DEPTH_CHARGE                = 23025,
SPELL_RIZZLE_BLACKJACK          = 39865,
SPELL_RIZZLE_ESCAPE             = 39871,
SPELL_RIZZLE_FROST_GRENADE      = 40525,
SPELL_DEPTH_CHARGE_TRAP         = 38576,
SPELL_PERIODIC_DEPTH_CHARGE     = 39912,
SPELL_GIVE_SOUTHFURY_MOONSTONE  = 39886,

SAY_RIZZLE_START                = -1000245,
SAY_RIZZLE_GRENADE              = -1000246,
SAY_RIZZLE_FINAL                = -1000247
};

#define GOSSIP_GET_MOONSTONE "Donne-moi la pierre de lune Furie-du-sud et je te laisserai partir."

//next message must be send to player when Rizzle jump into river, not implemented
#define MSG_ESCAPE_NOTICE "Rizzle Sprysprocket prend la pierre de lune Furie-du-sud et s'enfuit dans la rivière. Suivez-la !"

float WPs[58][4] =
{
//      pos_x     pos_y      pos_z     orien
    { 3691.97f, -3962.41f, 35.9118f, 3.67f },
    { 3675.02f, -3960.49f, 35.9118f, 3.67f },
    { 3653.19f, -3958.33f, 33.9118f, 3.59f },
    { 3621.12f, -3958.51f, 29.9118f, 3.48f },
    { 3604.86f, -3963.00f, 29.9118f, 3.48f },
    { 3569.94f, -3970.25f, 29.9118f, 3.44f },
    { 3541.03f, -3975.64f, 29.9118f, 3.41f },
    { 3510.84f, -3978.71f, 29.9118f, 3.41f },
    { 3472.70f, -3997.07f, 29.9118f, 3.35f },
    { 3439.15f, -4014.55f, 29.9118f, 3.29f },
    { 3412.80f, -4025.87f, 29.9118f, 3.25f },
    { 3384.95f, -4038.04f, 29.9118f, 3.24f },
    { 3346.77f, -4052.93f, 29.9118f, 3.22f },
    { 3299.56f, -4071.59f, 29.9118f, 3.20f },
    { 3261.22f, -4080.38f, 30.9118f, 3.19f },
    { 3220.68f, -4083.09f, 31.9118f, 3.18f },
    { 3187.11f, -4070.45f, 33.9118f, 3.16f },
    { 3162.78f, -4062.75f, 33.9118f, 3.15f },
    { 3136.09f, -4050.32f, 33.9118f, 3.07f },
    { 3119.47f, -4044.51f, 36.0363f, 3.07f },
    { 3098.95f, -4019.80f, 33.9118f, 3.07f },
    { 3073.07f, -4011.42f, 33.9118f, 3.07f },
    { 3051.71f, -3993.37f, 33.9118f, 3.02f },
    { 3027.52f, -3978.60f, 33.9118f, 3.00f },
    { 3003.78f, -3960.14f, 33.9118f, 2.98f },
    { 2977.99f, -3941.98f, 31.9118f, 2.96f },
    { 2964.57f, -3932.07f, 30.9118f, 2.96f },
    { 2947.90f, -3921.31f, 29.9118f, 2.96f },
    { 2924.91f, -3910.80f, 29.9118f, 2.94f },
    { 2903.04f, -3896.42f, 29.9118f, 2.93f },
    { 2884.75f, -3874.03f, 29.9118f, 2.90f },
    { 2868.19f, -3851.48f, 29.9118f, 2.82f },
    { 2854.62f, -3819.72f, 29.9118f, 2.80f },
    { 2825.53f, -3790.40f, 29.9118f, 2.74f },
    { 2804.31f, -3773.05f, 29.9118f, 2.71f },
    { 2769.78f, -3763.57f, 29.9118f, 2.70f },
    { 2727.23f, -3745.92f, 30.9118f, 2.69f },
    { 2680.12f, -3737.49f, 30.9118f, 2.67f },
    { 2647.62f, -3739.94f, 30.9118f, 2.66f },
    { 2616.60f, -3745.75f, 30.9118f, 2.64f },
    { 2589.38f, -3731.97f, 30.9118f, 2.61f },
    { 2562.94f, -3722.35f, 31.9118f, 2.56f },
    { 2521.05f, -3716.60f, 31.9118f, 2.55f },
    { 2485.26f, -3706.67f, 31.9118f, 2.51f },
    { 2458.93f, -3696.67f, 31.9118f, 2.51f },
    { 2432.00f, -3692.03f, 31.9118f, 2.46f },
    { 2399.59f, -3681.97f, 31.9118f, 2.45f },
    { 2357.75f, -3666.60f, 31.9118f, 2.44f },
    { 2311.99f, -3656.88f, 31.9118f, 2.94f },
    { 2263.41f, -3649.55f, 31.9118f, 3.02f },
    { 2209.05f, -3641.76f, 31.9118f, 2.99f },
    { 2164.83f, -3637.64f, 31.9118f, 3.15f },
    { 2122.42f, -3639.00f, 31.9118f, 3.21f },
    { 2075.73f, -3643.59f, 31.9118f, 3.22f },
    { 2033.59f, -3649.52f, 31.9118f, 3.42f },
    { 1985.22f, -3662.99f, 31.9118f, 3.42f },
    { 1927.09f, -3679.56f, 33.9118f, 3.42f },
    { 1873.57f, -3695.32f, 33.9118f, 3.44f }
};


class mob_rizzle_sprysprocket : public CreatureScript
{
public:
    mob_rizzle_sprysprocket() : CreatureScript("mob_rizzle_sprysprocket")
    { }

    class mob_rizzle_sprysprocketAI : public ScriptedAI
    {
        public:
        mob_rizzle_sprysprocketAI(Creature *c) : ScriptedAI(c) {}
    
        uint32 spellEscape_Timer;
        uint32 Teleport_Timer;
        uint32 Check_Timer;
        uint32 Grenade_Timer;
        uint32 Must_Die_Timer;
        uint32 CurrWP;
    
        uint64 PlayerGUID;
    
        bool Must_Die;
        bool Escape;
        bool ContinueWP;
        bool Reached;
    
        void Reset() override
        {
            spellEscape_Timer = 1300;
            Teleport_Timer = 3500;
            Check_Timer = 10000;
            Grenade_Timer = 30000;
            Must_Die_Timer = 3000;
            CurrWP = 0;
    
            PlayerGUID = 0;
    
            Must_Die = false;
            Escape = false;
            ContinueWP = false;
            Reached = false;
        }
    
        void Despawn()
        {
            me->DespawnOrUnsummon();
        }
    
        void UpdateAI(const uint32 diff) override
        {
            if(Must_Die)
            {
                if(Must_Die_Timer < diff)
                {
                    Despawn();
                    return;
                } else { Must_Die_Timer -= diff; }
            }
            
            if(!Escape)
            {
                if(!PlayerGUID)
                    return;
    
                if(spellEscape_Timer < diff)
                {
                    DoCast(me, SPELL_RIZZLE_ESCAPE, false);
                    spellEscape_Timer = 10000;
                } else spellEscape_Timer -= diff;
    
                if(Teleport_Timer < diff)
                {
                    //temp solution - unit can't be teleported by core using spelleffect 5, only players
                    Map *map = me->GetMap();
                    if(map)
                    {
                        map->CreatureRelocation(me, 3706.39, -3969.15, 35.9118, 0);
                        //me->AI_SendMoveToPacket(3706.39, -3969.15, 35.9118, 0, 0, 0);
                    }
                    //begin swimming and summon depth charges
                    Player* player = ObjectAccessor::GetPlayer(*me, PlayerGUID);
                    SendText(MSG_ESCAPE_NOTICE, player);
                    DoCast(me, SPELL_PERIODIC_DEPTH_CHARGE);
                    me->SetDisableGravity(true);
                    me->SetSwim(true);
                    me->SetSpeedRate(MOVE_RUN, 0.85f, true);
                    me->GetMotionMaster()->MovementExpired();
                    me->GetMotionMaster()->MovePoint(CurrWP, WPs[CurrWP][0], WPs[CurrWP][1], WPs[CurrWP][2]);
                    Escape = true;
                } else Teleport_Timer -= diff;
    
                return;
            }
    
            if(ContinueWP)
            {
                me->GetMotionMaster()->MovePoint(CurrWP, WPs[CurrWP][0], WPs[CurrWP][1], WPs[CurrWP][2]);
                ContinueWP = false;
            }
    
            if(Grenade_Timer < diff)
            {
                Player *player = (Player*)ObjectAccessor::GetUnit((*me), PlayerGUID);
                if(player)
                {
                   DoScriptText(SAY_RIZZLE_GRENADE, me, player);
                   DoCast(player, SPELL_RIZZLE_FROST_GRENADE, true);
                }
                Grenade_Timer = 30000;
            } else Grenade_Timer -= diff;
    
            if(Check_Timer < diff)
            {
                Player *player = (Player*)ObjectAccessor::GetUnit((*me), PlayerGUID);
                if(!player)
                {
                    Despawn();
                    return;
                }
                float dist = me->GetDistance(player);
                if(dist < 10 && me->GetPositionX() > player->GetPositionX() && !Reached)
                {
                    DoScriptText(SAY_RIZZLE_FINAL, me);
                    me->SetUInt32Value(UNIT_NPC_FLAGS, 1);
                    me->SetFaction(FACTION_FRIENDLY);
                    me->GetMotionMaster()->MoveIdle();
                    me->RemoveAurasDueToSpell(SPELL_PERIODIC_DEPTH_CHARGE);
                    Reached = true;
                }
    
                Check_Timer = 1000;
            } else Check_Timer -= diff;
    
        }
    
        void SendText(const char *text, Player* player)
        {
            WorldPacket data(SMSG_SERVER_MESSAGE, 0);              // guess size
            data << text;
            if(player)
                player->GetSession()->SendPacket(&data);
        }
    
        void AttackStart(Unit *who) override
        {
            if (!who || PlayerGUID)
                return;
    
            if(who->GetTypeId() == TYPEID_PLAYER && (who->ToPlayer())->GetQuestStatus(10994) == QUEST_STATUS_INCOMPLETE)
            {
                PlayerGUID = who->GetGUID();
                DoScriptText(SAY_RIZZLE_START, me);
                DoCast(who, SPELL_RIZZLE_BLACKJACK, false);
                return;
            }
        }
    
        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;
    
            if(id == 57)
            {
                Despawn();
                return;
            }
    
            ++CurrWP;
            ContinueWP = true;
        }
    

        virtual bool GossipHello(Player* player) override
        {
            if(player->GetQuestStatus(10994) != QUEST_STATUS_INCOMPLETE)
                return true;
            player->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, GOSSIP_GET_MOONSTONE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU_TEXTID(10811, me->GetGUID());
            return true;
        }


        virtual bool GossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            if (action == GOSSIP_ACTION_INFO_DEF + 1 && player->GetQuestStatus(10994) == QUEST_STATUS_INCOMPLETE)
            {
                player->CLOSE_GOSSIP_MENU();
                me->CastSpell(player, SPELL_GIVE_SOUTHFURY_MOONSTONE, TRIGGERED_FULL_MASK);
                ((mob_rizzle_sprysprocket::mob_rizzle_sprysprocketAI*)me->AI())->Must_Die_Timer = 3000;
                ((mob_rizzle_sprysprocket::mob_rizzle_sprysprocketAI*)me->AI())->Must_Die = true;
            }
            return true;
        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mob_rizzle_sprysprocketAI(creature);
    }
};




/*####
# mob_depth_charge
####*/


class mob_depth_charge : public CreatureScript
{
public:
    mob_depth_charge() : CreatureScript("mob_depth_charge")
    { }

    class mob_depth_chargeAI : public ScriptedAI
    {
        public:
        mob_depth_chargeAI(Creature *c) : ScriptedAI(c) {}
    
        bool we_must_die;
        uint32 must_die_timer;
    
        void Reset() override
        {
            me->SetDisableGravity(true);
            me->SetSwim(true);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            we_must_die = false;
            must_die_timer = 1000;
        }
    
        void UpdateAI(const uint32 diff) override
        {
            if(we_must_die)
            {
                if(must_die_timer < diff)
                {
                    me->DespawnOrUnsummon();
                } else { must_die_timer -= diff; }
            }
            return;
        }
    
        void MoveInLineOfSight(Unit *who) override
        {
            if (!who)
                return;
    
            if(who->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(who, 5))
            {
                DoCast(who, SPELL_DEPTH_CHARGE_TRAP);
                we_must_die = true;
                return;
            }
        }
    
        void AttackStart(Unit *who) override
        {
            return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mob_depth_chargeAI(creature);
    }
};


void AddSC_azshara()
{

    new mobs_spitelashes();

    new npc_loramus_thalipedes();

    new mob_rizzle_sprysprocket();

    new mob_depth_charge();
}

