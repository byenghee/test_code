#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "test_lib.h"

#define MAX_BUF (256U)
#define MAX_HEADER (10U)
 
int main(int argc, char * argv[])
{

    int o = 0;
    char buf[MAX_BUF] = {0};
    char * headerv[MAX_HEADER];
    int header_index = 0;
    int number_of_test = 0;
    TestErrorCode ret = TEST_OK;

    while(-1 != (o = getopt(argc, argv, "H:n:")))
    {
        switch(o)
        {
            case 'H':
                headerv[header_index++] = optarg;
                if(MAX_HEADER <= header_index)
                {
                    printf("too many headers \n");
                    header_index = 0;
                }
                break;

            case 'n':
                number_of_test = atoi(optarg);
                break;

            default:
                printf("unknown option %c \n", o);
                break;
        }
    }

    ret = StartTest(header_index, (const char **)headerv, number_of_test);
    if(TEST_OK == ret)
    {
        for(int i = TEST_NAMELOOKUP; i < TEST_LAST_TYPE; i++)
        {
            ret = GetInfo((TestInfoType) i, buf, MAX_BUF);
            if(TEST_OK == ret)
                printf("%s;", buf);
        }

        printf("\n");
    }

    return 0;
}