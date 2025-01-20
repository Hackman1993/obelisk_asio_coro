vcpkg_from_git(
        OUT_SOURCE_PATH SOURCE_PATH
        URL https://github.com/Hackman1993/Sahara.git
        REF 356245c536cebadb70413da7a6bdc446a6ea3525
)

vcpkg_cmake_configure(SOURCE_PATH ${SOURCE_PATH})
vcpkg_cmake_install()

vcpkg_cmake_config_fixup()
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
#file(INSTALL "${SOURCE_PATH}/LICENSE"
#        DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
#        RENAME copyright)