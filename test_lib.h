/*
* BH Ahn test lib API to measure network statistics to specific site	
*/

typedef enum
{
	TEST_OK,
	TEST_BUSY,
	TEST_INVALID_AGUMENTS,
	TEST_FAIL,
	TEST_LAST_ERRORCODE,
} TestErrorCode;

typedef enum
{
	TEST_NAMELOOKUP,
	TEST_TOTAL,
	TEST_HEADER,
	TEST_LAST_TYPE,
} TestInfoType;

/*start network test 
 * API is syncrous call, function would be blocked till whole tests ended or failed
 *
 * header_count : header char array size
 * header    	: char * array to pass header strings 
 * number_of_test : number of try ,100 times is MAX limit 
 * return       : TEST_OK, TEST_INVALID_AGUMENTS, TEST_FAIL, TEST_BUSY (another test running) */
TestErrorCode StartTest(int header_count, const char * header[], int number_of_test);


/* Get statistic info for previous test
 * info from this API are result of previous test
 *
 * type 		: queried info type
 * buf  (out)   : buffer to save info. info will be saved as null terminated string.
 * size_of_buf  : buffer size, if info exceed buffer size, string will be truncated to fit buffer size.
 * return       : TEST_OK, TEST_INVALID_AGUMENTS, TEST_BUSY (another test running) */
TestErrorCode GetInfo(TestInfoType type, char * buf, int size_of_buf);

TestErrorCode StopTest(void);