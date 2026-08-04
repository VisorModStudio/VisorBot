cmake_minimum_required(VERSION 4.2)
project(VisorBot CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(dpp CONFIG REQUIRED)


find_package(SQLite3 REQUIRED)

add_executable(VisorBot main.cpp
        Command.h
        BotClient.h
        BotClient.cpp
        commands/PingCommand.cpp
        commands/SetModChannel.cpp
        commands/SetModChannel.h
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
        commands/SetHelpChannel.cpp
        commands/SetHelpChannel.h
        commands/SetIssueChannel.cpp
        commands/SetIssueChannel.h
        commands/SetFaqChannel.cpp
        commands/SetFaqChannel.h
        commands/SetIssueResponseChannel.cpp
        commands/SetIssueResponseChannel.h
)

target_include_directories(VisorBot PRIVATE C:/vcpkg/installed/x64-windows/include)


target_link_libraries(VisorBot PRIVATE dpp::dpp SQLite::SQLite3)