cmake_minimum_required(VERSION 2.8.12)

project(kdf)

# KDE Application Version, managed by release script
set (KDE_APPLICATIONS_VERSION_MAJOR "16")
set (KDE_APPLICATIONS_VERSION_MINOR "12")
set (KDE_APPLICATIONS_VERSION_MICRO "3")
set (KDE_APPLICATIONS_VERSION "${KDE_APPLICATIONS_VERSION_MAJOR}.${KDE_APPLICATIONS_VERSION_MINOR}.${KDE_APPLICATIONS_VERSION_MICRO}")

if(NOT UNIX)
  message(FATAL_ERROR "kdf is only supposed to be built on Unix platforms.")
endif(NOT UNIX)

set (QT_MIN_VERSION "5.2.0")
set (KF5_MIN_VERSION "5.6.0")
set (ECM_MIN_VERSION "5.14.0")

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
                  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/kdf_version.h"
)

ecm_setup_version(${KDE_APPLICATIONS_VERSION} VARIABLE_PREFIX KDFPRIVATE
    SOVERSION ${KDE_APPLICATIONS_VERSION_MAJOR}
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

add_subdirectory(desktop)
add_subdirectory(doc)
add_subdirectory(pics)
add_subdirectory(src)
