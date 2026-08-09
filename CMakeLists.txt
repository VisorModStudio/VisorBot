cmake_minimum_required(VERSION 4.2)
project(VisorBot CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(dpp CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)


find_package(SQLite3 REQUIRED)

add_executable(VisorBot main.cpp
        Command.h
        BotClient.h
        BotClient.cpp
        commands/PingCommand.cpp
        commands/BulkMessageDelete.cpp
        commands/BulkMessageDelete.h
        commands/SetUserPermission.cpp
        commands/SetUserPermission.h
        EventModule.h
        ModLogModule.cpp
        ModLogModule.h
        AuditLogEvents.h
        GeminiClient.cpp
        GeminiClient.h
        commands/SetChannel.cpp
        commands/SetChannel.h
        commands/SetFaqUrl.cpp
        commands/SetFaqUrl.h
)

target_include_directories(VisorBot PRIVATE C:/vcpkg/installed/x64-windows/include)


target_link_libraries(VisorBot PRIVATE dpp::dpp SQLite::SQLite3 cpr::cpr)