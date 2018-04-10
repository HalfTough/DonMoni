#cmake_minimum_required (VERSION 3.2)
project (MonEx)
set (MonEx_VERSION_MAJOR 1)
set (MonEx_VERSION_MINOR 0)
set (MonEx_VERSION_PATCH 0)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Core)
find_package(Qt5Network)
find_package(Qt5Widgets)
find_package(Qt5LinguistTools)

#add_custom_target(translations ALL DEPENDS ${QM_FILES})
SET(Lang_files
  translations/monex_pl_PL.ts
  translations/monex_en_US.ts
)

#QT5_CREATE_TRANSLATION(LangSrcs ${Lang_files})

add_executable(monex main.cpp mainthread.cpp settings.cpp currencies.cpp money.cpp parser.cpp filter.cpp recur.cpp tracker.cpp project.cpp payment.cpp printer.cpp ${LangSrcs})
target_link_libraries(monex Qt5::Core ${CURSES_LIBRARIES})
target_link_libraries(monex Qt5::Widgets ${CURSES_LIBRARIES})
target_link_libraries(monex Qt5::Network ${CURSES_LIBRARIES})
add_definitions( -DVERSION_MAJOR=${MonEx_VERSION_MAJOR} )
add_definitions( -DVERSION_MINOR=${MonEx_VERSION_MINOR} )
add_definitions( -DVERSION_PATCH=${MonEx_VERSION_PATCH} )
