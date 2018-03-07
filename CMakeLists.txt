#cmake_minimum_required (VERSION 3.2)
project (DonationMonitor)
set (DonationMonitor_VERSION_MAJOR 1)
set (DonationMonitor_VERSION MINOR 0)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Core)
find_package(Qt5Network)
find_package(Qt5Widgets)
find_package(Qt5LinguistTools)

#add_custom_target(translations ALL DEPENDS ${QM_FILES})
SET(Lang_files
  translations/donmoni_pl_PL.ts
  translations/donmoni_en_US.ts
)

#QT5_CREATE_TRANSLATION(LangSrcs ${Lang_files})

add_executable(donate main.cpp mainthread.cpp settings.cpp currencies.cpp money.cpp parser.cpp filter.cpp recur.cpp tracker.cpp project.cpp payment.cpp printer.cpp ${LangSrcs})
target_link_libraries(donate Qt5::Core ${CURSES_LIBRARIES})
target_link_libraries(donate Qt5::Widgets ${CURSES_LIBRARIES})
target_link_libraries(donate Qt5::Network ${CURSES_LIBRARIES})
