/**
 * @file /std/living/decision.c
 * @description Decision making for living objects.
 *
 * Converted from: https://www.npmjs.com/package/utility-ai
 *
 * @created 2023-06-18 - Gesslar
 * @last_modified 2023-06-18 - Gesslar
 *
 * @history
 * 2023-06-18 - Gesslar - Created
 */

class Score {
  string description;
  function callback;
}

class Decision {
  string description;
  function condition;
  class Score *scores;
}

private nosave class Decision *decisions = ({});
private nosave mapping decision_funcs = ([]);

void add_decision(string description, function callback) {
  class Decision decision;

  if(!description)
    error("ai_add_action: Missing description");
  if(!callback)
    error("ai_add_action: Missing callback");

  decision = new(class Decision);
  decision.description = description;
  decision.condition = callback;
  decision.scores = ({});

  decisions += ({ decision });
}

void add_func(string description, function func) {
  if(!description)
    error("ai_add_func: Missing description");
  if(!func)
    error("ai_add_func: Missing function");

  decision_funcs[description] = func;
}

void modify_condition(string decision_desc, function callback) {
  class Decision decision;

  if(!callback)
    error("ai_modify_condition: Missing callback");

  foreach(decision in decisions) {
    if(decision.description == decision_desc) {
      decision.condition = callback;
      break;
    }
  }
}

void add_score(string decision_desc, string score_desc, function callback) {
  class Decision decision;
  class Score score;

  if(!score_desc)
    error("ai_add_score: Missing description");
  if(!callback)
    error("ai_add_score: Missing callback");

  foreach(decision in decisions) {
    if(decision.description == decision_desc) {
      score = new(class Score);
      score.description = score_desc;
      score.callback = callback;

      decision.scores += ({ score });
      break;
    }
  }
}

void modify_score(string decision_desc, string score_desc, function new_callback) {
  class Decision decision;
  class Score score;

  if(!score_desc)
    error("ai_modify_score: Missing score description");
  if(!new_callback)
    error("ai_modify_score: Missing new callback");

  foreach(decision in decisions) {
    if(decision.description == decision_desc) {
      foreach(score in decision.scores) {
        if(score.description == score_desc) {
          score.callback = new_callback;
          break;
        }
      }
      break;
    }
  }
}

int evaluate_decision(class Decision decision, mapping data) {
  class Score score;
  int total_score = 0;

  if(!evaluate(decision.condition, data)) return -MAX_INT;

  foreach(score in decision.scores) {
    total_score += evaluate(score.callback, data);
  }

  return total_score;
}

mapping decide(mapping data) {
  mapping best_decision = ([ "decision": "", "score": -MAX_INT, "func": null ]);
  class Decision decision;
  int decision_score;

  foreach(decision in decisions) {
    decision_score = evaluate_decision(decision, data);
    if(decision_score > best_decision["score"]) {
      best_decision["decision"] = decision.description;
      best_decision["score"] = decision_score;
      best_decision["func"] = decision_funcs[decision.description];
    }
  }

  return best_decision;
}

/*
void setup_utility_ai() {
  // if stunned, remove_stun
  add_decision("remove_stun", (: is_stunned :));
  add_score("remove_stun", "priority", (: 100 :)); // High score as we want to prioritize removing stun
  add_func("remove_stun", (: remove_stun :));

  // if more than one opponent, cast fireball
  add_decision("cast_fireball", (: num_combatants :));
  add_score("cast_fireball", "priority", (: 50 + random(10) :)); // Medium score as this is less priority than removing stun
  add_func("cast_fireball", (: cast_fireball :));

  // regular single-attack called lightning_bolt
  add_decision("cast_lightning", (: 1 :)); // The condition is always true, lightning can always be cast
  add_score("cast_lightning", "priority", (: 50 + random(10) :)); // Similar score to fireball with a small variation
  add_func("cast_lightning", (: cast_lightning :));
}

mapping combat_logic(mapping data) {
  return decide(data);
}
*/
