add_executable (dweetWriter dweetWriter.cpp)
target_link_libraries(dweetWriter LINK_PUBLIC dweetAgent)
target_link_libraries(dweetWriter LINK_PUBLIC curl)

add_executable (dweetReader dweetReader.cpp)
target_link_libraries(dweetReader LINK_PUBLIC dweetAgent)
target_link_libraries(dweetReader LINK_PUBLIC curl)

