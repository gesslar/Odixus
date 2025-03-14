#include <daemons.h>

varargs void simple_action(mixed msg, mixed *obs...) {
    ACTION_D->simple_action(msg, obs...);
}

varargs void my_action(mixed msg, mixed *obs...) {
    ACTION_D->my_action(msg, obs...);
}

varargs void other_action(mixed msg, mixed *obs...) {
    ACTION_D->other_action(msg, obs...);
}

varargs void my_target_action(mixed msg, object target, mixed *obs...) {
    ACTION_D->my_target_action(msg, target, obs...);
}

varargs void target_action(mixed msg, object target, mixed *obs...) {
    ACTION_D->target_action(msg, target, obs...);
}

varargs void targetted_action(mixed msg, object target, mixed *obs...) {
    ACTION_D->targetted_action(msg, target, obs...);
}
