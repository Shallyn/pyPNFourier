/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#include "utils_OptionParser.h"
#include "basic_Error.h"
#include <stdio.h>

#define SWAP_FLAGS(ch1, ch2)
#define my_index strchr

CHAR *g_optarg;
INT4 g_optind = 1;
INT4 g_optopt = '?';

static enum
{
    REQUIRE_ORDER, PERMUTE, RETURN_IN_ORDER
} ordering;


static INT4 __getopt_initialized;
static CHAR *nextchar;
static INT4 first_nonopt;
static INT4 last_nonopt;

static void
exchange(CHAR **argv)
{
    INT4 bottom = first_nonopt;
    INT4 middle = last_nonopt;
    INT4 top = g_optind;
    CHAR *tem;
    
    /*
     * Exchange the shorter segment with the far end of the longer segment.
     * That puts the shorter segment into the right place.
     * It leaves the longer segment in the right place overall,
     * but it consists of two parts that need to be swapped next.
     */
    
    while(top > middle && middle > bottom)
    {
        if (top - middle > middle - bottom)
        {
            /* Bottom segment is the shorter one. */
            INT4 len = middle - bottom;
            register INT4 i;
            for(i = 0; i < len; i++)
            {
                tem = argv[bottom + 1];
                argv[bottom + i] = argv[top - (middle - bottom) + i];
                argv[top - (middle - bottom) + i] = tem;
                SWAP_FLAGS(bottom + i, top - (middle - bottom) + i);
            }
            /* Exclude the moved bottom segment from further swapping */
            top -= len;
        }
        else
        {
            /* Top segment is the shorter one. */
            INT4 len = top - middle;
            register INT4 i;
            for(i = 0; i < len; i++)
            {
                tem = argv[bottom + i];
                argv[bottom + i] = argv[middle + i];
                argv[middle + i] = tem;
                SWAP_FLAGS(bottom + i, middle + i);
            }
            bottom += len;
        }
    }
    /* Update records for the slots the non-options now occupy */
    first_nonopt += (g_optind - last_nonopt);
    last_nonopt = g_optind;
}

static const CHAR *
_getopt_initialize(INT4 argc, CHAR *const *argv, const CHAR *optstring)
{
    /* Start processing options with argv-element 1;
     * the sequence of previously skipped non-option argv-elements is empty.
     */
    argc = 0;
    argv = NULL;
    first_nonopt = last_nonopt = g_optind;
    nextchar = NULL;
    
    /* Determine how to handle the ordering of options and nonoptions. */
    if (optstring[0] == '-')
    {
        ordering = RETURN_IN_ORDER;
        ++optstring;
    }
    else if (optstring[0] == '+')
    {
        ordering = REQUIRE_ORDER;
        ++optstring;
    }
    else
        ordering = PERMUTE;
    
    return optstring;
    (void)argc;
    (void)argv;
}

static INT4
_getopt_internal(INT4 argc, CHAR *const *argv, const CHAR *optstring,
                            const OptionParserArg *longopts, INT4 *longind, INT4 long_only)
{
    INT4 print_errors = 1;
    if (optstring[0] == ':')
        print_errors = 0;
    if (argc < 1)
        return X_FAILURE;
    
    // Check initialization
    g_optarg = NULL;
    if (g_optind == 0 || !__getopt_initialized)
    {
        if (g_optind == 0)
            g_optind = 1; // Skip argv[0]
        optstring = _getopt_initialize(argc, argv, optstring);
        __getopt_initialized = 1;
    }
    /* Test whether argv[g_optind] points to a non-option argument.
     * Either it does not have option syntax, or there is an enviroment flag
     * from the shell indicating it is not an option.
     * The later information is only used when the used in the GNU libc.
     */
#define NONOPTION_P (argv[g_optind][0] != '-' || argv[g_optind][1] == '\0')
    
    if (nextchar == NULL || *nextchar == '\0')
    {
        if (last_nonopt > g_optind)
            last_nonopt = g_optind;
        if (first_nonopt > g_optind)
            first_nonopt = g_optind;
        
        if (ordering == PERMUTE)
        {
            /* If we have just processed some options following some non-options,
             * exchange them so that the options come first.
             */
            union {CHAR *const *pcs; CHAR **ps;} bad = { argv };
            if (first_nonopt != last_nonopt && last_nonopt != g_optind)
                exchange( (CHAR **) bad.ps );
            else if (last_nonopt != g_optind)
                first_nonopt = g_optind;
            
            /*
             * Skip any additional non-options and extend the range of non-options previously skipped.
             */
            
            while (g_optind < argc && NONOPTION_P)
                g_optind++;
            last_nonopt = g_optind;
        }
        
        /*
         * The special ARGV-element `--' means premature end of options.
         * Skip it like a null option,
         * then exchange with previous non-options as if it were an option,
         *then skip everything else like a non-option.
         */
        if (g_optind != argc && !strcmp(argv[g_optind], "--"))
        {
            union {CHAR *const *pcs; CHAR **ps;} bad = {argv};
            g_optind++;
            
            if (first_nonopt != last_nonopt && last_nonopt != g_optind)
                exchange((CHAR **) bad.ps);
            else if (first_nonopt == last_nonopt)
                first_nonopt = g_optind;
            last_nonopt = argc;
            g_optind = argc;
        }
        
        /*
         * If we have done all the ARGV-elements,
         * stop the scan and back over any non-options
         * that we skipped and permuted.
         */
        if (g_optind == argc)
        {
            /*
             * Set the next-arg-index to point at the non-options
             * that we previously skipped, so the caller will digest them.
             */
            if (first_nonopt != last_nonopt)
                g_optind = first_nonopt;
            return -1;
        }
        
        /*
         * If we have come to a non-option and did not permute it,
         * either stop the scan or describe it to the caller and pass it by.
         */
        if (NONOPTION_P)
        {
            if (ordering == REQUIRE_ORDER)
                return -1;
            g_optarg = argv[g_optind++];
            return 1;
        }
        /*
         * We have found another option-ARGV-element.
         * Skip the initial punctuation.
         */
        nextchar = (argv[g_optind] + 1 +
                    (longopts != NULL && argv[g_optind][1] == '-'));

    }
    /* Decode the current option-ARGV-element.  */
    
    /*
     * Check whether the ARGV-element is a long option.
     *
     * If long_only and the ARGV-element has the form "-f", where f is
     * a valid short option, don't consider it an abbreviated form of
     * a long option that starts with f.  Otherwise there would be no
     * way to give the -f short option.
     *
     * On the other hand, if there's a long option "fubar" and
     * the ARGV-element is "-fu", do consider that an abbreviation of
     * the long option, just like "--fu", and not "-f" with arg "u".
     *
     * This distinction seems to be the most useful approach.
     */
    
    if(longopts != NULL &&
       (argv[g_optind][1] == '-' || (long_only && (argv[g_optind][2] || !my_index(optstring, argv[g_optind][1]) ) ) ))
    {
        CHAR *nameend;
        const OptionParserArg *p;
        const OptionParserArg *pfound = NULL;
        INT4 exact = 0;
        INT4 ambig = 0;
        INT4 indfound = -1;
        INT4 option_index;
        
        for(nameend = nextchar; *nameend && *nameend != '='; nameend ++);
        
        for (p = longopts, option_index = 0; p->name; p++, option_index++ )
        {
            if (!strncmp( p->name, nextchar, nameend - nextchar ))
            {
                if ((UINT4)(nameend - nextchar) == (UINT4)strlen(p->name))
                {
                    /* Exact match */
                    pfound = p;
                    indfound = option_index;
                    exact = 1;
                    break;
                }
                else if (pfound == NULL)
                {
                    /* First nonexact match */
                    pfound = p;
                    indfound = option_index;
                }
                else if(long_only
                        || pfound->has_arg != p->has_arg
                        || pfound->flag != p->flag
                        || pfound->val != p->val)
                    /* Second or later nonexact match */
                    ambig = 1;
            }
        }
        
        if (ambig && !exact)
        {
            if (print_errors)
                fprintf(stderr, "%s: option '%s' is ambiguous\n", argv[0], argv[g_optind]);
            nextchar += strlen(nextchar);
            g_optind++;
            g_optopt = 0;
            return '?';
        }
        
        if (pfound != NULL)
        {
            option_index = indfound;
            g_optind++;
            if(*nameend)
            {
                if (pfound->has_arg)
                    g_optarg = nameend + 1;
                else
                {
                    if (print_errors)
                    {
                        if (argv[g_optind - 1][1] == '-')
                            fprintf(stderr, "%s: option '--%s' doesn't allow an argument\n", argv[0], pfound->name);
                        else
                            fprintf(stderr, "%s: option '%c%s' doesn't allow an argument\n", argv[0], argv[g_optind - 1][0], pfound->name);
                    }
                    nextchar += strlen(nextchar);
                    g_optopt = pfound->val;
                    return '?';
                }
            }
            else if(pfound->has_arg == 1)
            {
                if (g_optind < argc)
                    g_optarg = argv[g_optind++];
                else
                {
                    if (print_errors)
                        fprintf(stderr, "%s: option '%s' requires an argument\n", argv[0], argv[g_optind - 1]);
                    nextchar += strlen(nextchar);
                    g_optopt = pfound->val;
                    return optstring[0] == ':' ? ':' : '?';
                }
            }
            nextchar += strlen(nextchar);
            if(longind != NULL)
                *longind = option_index;
            if (pfound->flag)
            {
                *(pfound->flag) = pfound->val;
                return 0;
            }
            return pfound->val;
        }
        /*
         * Can't find it as a long option.
         * If this is not getopt_long_only,
         * or the option starts with '--'
         * or is not a valid short option, then it's an error.
         * Otherwise interpret it as a short option.
         */
        
        if (!long_only ||
            argv[g_optind][1] == '-'||
            my_index (optstring, *nextchar) == NULL)
        {
            union {const CHAR *cs; CHAR *c;} wtf = { "" };
            if (print_errors)
            {
                if (argv[g_optind][1] == '-')
                    /* --option */
                    fprintf(stderr, "%s: unrecognized option '--%s'\n", argv[0], nextchar);
                else
                    /* +option or -option */
                    fprintf(stderr, "%s: unrecognized option '%c%s'\n", argv[0], argv[g_optind][0], nextchar);
            }
            nextchar = wtf.c;
            g_optind++;
            g_optopt = 0;
            return '?';
        }
        
    }

    /* Look at and handle the next short option-character.  */
    {
        CHAR c = *nextchar++;
        const CHAR *temp = my_index(optstring, c);
        if (*nextchar == '\0')
            ++g_optind;
        
        if (temp == NULL ||  c == ':')
        {
            if (print_errors)
            {
                fprintf(stderr, "%s: invalid option --%c\n", argv[0], c);
            }
            g_optopt = c;
            return '?';
        }
        if (temp[0] == 'W' && temp[1] == ';')
        {
            CHAR *nameend;
            const OptionParserArg *p;
            const OptionParserArg *pfound = NULL;
            INT4 exact = 0;
            INT4 ambig = 0;
            INT4 indfound = 0;
            INT4 option_index;
            
            /* This is an option that requires an argument. */
            if (*nextchar != '\0')
            {
                g_optarg = nextchar;
                g_optind++;
            }
            else if (g_optind == argc)
            {
                if (print_errors)
                    fprintf(stderr, "%s: option requires an argument -- %c\n", argv[0], c);
                g_optopt = c;
                if(optstring[0] == ':')
                    c = ':';
                else
                    c = '?';
                return c;
            }
            else
            /*
             * We already incremented `LALoptind' once;
             * increment it again when taking next ARGV-elt as argument.
             */
                g_optarg = argv[g_optind++];
            /*
             *LALoptarg is now the argument, see if it's in the
             * table of longopts.
             */
            for (nextchar = nameend = g_optarg; *nameend && *nameend != '='; nameend++);
            
            /*
             * Test all long options for either exact match or abbreviated matches.
             */
            for (p = longopts, option_index = 0; p->name; p++, option_index++)
            {
                if (!strncmp(p->name, nextchar, nameend-nextchar))
                {
                    if ((UINT4)(nameend - nextchar) == strlen(p->name))
                    {
                        /* Exact match */
                        pfound = p;
                        indfound = option_index;
                        exact = 1;
                        break;
                    }
                    else if (pfound == NULL)
                    {
                        /* First nonexact match */
                        pfound = p;
                        indfound = option_index;
                    }
                    else
                        ambig = 1;
                }
            }
            if (ambig && !exact)
            {
                if (print_errors)
                    fprintf(stderr, "%s: option '-W %s' is ambiguous\n", argv[0], argv[g_optind]);
                nextchar += strlen(nextchar);
                g_optind++;
                return '?';
            }
            if (pfound != NULL)
            {
                option_index = indfound;
                if (*nameend)
                {
                    if (pfound->has_arg)
                        g_optarg = nameend + 1;
                    else
                    {
                        if (print_errors)
                            fprintf(stderr, "%s: option '-W %s' doesn't allow an argument\n", argv[0], pfound->name);
                        nextchar += strlen(nextchar);
                        return '?';
                    }
                }
                else if (pfound->has_arg == 1)
                {
                    if (g_optind < argc)
                        g_optarg = argv[g_optind++];
                    else
                    {
                        if(print_errors)
                            fprintf(stderr, "%s: option '%s' requires an argument", argv[0], argv[g_optind-1]);
                        return optstring[0] == ':' ? ':' : '?';
                    }
                }
                nextchar += strlen(nextchar);
                if (longind != NULL)
                    *longind = option_index;
                if (pfound->flag)
                {
                    *(pfound->flag) = pfound->val;
                    return 0;
                }
                return pfound->val;
            }
            nextchar = NULL;
            return 'W';
        }
        if (temp[1] == ':')
        {
            if (temp[2] == ':')
            {
                if (*nextchar != '\0')
                {
                    g_optarg = nextchar;
                    g_optind++;
                }
                else
                    g_optarg = NULL;
                nextchar = NULL;
            }
            else
            {
                if (*nextchar != '\0')
                {
                    g_optarg = nextchar;
                    g_optind++;
                }
                else if (g_optind == argc)
                {
                    if (print_errors)
                        fprintf(stderr, "%s: option requires an argument -- %c\n", argv[0], c);
                    g_optopt = c;
                    if(optstring[0] == ':')
                        c = ':';
                    else
                        c = '?';
                }
                else
                    g_optarg = argv[g_optind++];
                nextchar = NULL;
            }
        }
        return c;
    }
}

INT4 getopt_long_only(INT4 argc, CHAR *const *argv, const CHAR *options,
                     const OptionParserArg *opt, INT4 *opt_index)
{
    return _getopt_internal(argc, argv, options, opt, opt_index,1);
}

#undef SWAP_FLAGS
#undef my_index