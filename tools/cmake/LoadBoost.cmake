
function(load_boost)
    set(Boost_NO_WARN_NEW_VERSIONS ON)
    set(Boost_USE_MULTITHREADED ON)
    find_package(Boost REQUIRED COMPONENTS wave)
    include_directories(${Boost_INCLUDE_DIRS})
endfunction()
