cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(kdf)

# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "21")
set (RELEASE_SERVICE_VERSION_MINOR "03")
set (RELEASE_SERVICE_VERSION_MICRO "70")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

if(NOT UNIX)
  message(FATAL_ERROR "kdf is only supposed to be built on Unix platforms.")
endif(NOT UNIX)

set (QT_MIN_VERSION "5.12.0")
set (KF5_MIN_VERSION "5.70.0")

find_package(ECM ${KF5_MIN_VERSION} CONFIG REQUIRED)
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

ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX KDF
                  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/kdf_version.h"
)

ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX KDFPRIVATE
    SOVERSION ${RELEASE_SERVICE_VERSION_MAJOR}
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

add_definitions(
    -DQT_NO_FOREACH
    -DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT
)
#add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)

if (CMAKE_UNITY_BUILD)
	add_definitions(-DCMAKE_UNITY_BUILD)
endif()

add_subdirectory(desktop)
add_subdirectory(doc)
add_subdirectory(pics)
add_subdirectory(src)
install(FILES kdf.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

