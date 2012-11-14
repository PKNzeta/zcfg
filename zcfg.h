#ifndef H__ZCFG
#define H__ZCFG

#define NUM_KEY_MAX 128

typedef struct
{
    /* a NULL terminated list of value that the
     * key can take in the config file */
    const char*    key_values[NUM_KEY_MAX];
    /* the corresponding values in the program */
    int            prg_values[NUM_KEY_MAX];

} KEY_VALUES;

/** add a key that zcfg can handle
 *  name : the key name
 *  values : the values that the key can take (a KEY_VALUES).
 *  var_dst : a pointer to the value that will be modified in the program
 */
void zcfg_add_key (const char* name, KEY_VALUES* values, void* var_dst);


/** Once all aption has been added to the parser, user must call this function
 *  for applying all the settings of the config file
 */
void zcfg_apply_options (void);

/** You can also save the state of the program in the configuration file
 *  for exemple when a value is set to 1 in the config file but for some reason
 *  since the loading of the config file the value in the program as been
 *  changed to 2, then the corresponding value will be set in the config file.
 */
void zcfg_save_options (void);

/** this will init all things that zcfg want, this also create the
 *  config file with the default value and its directory if it does not exist
 *  default_cfg_str : is what's in the default config file
 *  prog_name : is the program name (for directory and filename)
 */
void zcfg_init (const char* default_cfg_str, const char* prog_name);

#endif /* H__ZCFG */
