project(Wizard-unit-tests)

set(TEST_SOURCE_FILES
        card.cpp
        deck.cpp
       # hand.cpp
        player.cpp
        trick.cpp)

add_executable(Wizard-tests ${TEST_SOURCE_FILES})

target_compile_definitions(Wizard-tests PRIVATE WIZARD_SERVER=1 RAPIDJSON_HAS_STDSTRING=1)

target_link_libraries(Wizard-tests gtest gtest_main Wizard-lib)

