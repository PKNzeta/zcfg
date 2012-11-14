/* util.h */
#define PROGRAM_NAME    "zcfg"
#define PROGRAM_VERSION "alpha"

#define FATAL_ERROR(str)\
    {fprintf (stderr,"[EE] %s\n",str); exit(EXIT_FAILURE);}

#define WARNING(str)\
    fprintf (stderr,"[WW] %s\n", str)

// #define DEBUG_VERBOSE
