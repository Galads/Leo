cmake_minimum_required(VERSION 3.15)
project(keil)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(RTE)
include_directories(RTE/_Target_1)
include_directories(RTE/Device)
include_directories(RTE/Device/STM32F100RB)
include_directories(RTE/Device/STM32F103RB)

add_executable(keil
        RTE/_Target_1/RTE_Components.h
        RTE/Device/STM32F100RB/RTE_Device.h
        RTE/Device/STM32F100RB/system_stm32f10x.c
        RTE/Device/STM32F103RB/RTE_Device.h
        RTE/Device/STM32F103RB/system_stm32f10x.c
        EventRecorderStub.scvd
        RCC.uvguix.Niko
        RCC.uvoptx
        RCC.uvprojx
        rccLab1.c)
