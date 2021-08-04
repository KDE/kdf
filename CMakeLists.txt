cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "21")
set (RELEASE_SERVICE_VERSION_MINOR "08")
set (RELEASE_SERVICE_VERSION_MICRO "0")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(kdf VERSION ${RELEASE_SERVICE_VERSION})

if(NOT UNIX)
  message(FATAL_ERROR "kdf is only supposed to be built on Unix platforms.")
endif(NOT UNIX)

set (QT_MIN_VERSION "5.15.0")
set (KF5_MIN_VERSION "5.82.0")

find_package(ECM ${KF5_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

include(ECMGenerateHeaders)
include(ECMInstallIcons)
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

#add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)

if (CMAKE_UNITY_BUILD)
	add_definitions(-DCMAKE_UNITY_BUILD)
endif()

add_subdirectory(desktop)
add_subdirectory(doc)
add_subdirectory(pics)
add_subdirectory(src)
ecm_qt_install_logging_categories(
	EXPORT KDF
        FILE kdf.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
        )
ki18n_install(po)
kdoctools_install(po)
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

