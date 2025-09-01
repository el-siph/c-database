#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[])
{
    printf("Usage: %s -n -f <database file>\n", argv[0]);
    printf("\t -n - create new database file\n");
    printf("\t -f - (required) path to database file");
    return;
}

int main(int argc, char *argv[])
{
    char *filepath = NULL;           // if: creating new or loading existing database (required)
    bool newfile = false;            // if: creating new database
    int c;                           // input flag
    int dbfd = -1;                   // database file descriptor
    struct dbheader_t *dbhdr = NULL; // points to new database header, when created

    while ((c = getopt(argc, argv, "nf:")) != -1)
    {
        switch (c)
        {
        case 'n':
            newfile = true;
            break;
        case 'f':
            filepath = optarg;
            break;
        case '?':
            printf("Unknown option -%c\n", c);
            break;
        default:
            return -1;
        }
    }

    if (filepath == NULL)
    {
        printf("Filepath is required.");
        print_usage(argv);
    }

    if (newfile)
    {
        int dbfd = create_db_file(filepath);
        if (dbfd == STATUS_ERROR)
        {
            printf("Unable to create database file\n");
            return -1;
        }

        // if (create_db_header(dbfd, &dbhdr) == STATUS_ERROR)
        if (create_db_header(&dbhdr) == STATUS_ERROR)
        {
            printf("Failed to create new database header\n");
            return -1;
        }
        else
        {
            dbfd = open_db_file(filepath);
            if (dbfd == STATUS_ERROR)
            {
                printf("Unable to open database file\n");
                return -1;
            }

            if (validate_db_header(dbfd, &dbhdr) == STATUS_ERROR)
            {
                printf("Failed to validate database header\n");
                return -1;
            }
        }
    }

    printf("new file: %d\n", newfile);
    printf("filepath %s\n", filepath);

    struct employee_t *e = malloc(sizeof(struct employee_t));

    output_file(dbfd, dbhdr, e);

    return 0;
}
