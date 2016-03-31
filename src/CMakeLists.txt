 
########### kdfprivate library ###############

set(kdfprivate_SRCS
    kdfwidget.cpp
    kdfconfig.cpp
    kdfitemdelegate.cpp
    kdfsortproxymodel.cpp
    mntconfig.cpp
    disklist.cpp
    disks.cpp
    optiondialog.cpp
    stdoption.cpp
)

ecm_qt_declare_logging_category(kdfprivate_SRCS
                                HEADER kdfprivate_debug.h
                                IDENTIFIER KDF
                                CATEGORY_NAME kdf.private)

ki18n_wrap_ui(kdfprivate_SRCS
    kdfconfig.ui
    mntconfig.ui )

add_library(kdfprivate ${kdfprivate_SRCS})
generate_export_header(kdfprivate BASE_NAME kdfprivate)
target_link_libraries(kdfprivate PUBLIC
    Qt5::Core
    KF5::Completion
    KF5::ConfigWidgets
    KF5::I18n
    KF5::IconThemes
    KF5::KIOCore
    KF5::WidgetsAddons
)

set_target_properties(kdfprivate PROPERTIES
    VERSION ${KDFPRIVATE_VERSION_STRING}
    SOVERSION ${KDFPRIVATE_SOVERSION}
)

install(TARGETS kdfprivate ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)


########### kdf executable ###############

set(kdf_SRCS kdf.cpp)

ecm_qt_declare_logging_category(kdf_SRCS
                                HEADER kdf_debug.h
                                IDENTIFIER KDF
                                CATEGORY_NAME kdf.main)

add_executable(kdf ${kdf_SRCS})

target_link_libraries(kdf
    kdfprivate
    Qt5::Core
    KF5::XmlGui
)

install(TARGETS kdf  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### kwikdisk executable ###############

set(kwikdisk_SRCS kwikdisk.cpp)

ecm_qt_declare_logging_category(kwikdisk_SRCS
                                HEADER kwikdisk_debug.h
                                IDENTIFIER KDF
                                CATEGORY_NAME kdf.kwikdisk)

add_executable(kwikdisk ${kwikdisk_SRCS})

target_link_libraries(kwikdisk
    kdfprivate
    Qt5::Core
    KF5::KIOWidgets
    KF5::Notifications
)

install(TARGETS kwikdisk  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### kdf KCM ###############

set(kcm_kdf_PART_SRCS kcmdf.cpp)

ecm_qt_declare_logging_category(kcm_kdf_PART_SRCS
                                HEADER kcmdf_debug.h
                                IDENTIFIER KDF
                                CATEGORY_NAME kdf.kcm)

add_library(kcm_kdf ${kcm_kdf_PART_SRCS})

target_link_libraries(kcm_kdf
    kdfprivate
    Qt5::Core
)

install(TARGETS kcm_kdf  DESTINATION ${KDE_INSTALL_PLUGINDIR})
