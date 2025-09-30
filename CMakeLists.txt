cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "25")
set (RELEASE_SERVICE_VERSION_MINOR "08")
set (RELEASE_SERVICE_VERSION_MICRO "2")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(kdf VERSION ${RELEASE_SERVICE_VERSION})

if(NOT UNIX)
  message(FATAL_ERROR "kdf is only supposed to be built on Unix platforms.")
endif(NOT UNIX)

set (QT_MIN_VERSION "6.5.0")
set (KF_MIN_VERSION "6.3")

find_package(ECM ${KF_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

include(ECMGenerateHeaders)
include(ECMInstallIcons)
include(ECMQtDeclareLoggingCategory)
include(ECMSetupVersion)
include(GenerateExportHeader)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMDeprecationSettings)
include(KDEGitCommitHooks)
include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})


ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX KDF
                  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/kdf_version.h"
)

ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX KDFPRIVATE
    SOVERSION ${RELEASE_SERVICE_VERSION_MAJOR}
)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED
    Core
    Gui
    Widgets
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED
    ConfigWidgets
    CoreAddons
    I18n
    IconThemes
    JobWidgets
    KIO
    KCMUtils
    Service
    Notifications
    WidgetsAddons
    XmlGui
    StatusNotifierItem
    Crash
)


find_package(KF6DocTools ${KF_MIN_VERSION})
set_package_properties(KF6DocTools PROPERTIES DESCRIPTION
    "Tools to generate documentation"
    TYPE OPTIONAL
)
add_definitions(-DQT_NO_CONTEXTLESS_CONNECT)
ecm_set_disabled_deprecation_versions(QT 6.8.0
    KF 6.14.0
)

if (CMAKE_UNITY_BUILD)
	add_definitions(-DCMAKE_UNITY_BUILD)
endif()

add_subdirectory(desktop)
add_subdirectory(pics)
add_subdirectory(src)
ecm_qt_install_logging_categories(
	EXPORT KDF
        FILE kdf.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
        )
ki18n_install(po)
if(KF6DocTools_FOUND)
    kdoctools_install(po)
    add_subdirectory(doc)
endif()
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

