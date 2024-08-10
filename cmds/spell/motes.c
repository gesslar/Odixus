/**
 * @file /cmds/spell/motes.c
 * @description Motes spell.
 *
 * @created 2024/08/09 - Gesslar
 * @last_modified 2024/08/09 - Gesslar
 *
 * @history
 * 2024/08/09 - Gesslar - Created
 */

inherit STD_SPELL ;

void setup() {
    set_name("motes of light") ;

    aggressive = true ;
    target_current = true ;
    sp_cost = 5.0 ;
    cooldown = 10.0 ;

    usage_text = "motes <target>" ;
    help_text = sprintf(
"Summon motes of light to strike a target. This spell costs %.1f SP and has a "
"cooldown of %.1f seconds.",
        evaluate(sp_cost), evaluate(cooldown)
    ) ;
}

void finish_motes(object tp, object victim) ;

mixed use(object tp, string arg) {
    object victim ;
    mixed result ;

    if(!victim = local_target(tp, arg, (: living($1) && $1 != $(tp) :)))
        return 1 ;

    if(!result = delay_act("motes", 2.0, assemble_call_back(
        (: finish_motes :),
        tp, victim
    ))) {
        return "You are already doing something." ;
    }

    tp->simple_action("$N $vsummon motes of light...") ;

    return 1 ;
}

void finish_motes(int status, object tp, object victim) {
    if(!status)
        return ;

    if(!same_env_check(tp, victim))
        return ;

    if(tp->can_strike(victim, "combat.spell.light")) {
        float damage = percent_of(50.0, tp->query_damage()) ;

        tp->targetted_action(
            "\e0197\eMotes of light strike $t!\eres\e", victim) ;
        tp->deliver_damage(victim, damage, "light") ;
        tp->use_skill("combat.spell.light") ;
    } else {
        tp->simple_action("The motes of light disperse harmlessly.") ;
        victim->use_skill("combat.defense.evade") ;
    }

    victim->start_attack(tp) ;
}
