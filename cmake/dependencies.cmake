
set(WK_SERIALIZATION_JSON ON)
set(WK_HASHING_XXHASH ON)

FetchContent_Declare(
    WorkshopCore
    GIT_REPOSITORY https://github.com/sc-workshop/Workshop-Core.git
    GIT_TAG main
)
FetchContent_MakeAvailable(WorkshopCore)