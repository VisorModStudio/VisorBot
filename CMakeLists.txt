cmake_minimum_required(VERSION 3.25)
project(VisorBot CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(dpp CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(SQLite3 REQUIRED)

add_executable(VisorBot src/main.cpp
        src/Command.h
        src/BotClient.h
        src/BotClient.cpp
        src/commands/PingCommand.cpp
        src/commands/BulkMessageDelete.cpp
        src/commands/BulkMessageDelete.h
        src/EventModule.h
        src/services/ModLogModule.cpp
        src/services/ModLogModule.h
        src/AuditLogEvents.h
        src/services/GeminiClient.cpp
        src/services/GeminiClient.h
        src/commands/SetServerConfigValue.cpp
        src/commands/SetServerConfigValue.h
        src/services/KnowledgeCache.cpp
        src/services/KnowledgeCache.h
        src/commands/SetKnowledgeSource.cpp
        src/commands/SetKnowledgeSource.h
        src/services/UrlSafety.cpp
        src/services/UrlSafety.h
        src/services/Logfetcher.cpp
        src/services/Logfetcher.h
)

target_include_directories(VisorBot PRIVATE
        src
        src/commands
        src/services
)

add_compile_definitions(NOMINMAX)

target_link_libraries(VisorBot PRIVATE dpp::dpp SQLite::SQLite3 cpr::cpr)