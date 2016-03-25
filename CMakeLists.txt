cmake_minimum_required(VERSION 2.8.12)

project(kdf)

# KDE Application Version, managed by release script
set (KDE_APPLICATIONS_VERSION_MAJOR "16")
set (KDE_APPLICATIONS_VERSION_MINOR "07")
set (KDE_APPLICATIONS_VERSION_MICRO "70")
set (KDE_APPLICATIONS_VERSION "${KDE_APPLICATIONS_VERSION_MAJOR}.${KDE_APPLICATIONS_VERSION_MINOR}.${KDE_APPLICATIONS_VERSION_MICRO}")

if(NOT UNIX)
  message(FATAL_ERROR "kdf is only supposed to be built on Unix platforms.")
endif(NOT UNIX)

set (QT_MIN_VERSION "5.2.0")
set (KF5_MIN_VERSION "5.6.0")
set(ECM_MIN_VERSION "1.6.0")

find_package(ECM ${ECM_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

include(ECMGenerateHeaders)
include(ECMInstallIcons)
include(ECMPackageConfigHelpers)
include(ECMQtDeclareLoggingCategory)
include(ECMSetupVersion)
include(GenerateExportHeader)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)

ecm_setup_version(${KDE_APPLICATIONS_VERSION} VARIABLE_PREFIX KDF
                  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kdf_version.h"
)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED
    Core
    Gui
    Widgets
)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED
    ConfigWidgets
    CoreAddons
    DocTools
    I18n
    IconThemes
    KIO
    KCMUtils
    Notifications
    WidgetsAddons
    XmlGui
)

add_subdirectory(doc)
add_subdirectory(pics)

########### next target ###############
set(libkdf_common_SRCS 
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

ecm_qt_declare_logging_category(libkdf_common_SRCS
                                HEADER kdf_debug.h
                                IDENTIFIER KDF
                                CATEGORY_NAME kdf)

ki18n_wrap_ui(libkdf_common_SRCS
    kdfconfig.ui
    mntconfig.ui )

set(kdf_SRCS kdf.cpp ${libkdf_common_SRCS} )

add_executable(kdf ${kdf_SRCS})

target_link_libraries(kdf
    Qt5::Core
    Qt5::Gui
    KF5::Completion
    KF5::I18n
    KF5::IconThemes
    KF5::KIOCore
    KF5::XmlGui
)

install(TARGETS kdf  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kwikdisk_SRCS kwikdisk.cpp ${libkdf_common_SRCS})

add_executable(kwikdisk ${kwikdisk_SRCS})

target_link_libraries(kwikdisk
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    KF5::Completion
    KF5::ConfigWidgets
    KF5::I18n
    KF5::IconThemes
    KF5::KIOCore
    KF5::KIOWidgets
    KF5::Notifications
    KF5::WidgetsAddons
)

install(TARGETS kwikdisk  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kcm_kdf_PART_SRCS kcmdf.cpp ${libkdf_common_SRCS})

add_library(kcm_kdf ${kcm_kdf_PART_SRCS})

target_link_libraries(kcm_kdf
    Qt5::Core
    Qt5::Gui
    KF5::Completion
    KF5::IconThemes
    KF5::I18n
    KF5::KIOCore
    KF5::KCMUtils
)

install(TARGETS kcm_kdf  DESTINATION ${KDE_INSTALL_PLUGINDIR})

install( FILES kcmdf.desktop DESTINATION  ${KDE_INSTALL_KSERVICES5DIR} )

########### install files ###############

install( PROGRAMS org.kde.kdf.desktop org.kde.kwikdisk.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install( FILES kdfui.rc  DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/kdf)
