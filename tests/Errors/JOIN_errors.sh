#!/bin/bash

exec 3<>/dev/tcp/localhost/6667

echo -ne "PASS test\r\n" >&3
sleep 1
echo -ne "NICK  testnick\r\n" >&3
sleep 1
echo -ne "USER testuser\r\n" >&3
sleep 1

echo -ne "JOIN \r\n" >&3
sleep 1

echo -ne "JOIN testchannel\r\n" >&3
sleep 1

echo -ne "JOIN #\r\n" >&3
sleep 1

echo -ne "JOIN #testchanneltestchannelestchanneltestchannelestchanneltestchannel\r\n" >&3
sleep 1

echo -ne "JOIN #test\nchannel\r\n" >&3
sleep 1

echo -ne "JOIN #test1,#test2,#test3,#test4,#test5,#test6,#test7,#test8,#test9,#test10,#test11\r\n" >&3
sleep 1	

echo -ne "PART #test1,#test2,#test3,#test4\r\n" >&3
sleep 1

echo -ne "JOIN #testpassword password\r\n" >&3

exec 4<>/dev/tcp/localhost/6667

echo -ne "PASS test\r\n" >&4
sleep 1
echo -ne "NICK  reguser\r\n" >&4
sleep 1
echo -ne "USER reguser\r\n" >&4
sleep 1

echo -ne "JOIN #testpassword wrongpassword\r\n" >&4
sleep 1

echo -ne "JOIN #testpassword password\r\n" >&4
sleep 1

echo -ne "QUIT \r\n" >&4
exec 4<&-  # Close input stream
exec 4>&-  # Close output stream

echo -ne "QUIT \r\n" >&3
exec 3<&-  # Close input stream
exec 3>&-  # Close output stream

