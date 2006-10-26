/*
 *  TOIlet        The Other Implementation’s letters
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 * This is the main program entry point.
 */

#include "config.h"

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#endif
#if defined(HAVE_GETOPT_H)
#   include <getopt.h>
#endif
#if defined(HAVE_SYS_IOCTL_H) && defined(HAVE_TIOCGWINSZ)
#   include <sys/ioctl.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <cucul.h>

#include "toilet.h"
#include "render.h"
#include "filter.h"

static void version(void);
#if defined(HAVE_GETOPT_H)
static void usage(void);
#endif

int main(int argc, char *argv[])
{
    context_t struct_cx;
    context_t *cx = &struct_cx;

    int infocode = -1;

    cx->export = "utf8";
    cx->font = "mono9";
    cx->dir = "/usr/share/figlet/";

    cx->term_width = 80;

    cx->filters = NULL;
    cx->nfilters = 0;

#if defined(HAVE_GETOPT_H)
    for(;;)
    {
#   ifdef HAVE_GETOPT_LONG
#       define MOREINFO "Try `%s --help' for more information.\n"
        int option_index = 0;
        static struct option long_options[] =
        {
            /* Long option, needs arg, flag, short option */
            { "font", 1, NULL, 'f' },
            { "directory", 1, NULL, 'd' },
            { "width", 1, NULL, 'w' },
            { "termwidth", 0, NULL, 't' },
            { "filter", 1, NULL, 'F' },
            { "gay", 0, NULL, 130 },
            { "metal", 0, NULL, 131 },
            { "irc", 0, NULL, 140 },
            { "html", 0, NULL, 141 },
            { "tga", 0, NULL, 142 },
            { "help", 0, NULL, 'h' },
            { "infocode", 1, NULL, 'I' },
            { "version", 0, NULL, 'v' },
            { NULL, 0, NULL, 0 }
        };

        int c = getopt_long(argc, argv, "f:d:w:tF:hI:v",
                            long_options, &option_index);
#   else
#       define MOREINFO "Try `%s -h' for more information.\n"
        int c = getopt(argc, argv, "f:d:w:tF:hI:v");
#   endif
        if(c == -1)
            break;

        switch(c)
        {
        case 'h': /* --help */
            usage();
            return 0;
        case 'I': /* --infocode */
            infocode = atoi(optarg);
            break;
        case 'v': /* --version */
            version();
            return 0;
        case 'f': /* --font */
            cx->font = optarg;
            break;
        case 'd': /* --directory */
            cx->dir = optarg;
            break;
        case 'F': /* --filter */
            if(filter_add(cx, optarg))
                return -1;
            break;
        case 130: /* --gay */
            filter_add(cx, "gay");
            break;
        case 131: /* --metal */
            filter_add(cx, "metal");
            break;
        case 'w': /* --width */
            cx->term_width = atoi(optarg);
            break;
        case 't': /* --termwidth */
        {
#if defined(HAVE_SYS_IOCTL_H) && defined(HAVE_TIOCGWINSZ)
            struct winsize ws;

            if((ioctl(1, TIOCGWINSZ, &ws) != -1 ||
                ioctl(2, TIOCGWINSZ, &ws) != -1 ||
                ioctl(0, TIOCGWINSZ, &ws) != -1) && ws.ws_col != 0)
                cx->term_width = ws.ws_col;
#endif
            break;
        }
        case 140: /* --irc */
            cx->export = "irc";
            break;
        case 141: /* --html */
            cx->export = "html";
            break;
        case 142: /* --tga */
            cx->export = "tga";
            break;
        case '?':
            printf(MOREINFO, argv[0]);
            return 1;
        default:
            printf("%s: invalid option -- %i\n", argv[0], c);
            printf(MOREINFO, argv[0]);
            return 1;
        }
    }
#else
#   define MOREINFO "Usage: %s message...\n"
    int optind = 1;
#endif

    switch(infocode)
    {
        case -1:
            break;
        case 0:
            version();
            return 0;
        case 1:
            printf("20201\n");
            return 0;
        case 2:
            printf("%s\n", cx->dir);
            return 0;
        case 3:
            printf("%s\n", cx->font);
            return 0;
        case 4:
            printf("%u\n", cx->term_width);
            return 0;
        default:
            return 0;
    }

    if(render_init(cx) < 0)
        return -1;

    if(optind >= argc)
        render_stdin(cx);
    else
        render_list(cx, argc - optind, argv + optind);

    render_end(cx);
    filter_end(cx);

    return 0;
}

#if defined(HAVE_GETOPT_H)
#   define USAGE \
    "Usage: toilet [ -htv ] [ -d fontdirectory ]\n" \
    "              [ -f fontfile ] [ -F filter ] [ -w outputwidth ]\n" \
    "              [ -I infocode ] [ message ]\n"
#else
#   define USAGE ""
#endif

static void version(void)
{
    printf(
    "TOIlet Copyright 2006 Sam Hocevar\n"
    "Internet: <sam@zoy.org> Version: %s, date: %s\n"
    "\n"
    "TOIlet, along with the various TOIlet fonts and documentation, may be\n"
    "freely copied and distributed.\n"
    "\n"
    "If you use TOIlet, please send an e-mail message to <sam@zoy.org>.\n"
    "\n"
    "The latest version of TOIlet is available from the web site,\n"
    "        http://libcaca.zoy.org/toilet.html\n"
    "\n"
    USAGE,
    VERSION, DATE);
}

#if defined(HAVE_GETOPT_H)
static void usage(void)
{
    printf(USAGE);
#   ifdef HAVE_GETOPT_LONG
    printf("  -f, --font <name>        select the font\n");
    printf("  -d, --directory <dir>    specify font directory\n");
    printf("  -w, --width <width>      set output width\n");
    printf("  -t, --termwidth          adapt to terminal's width\n");
    printf("  -F, --filter <name>      apply one or several filters to the text\n");
    printf("      --gay                rainbow filter (same as -F gay)\n");
    printf("      --metal              metal filter (same as -F metal)\n");
    printf("      --irc                output IRC colour codes\n");
    printf("      --html               output an HTML document\n");
    printf("      --tga                output a TGA image\n");
    printf("  -h, --help               display this help and exit\n");
    printf("  -I, --infocode <code>    print FIGlet-compatible infocode\n");
    printf("  -v, --version            output version information and exit\n");
#   else
    printf("  -f <name>        select the font\n");
    printf("  -d <dir>         specify font directory\n");
    printf("  -w <width>       set output width\n");
    printf("  -t               adapt to terminal's width\n");
    printf("  -F <name>        apply one or several filters to the text\n");
    printf("  -h               display this help and exit\n");
    printf("  -I <code>        print FIGlet-compatible infocode\n");
    printf("  -v               output version information and exit\n");
#   endif
}
#endif

