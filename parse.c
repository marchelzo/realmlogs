#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>
#include <inttypes.h>

static void _Noreturn
fail(char const *fmt, ...)
{
        va_list ap;

        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);

        exit(EXIT_FAILURE);
}


static void
parselog(FILE *f)
{
        char line[4096];
        static bool year_set = false;
        struct tm date = { .tm_zone = "UTC" };

        while (fgets(line, sizeof line, f) != NULL) {
                int month = date.tm_mon;
                char const *s = strptime(line, "%m/%d %H:%M:%S", &date);
                if (s == NULL)
                        fail("...");

                if (date.tm_mon < month)
                        ++date.tm_year;

                if (!year_set) {
                        time_t clock = time(NULL);
                        struct tm const *l = localtime(&clock);
                        date.tm_year = l->tm_year - (date.tm_mon > l->tm_mon);
                        year_set = true;
                }

                uint64_t t = 1000 * mktime(&date) + strtoumax(s + 1, (char **)&s, 10);
                s += 2;
                printf("%"PRIu64": %s\n", t, s);
        }
}

int
main(int ac, char *av[])
{
        FILE *f = (ac > 1) ? fopen(av[1], "r") : stdin;
        if (f == NULL)
                exit(EXIT_FAILURE);

        parselog(f);

        return 0;
}
