# time_ms

## SYNOPSIS

    varargs int time_ms(int nanoseconds)

### PARAMETERS

    int nanoseconds - The time value in nanoseconds. Defaults to the current time in nanoseconds if not provided.

### RETURNS

    int - - The time value converted to milliseconds.

## DESCRIPTION

Converts a time value from nanoseconds to milliseconds.
This function takes a time value in nanoseconds and converts it
to milliseconds by dividing the input by 1,000,000.
