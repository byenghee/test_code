test codes composed of 3 files.
main.c     : lib driver code which is using exposed lib APIs
test_lib.c : http get library using curl libs. 
test_lib.h : libs header.

test_lib is supporting StartTest(), GetInfo() for now.

To simplify implementation considering time. I put below limitation on
current code.
.StartTest() api is syncronous call and don't support concurrent tests.
so the StartTest() API blocked till test finished, 
.only 1 job structure (to save internal statistics and data) is used.
so if 2nd test excuted, previous result will be overwritten.

This limit can be overcomed by asyncronous callback, dynamic job data structure
allocation, and fork seperated working thread for test. but this kind of 
elaboration need some more time to implement.

I have tested this code on Mac + curl 7.54.0
and took around 3H excluding some curl lib setup time.

Thanks //BH Ahn
