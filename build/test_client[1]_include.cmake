if(EXISTS "/home/gchamore/42Paris/projects_on_duty/irc/build/test_client[1]_tests.cmake")
  include("/home/gchamore/42Paris/projects_on_duty/irc/build/test_client[1]_tests.cmake")
else()
  add_test(test_client_NOT_BUILT test_client_NOT_BUILT)
endif()
