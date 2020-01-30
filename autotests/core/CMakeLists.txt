function(add_core_test _name)
    ecm_add_test(${CMAKE_CURRENT_SOURCE_DIR}/${_name}.cpp
        LINK_LIBRARIES shirk_core Qt5::Test
        NAME_PREFIX shirk-core-
        TEST_NAME ${_name}
    )
endfunction()


add_core_test(futurepromisetest)
