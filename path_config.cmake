macro(CM2DSetup CM2D_ROOT_PATH)
    message("CM2D root path: " ${CM2D_ROOT_PATH})

    set(CM2D_ROOT_INCLUDE_PATH ${CM2D_ROOT_PATH}/inc)
    set(CM2D_ROOT_SOURCE_PATH ${CM2D_ROOT_PATH}/src)

    include_directories(${CM2D_ROOT_INCLUDE_PATH})
endmacro()