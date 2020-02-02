set(themestructure_SRCS
    themestructure.cpp
)

add_library(shirk_packagestructure_uipackage MODULE ${themestructure_SRCS})
target_link_libraries(shirk_packagestructure_uipackage
    KF5::I18n
    KF5::Package
)

install(TARGETS shirk_packagestructure_uipackage DESTINATION ${KDE_INSTALL_PLUGINDIR}/kpackage/packagestructure)
install(FILES shirk-uipackage.desktop DESTINATION ${KDE_INSTALL_KSERVICETYPES5DIR})
