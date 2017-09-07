/*
* BH Ahn test code to measure network statistics to specific site   
*/

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "test_lib.h"


#define MAXTEST_NO (100U)
#define MAXBUFF_SIZE (256U)
#define DEBUG (0)

#define DEBUG_PRINT(fmt, ...) \
                do { if(DEBUG) \
                printf(fmt, __VA_ARGS__); } while(0)

typedef struct 
{
    CURL *curl;
    char header[MAXBUFF_SIZE];
    long double namelookuptime_sec;
    long double totaltime_sec;
    unsigned int number_of_try;
} TestJob;

static TestJob test_job = {0};
static char target_url[] = "http://google.com";


static size_t callback(char *buffer, size_t size, size_t nitems, void *job)
{
    size_t ret = size * nitems;  
    TestJob *pJob = (TestJob *) job;

    if(NULL != pJob && '\0' == pJob->header[0])
    {
        /*just copy begining part of header on header buffer as much as buffer size*/
        size_t len = (MAXBUFF_SIZE < ret) ? MAXBUFF_SIZE : ret;

        memcpy((void *)pJob->header, (const void *)buffer, len);
        pJob->header[MAXBUFF_SIZE - 1U] = '\0';
        DEBUG_PRINT("header saved %s \n", pJob->header);
    }

    return ret;
}

static void init_jobdata(TestJob * pjob)
{
    if(NULL != pjob)
    {
        /*curl handle init at other point */
        (void)memset((void *) pjob->header, 0, MAXBUFF_SIZE);
        pjob->namelookuptime_sec = 0.0L;
        pjob->totaltime_sec = 0.0L;
        pjob->number_of_try = 0U;
    }
}

TestErrorCode StartTest(int header_count, const char * header[], int number_of_test)
{
    TestErrorCode ret = TEST_OK;
    struct curl_slist *list = NULL;

    if(NULL != test_job.curl)
    {
        ret = TEST_BUSY;
    }
    else if(header_count < 0 || number_of_test < 0 || MAXTEST_NO< number_of_test)
    {
        ret = TEST_INVALID_AGUMENTS;
    }
    else
    {
        int i = 0;

        for(i = 0; i < header_count && NULL != header[i]; i++)
        {
            DEBUG_PRINT("Debug header = %s\n", header[i]);
            list = curl_slist_append(list, header[i]);
            if(NULL == list)
            {
                ret = TEST_FAIL;
                break;
            }
        }

        /*set option, init */
        if(TEST_OK == ret)
        {
            ret = TEST_FAIL;


            test_job.curl = curl_easy_init();
            if(NULL != test_job.curl)
            {
                (void)init_jobdata(&test_job);

                if(CURLE_OK != curl_easy_setopt(test_job.curl, CURLOPT_URL, target_url))
                {
                    printf("url set fail \n");
                }
                else if(CURLE_OK != curl_easy_setopt(test_job.curl, CURLOPT_HTTPHEADER, list))
                {
                    printf("header set fail \n");
                }
                else if(CURLE_OK != curl_easy_setopt(test_job.curl, CURLOPT_HEADER, 1L))
                {
                    printf("header enable fail \n");
                }
                else if(CURLE_OK != curl_easy_setopt(test_job.curl, CURLOPT_HEADERFUNCTION, callback))
                {
                    printf("header callback set fail \n");
                }
                else if(CURLE_OK != curl_easy_setopt(test_job.curl, CURLOPT_HEADERDATA, &test_job))
                {
                    printf("header data set fail \n");
                }
                else if(CURLE_OK != curl_easy_setopt(test_job.curl, CURLOPT_VERBOSE, 0L))
                {
                    printf("verbose set fail \n");
                }
                else
                {
                    printf("curl init/set option success \n");
                    ret = TEST_OK;
                }
            }
            else 
            {
                printf("curl init fail \n");
            }
        }

        /*do test*/
        if(TEST_OK == ret)
        {
            double lookup = 0.0;
            double total = 0.0;

            for(i = 0; i < number_of_test; i++)
            {
                if(CURLE_OK != curl_easy_perform(test_job.curl))
                {
                    printf("curl perform fail \n");
                }
                else if(CURLE_OK != curl_easy_getinfo(test_job.curl, CURLINFO_NAMELOOKUP_TIME, 
                                        &lookup))
                {
                    printf("lookuptime get  fail \n");
                }
                else if(CURLE_OK != curl_easy_getinfo(test_job.curl, CURLINFO_TOTAL_TIME, 
                                        &total))
                {
                    printf("total time get fail \n");
                }
                else
                {
                    DEBUG_PRINT("Debug[%dth try] %.f, %f \n",i, lookup, total);
                    test_job.namelookuptime_sec += lookup;
                    test_job.totaltime_sec += total;
                    test_job.number_of_try++;
                }
            }

            if(test_job.number_of_try != number_of_test)
            {
                /*even though test not done request times, if some done, use that info as best effort*/
                printf("test number=%d not match requested number=%d\n", test_job.number_of_try,
                                                                         number_of_test);
            }

            if(0 == test_job.number_of_try)
                ret = TEST_FAIL;
        }
    }

    if(NULL != test_job.curl)
    {
        curl_easy_cleanup(test_job.curl);
        test_job.curl = NULL;
    }

    if(NULL != list)
    {
        curl_slist_free_all(list);
    }

    return ret;
}

TestErrorCode GetInfo(TestInfoType type, char * buf, int size_of_buf)
{
    TestErrorCode ret = TEST_OK;

    if(NULL == buf || size_of_buf <= 0)
    {
        ret = TEST_INVALID_AGUMENTS;
    }
    else if(NULL != test_job.curl)
    {
        ret = TEST_BUSY;
    }
    else
    {
        switch(type)
        {
            case TEST_NAMELOOKUP:
                snprintf(buf, size_of_buf, "namelookuptime = %.3Lf sec", 
                        (0U != test_job.number_of_try) ? 
                        test_job.namelookuptime_sec/test_job.number_of_try:0.0L);
                break;

            case TEST_TOTAL:
                snprintf(buf, size_of_buf, "totaltime = %.3Lf sec",
                        (0U != test_job.number_of_try) ? 
                        test_job.totaltime_sec/test_job.number_of_try:0.0L);
                break;

            case TEST_HEADER:
                snprintf(buf, (size_of_buf < MAXBUFF_SIZE) ? size_of_buf : MAXBUFF_SIZE, 
                         "%s", test_job.header);
                break;

            default:
                buf[0] = '\0';
                ret = TEST_INVALID_AGUMENTS;
                //printf("invalid info type = %d\n", type);
                break;
        }
    }

    return ret;
}

TestErrorCode StopTest(void)
{
    /*not supported*/
    printf("StopTest() not supported \n");
    return TEST_FAIL;
}

