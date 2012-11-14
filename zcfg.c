

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>

#include "zcfg.h"

/* Public data ****************************************************************/


   /* None */


/* Private constants **********************************************************/


   /* None */


/* Private structures *********************************************************/


typedef struct config_pair
{
    /* the key name in the config file */
    const char*  key_name;
    /* the values its can take */
    KEY_VALUES*  key_values;
    /* a pointer to the modified value in the program */
    int*         var_dst_int;

    struct config_pair* nxt;

} CONFIG_PAIR;

typedef struct config_line
{
    /* the line */
    char   line[128];

    /* this line is a comment/blank/invalid line or not */
    int    comment;

    /* the address of the next and the previous line */
    struct config_line* nxt;
    struct config_line* prv;

} CONFIG_LINE;


/* Private data ***************************************************************/


/* The path to the files */
static char cfg_path[256] = {'\0'};
static char cfg_path_backup[256] = {'\0'};

/* a chained list of values to be read in the config file */
static CONFIG_PAIR* keylist = NULL;

/* a double chained list of each line in the config file */
static CONFIG_LINE* cfg_save = NULL;


/* Private functions **********************************************************/


/** find the config key with the name val
 *  return : NULL if key not found
 */
static CONFIG_PAIR* zcfg__find_key (const char* val)
{
    CONFIG_PAIR* cur = keylist;
    CONFIG_PAIR* found = NULL;

    while (cur != NULL && found == NULL)
    {
        if (!strcmp (cur->key_name, val))
        {
            found = cur;
        }
        else
        {
            cur = cur->nxt;
        }
    }

    return found;
}


static const char* zcfg__keyvalue_get_value
    (KEY_VALUES* keyval, int* value)
{
    const char* found = NULL;
    int i = 0;

    while (keyval->key_values[i] != NULL && found == NULL)
    {
        if (keyval->prg_values[i] == *value)
        {
            found = keyval->key_values[i];
        }
        i++;
    }

    return found;
}

/** Apply the selected setting in program data
 *  key : the key name
 *  val : the key value
 */
static void zcfg__set_key (const char* key, const char* val)
{
    CONFIG_PAIR* pair = zcfg__find_key (key);
    int i = 0;

    if (pair != NULL)
    {
        while (i < NUM_KEY_MAX && pair->key_values->key_values[i] != NULL)
        {
            if (strcmp (pair->key_values->key_values[i], val))
            {
                i++;
            }
            else
            {
                *pair->var_dst_int = pair->key_values->prg_values[i];
                i = 999;
            }
        }
        if (i != 999)
        {
            fprintf (stderr, "[WW] invalid argument %s\n", val);
        }
    }
    else
    {
        fprintf (stderr, "[WW] %s is not a supported option\n", key);
    }
}



/** Create the default config file
 *  default_cfg_str : a string of what the default config file will contain
 */
static void zcfg__create_default_file (const char* default_cfg_str)
{
    FILE* file = fopen (cfg_path, "w");

    if (file)
    {
        fprintf (stderr, "[**] Config file created: %s\n", cfg_path);
        fputs (default_cfg_str, file);
        fclose (file);
    }
    else
    {
        WARNING ("Can't create config file.");
    }
}


/** return 1 if the config file exist, 0 if not */
static int zcfg__check_file (void)
{
    FILE* cfg_file = fopen (cfg_path, "r");

    if (cfg_file)
    {
        fclose(cfg_file);
    }

    return cfg_file? 1 : 0;
}


/** check/create the config directory */
static void zcfg__check_dir (const char* prog_name)
{
    struct passwd *pw = getpwuid(getuid());

    strncpy (cfg_path, pw->pw_dir, 127);
    strncat (cfg_path, "/.config/", 127);
    mkdir (cfg_path, S_IRWXU | S_IRWXG);
    strncat (cfg_path, prog_name, 127);
    mkdir (cfg_path, S_IRWXU | S_IRWXG);
    strncat (cfg_path, "/", 127);
    strncat (cfg_path, prog_name, 127);
    strncat (cfg_path, ".conf", 127);
    strncpy (cfg_path_backup, cfg_path, 127);
    strncat (cfg_path_backup, ".bak", 127);
}


/** Load the config file into cfg_save */
static void zcfg__buffer_load (void)
{
    char* line = malloc (128);
    FILE* cfg_file = fopen (cfg_path, "r");

    if (cfg_file != NULL)
    {
        while (fgets (line, 127, cfg_file) != NULL)
        {
            CONFIG_LINE* new_line = malloc (sizeof (CONFIG_LINE));

            /* go to the first non blank char */
            while (line[0] == ' ' || line[0] == '\0') line++;

            /* mark this line as blank or comment if it start by # or \n*/
            new_line->comment = (line[0] == '\n' || line[0] == '#')?    1 : 0;

            strncpy (new_line->line, line, 127);
            new_line->nxt = cfg_save;
            new_line->prv = NULL;
            if (cfg_save != NULL)
                new_line->nxt->prv = new_line;
            cfg_save = new_line;
        }
        free (line);
        fclose (cfg_file);
    }
    else
    {
        WARNING ("Can't read the config file, using defaults options.");
    }
}


static void zcfg__file_do_backup (void)
{
    char buf[128] = {'\0'};
    FILE* file = fopen (cfg_path, "r+");
    FILE* bak = fopen (cfg_path_backup, "w+");

    if (file && bak)
    {
        while (fgets (buf, 127, file) != NULL)
        {
            fputs (buf, bak);
        }
        fclose (file);
        fclose (bak);
    }
    else
    {
        WARNING ("Can't backup the config file.");
    }
}


static void zcfg__file_write (void)
{
    CONFIG_LINE* cur = cfg_save;
    FILE* file = NULL;

    if (cur)
    {
        zcfg__file_do_backup ();
        file = fopen (cfg_path, "w+");

        /* Go to the end of cfg_save (the first line in the config file) */
        while (cur->nxt != NULL)   cur = cur->nxt;

        if (file)
        {
            while (cur != NULL)
            {
                fputs (cur->line, file);
                cur = cur->prv;
            }
            fclose (file);
        }
        else
        {
            WARNING ("Can't open config file for writing.\n"
                     "     Your preferences will be lost.\n");
        }
    }
}


static void zcfg__buffer_save_changes (void)
{
    CONFIG_LINE* cur_line = cfg_save;
    char   key[64] = {'\0'};
    char   value[64] = {'\0'};

    while (cur_line != NULL)
    {
        if (cur_line->comment != 1)
        {
            if (sscanf (cur_line->line, "%[^=]=%s", key, value) == 2)
            {
                CONFIG_PAIR* pair = zcfg__find_key (key);

                if (pair)
                {
                    sprintf (cur_line->line, "%s=%s\n", pair->key_name,
                             zcfg__keyvalue_get_value
                                    (pair->key_values,pair->var_dst_int));
                }
                else
                {
                    cur_line->comment = 1;
                }
            }
        }
        cur_line = cur_line->nxt;
    }
}



/* Public functions ***********************************************************/

void zcfg_add_key (const char* name, KEY_VALUES* values, void* var_dst)
{
    CONFIG_PAIR* cur = keylist;

    keylist = malloc (sizeof (CONFIG_PAIR));
    keylist->key_name = name;
    keylist->key_values = values;
    keylist->var_dst_int = var_dst;
    keylist->nxt = cur;
}


void zcfg_apply_options (void)
{
    CONFIG_LINE* cur_line = cfg_save;
    char   key[64] = {'\0'};
    char   value[64] = {'\0'};

    while (cur_line != NULL)
    {
        if (cur_line->comment == 1)
        {
            cur_line = cur_line->nxt;
        }
        else
        {
            if (sscanf (cur_line->line, "%[^=]=%s", key, value) == 2)
            {
                zcfg__set_key (key, value);
            }
            else
            {
                fprintf (stderr, "[WW] Can't parse line: %s", cur_line->line);
                cur_line->comment = 1;
            }
            cur_line = cur_line->nxt;
        }
    }
}


void zcfg_save_options (void)
{
    zcfg__buffer_save_changes ();
    zcfg__file_write ();
}


void zcfg_init (const char* default_cfg_str, const char* prog_name)
{
    zcfg__check_dir (prog_name);

    if (!zcfg__check_file ())
    {
        zcfg__create_default_file (default_cfg_str);
    }
    zcfg__buffer_load ();
}
