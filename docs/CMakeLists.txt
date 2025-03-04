if (NOT WIN32)
    find_program(PANDOC_EXECUTABLE NAMES pandoc)

    set(MAN_NAMES awsmockmgr.1 awsmockctl.1 awsmockcfg.1 awslocal.1 awsmocksqs.1 awsmocks3.1 awsmocksns.1 awsmocklambda.1
            awsmockdynamodb.1 awsmockcognito.1 awsmocktransfer.1 awsmocksecretsmanager.1 awsmockkms.1 awsmockssm.1)
    set(MAN_FILES)

    if (BUILD_DOCUMENTATION)
        set(doxyfile ./Doxyfile)
        find_package(Doxygen)
        if (NOT DOXYGEN_FOUND)
            message(FATAL_ERROR "Doxygen is needed to build the documentation.")
        endif ()
        add_custom_target(docs COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Generating API documentation with Doxygen" VERBATIM)
    endif ()

    foreach (m IN LISTS MAN_NAMES)
        set(mf ${CMAKE_CURRENT_SOURCE_DIR}/man/${m})
        set(ms ${CMAKE_CURRENT_SOURCE_DIR}/man/${m}.md)
        set(version ${PROJECT_VERSION})
        set(date ${PROJECT_BUILDDATE})
        add_custom_command(OUTPUT ${mf}
                COMMAND ${PANDOC_EXECUTABLE} ${ms} --template ${ms} -V version=${version} -V builddate=${date} | ${PANDOC_EXECUTABLE} -s -t man -o ${mf}
                DEPENDS ${ms}
                COMMENT "Building manpage ${mf}"
                VERBATIM)
        list(APPEND MAN_FILES ${mf})
    endforeach ()

    add_custom_target(man ALL DEPENDS ${MAN_FILES})

    # Installation
    install(FILES man/awsmockctl.1 DESTINATION /usr/local/share/man/man1 PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
    install(FILES man/awsmockcfg.1 DESTINATION /usr/local/share/man/man1 PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
    install(FILES man/awsmockmgr.1 DESTINATION /usr/local/share/man/man1 PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
    install(FILES man/awslocal.1 DESTINATION /usr/local/share/man/man1 PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
    install(FILES man/awsmocksqs.1 DESTINATION /usr/local/share/man/man1 PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
    install(FILES man/awsmocksns.1 DESTINATION /usr/local/share/man/man1 PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
    install(FILES man/awsmocks3.1 DESTINATION /usr/local/share/man/man1 PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
    install(FILES man/awsmocklambda.1 DESTINATION /usr/local/share/man/man1 PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
    install(FILES man/awsmocktransfer.1 DESTINATION /usr/local/share/man/man1 PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
    install(FILES man/awsmockcognito.1 DESTINATION /usr/local/share/man/man1 PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
    install(FILES man/awsmockdynamodb.1 DESTINATION /usr/local/share/man/man1 PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
    install(FILES man/awsmocksecretsmanager.1 DESTINATION /usr/local/share/man/man1 PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
    install(FILES man/awsmockkms.1 DESTINATION /usr/local/share/man/man1 PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
    install(FILES man/awsmockssm.1 DESTINATION /usr/local/share/man/man1 PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
endif()