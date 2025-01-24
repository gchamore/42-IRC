add_test([=[ClientTest.NicknameTest]=]  /home/gchamore/42Paris/projects_on_duty/irc/build/test_client [==[--gtest_filter=ClientTest.NicknameTest]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ClientTest.NicknameTest]=]  PROPERTIES WORKING_DIRECTORY /home/gchamore/42Paris/projects_on_duty/irc/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[ClientTest.UsernameTest]=]  /home/gchamore/42Paris/projects_on_duty/irc/build/test_client [==[--gtest_filter=ClientTest.UsernameTest]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ClientTest.UsernameTest]=]  PROPERTIES WORKING_DIRECTORY /home/gchamore/42Paris/projects_on_duty/irc/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[ClientTest.AuthenticationTest]=]  /home/gchamore/42Paris/projects_on_duty/irc/build/test_client [==[--gtest_filter=ClientTest.AuthenticationTest]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ClientTest.AuthenticationTest]=]  PROPERTIES WORKING_DIRECTORY /home/gchamore/42Paris/projects_on_duty/irc/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[ClientTest.InvalidNicknameTest]=]  /home/gchamore/42Paris/projects_on_duty/irc/build/test_client [==[--gtest_filter=ClientTest.InvalidNicknameTest]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ClientTest.InvalidNicknameTest]=]  PROPERTIES WORKING_DIRECTORY /home/gchamore/42Paris/projects_on_duty/irc/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[ClientTest.InvalidUsernameTest]=]  /home/gchamore/42Paris/projects_on_duty/irc/build/test_client [==[--gtest_filter=ClientTest.InvalidUsernameTest]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ClientTest.InvalidUsernameTest]=]  PROPERTIES WORKING_DIRECTORY /home/gchamore/42Paris/projects_on_duty/irc/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[ClientTest.SetNicknameTwice]=]  /home/gchamore/42Paris/projects_on_duty/irc/build/test_client [==[--gtest_filter=ClientTest.SetNicknameTwice]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ClientTest.SetNicknameTwice]=]  PROPERTIES WORKING_DIRECTORY /home/gchamore/42Paris/projects_on_duty/irc/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[ClientTest.SetUsernameTwice]=]  /home/gchamore/42Paris/projects_on_duty/irc/build/test_client [==[--gtest_filter=ClientTest.SetUsernameTwice]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ClientTest.SetUsernameTwice]=]  PROPERTIES WORKING_DIRECTORY /home/gchamore/42Paris/projects_on_duty/irc/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[ClientTest.GetFDTest]=]  /home/gchamore/42Paris/projects_on_duty/irc/build/test_client [==[--gtest_filter=ClientTest.GetFDTest]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ClientTest.GetFDTest]=]  PROPERTIES WORKING_DIRECTORY /home/gchamore/42Paris/projects_on_duty/irc/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[ClientTest.AuthenticateTwice]=]  /home/gchamore/42Paris/projects_on_duty/irc/build/test_client [==[--gtest_filter=ClientTest.AuthenticateTwice]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ClientTest.AuthenticateTwice]=]  PROPERTIES WORKING_DIRECTORY /home/gchamore/42Paris/projects_on_duty/irc/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[ClientTest.NicknameNotSet]=]  /home/gchamore/42Paris/projects_on_duty/irc/build/test_client [==[--gtest_filter=ClientTest.NicknameNotSet]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ClientTest.NicknameNotSet]=]  PROPERTIES WORKING_DIRECTORY /home/gchamore/42Paris/projects_on_duty/irc/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[ClientTest.UsernameNotSet]=]  /home/gchamore/42Paris/projects_on_duty/irc/build/test_client [==[--gtest_filter=ClientTest.UsernameNotSet]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ClientTest.UsernameNotSet]=]  PROPERTIES WORKING_DIRECTORY /home/gchamore/42Paris/projects_on_duty/irc/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[ClientTest.NegativeSocketFD]=]  /home/gchamore/42Paris/projects_on_duty/irc/build/test_client [==[--gtest_filter=ClientTest.NegativeSocketFD]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ClientTest.NegativeSocketFD]=]  PROPERTIES WORKING_DIRECTORY /home/gchamore/42Paris/projects_on_duty/irc/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  test_client_TESTS ClientTest.NicknameTest ClientTest.UsernameTest ClientTest.AuthenticationTest ClientTest.InvalidNicknameTest ClientTest.InvalidUsernameTest ClientTest.SetNicknameTwice ClientTest.SetUsernameTwice ClientTest.GetFDTest ClientTest.AuthenticateTwice ClientTest.NicknameNotSet ClientTest.UsernameNotSet ClientTest.NegativeSocketFD)
