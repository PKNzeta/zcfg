#include <stdlib.h>
#include <stdio.h>
#include "options.h"
#include "global.h"

static const char* summary =
"Showing the result with the following config file:\n"
"~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 8< ~ \n"
"%s\n"
"~ >8 ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ \n"
"Go...\n";

int Exemple_value1 = 0;
int Exemple_value2 = 0;

int main (void)
{
    printf (summary, def_cfg_file);
    load_exemple_conf ();
    printf ("Exemple_value1 = %d\nExemple_value2 = %d\n",
            Exemple_value1, Exemple_value2);
    printf ("Now testing saving preferences\n");
    Exemple_value1 = 2;
    zcfg_save_options ();

    printf ("Et voila, you probably want to `rm -R ~/.config/zcfg_exemple`\n");

    return EXIT_SUCCESS;
}
