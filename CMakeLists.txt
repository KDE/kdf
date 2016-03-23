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
    KDELibs4Support
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
    KF5::KDELibs4Support
    KF5::XmlGui)

install(TARGETS kdf  ${INSTALL_TARGETS_DEFAULT_ARGS} )


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
    KF5::KDELibs4Support
    KF5::WidgetsAddons)

install(TARGETS kwikdisk  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kcm_kdf_PART_SRCS kcmdf.cpp ${libkdf_common_SRCS})

add_library(kcm_kdf ${kcm_kdf_PART_SRCS})

target_link_libraries(kcm_kdf
    Qt5::Core
    Qt5::Gui
    KF5::Completion
    KF5::IconThemes
    KF5::I18n
    KF5::KCMUtils
    KF5::KDELibs4Support
    KF5::KIOCore)

install(TARGETS kcm_kdf  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES kcmdf.desktop DESTINATION  ${SERVICES_INSTALL_DIR} )

########### install files ###############

install( PROGRAMS org.kde.kdf.desktop org.kde.kwikdisk.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES kdfui.rc  DESTINATION ${KXMLGUI_INSTALL_DIR}/kdf)
