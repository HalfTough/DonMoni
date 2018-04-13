cmake_minimum_required (VERSION 3.2)
project (MonEx)
set (MonEx_VERSION_MAJOR 1)
set (MonEx_VERSION_MINOR 4)
set (MonEx_VERSION_PATCH 0)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Core)
find_package(Qt5Network)
find_package(Qt5Widgets)
find_package( Qt5LinguistTools )

SET(TS_FILES
  translations/monex.pl_PL.ts
  translations/monex.en_US.ts
)

qt5_add_translation(QM_FILES ${TS_FILES})
#QT5_CREATE_TRANSLATION(LANG_FILES ${TS_FILES})
add_custom_target(translations DEPENDS ${QM_FILES})

add_executable(monex main.cpp mainthread.cpp settings.cpp currencies.cpp money.cpp parser.cpp filter.cpp recur.cpp tracker.cpp project.cpp payment.cpp printer.cpp ${QM_FILES} )
target_link_libraries(monex Qt5::Core ${CURSES_LIBRARIES})
target_link_libraries(monex Qt5::Widgets ${CURSES_LIBRARIES})
target_link_libraries(monex Qt5::Network ${CURSES_LIBRARIES})
add_definitions( -DVERSION_MAJOR=${MonEx_VERSION_MAJOR} )
add_definitions( -DVERSION_MINOR=${MonEx_VERSION_MINOR} )
add_definitions( -DVERSION_PATCH=${MonEx_VERSION_PATCH} )

if(NOT QT_TRANSLATIONS_DIR)
# If this directory is missing, we are in a Qt5 environment.
# Extract the qmake executable location
get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
# Ask Qt5 where to put the translations
execute_process( COMMAND ${QT5_QMAKE_EXECUTABLE} -query QT_INSTALL_TRANSLATIONS
OUTPUT_VARIABLE qt_translations_dir OUTPUT_STRIP_TRAILING_WHITESPACE )
# make sure we have / and not \ as qmake gives on windows
file( TO_CMAKE_PATH "${qt_translations_dir}" qt_translations_dir)
set( QT_TRANSLATIONS_DIR ${qt_translations_dir} CACHE PATH
"The location of the Qt translations" FORCE)
endif()

install(TARGETS monex DESTINATION bin)
install(FILES ${QM_FILES} DESTINATION ${QT_TRANSLATIONS_DIR})
