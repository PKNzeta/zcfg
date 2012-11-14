#include <stdlib.h>
#include "options.h"

/* this is the default config file. It is created if it does not exist */
const char* def_cfg_file =
"#TEST CONFIG FILE\n"
"EXEMPLE_VALUE1=TEST1\n"
"EXEMPLE_VALUE2=TEST2\n"
"UNKNOWN_OPTION=TEST1\n"
"EXEMPLE_VALUE1=UNKNOWN_VALUE\n"
"invalid line\n";


static KEY_VALUES opt_exemple_value = {
    {"TEST1", "TEST2", NULL},
    {1, 2}
};

void load_exemple_conf (void)
{
    zcfg_init (def_cfg_file, PROGRAM_NAME);
    zcfg_add_key ("EXEMPLE_VALUE1", &opt_exemple_value, &Exemple_value1);
    zcfg_add_key ("EXEMPLE_VALUE2", &opt_exemple_value, &Exemple_value2);
    zcfg_apply_options ();
}
