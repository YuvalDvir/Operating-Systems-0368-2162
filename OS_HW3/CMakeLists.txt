cmake_minimum_required(VERSION 3.15)
project(OS_HW3 C)

set(CMAKE_C_STANDARD 99)

add_executable(OS_HW3 message_slot.c message_slot.h message_sender.c message_reader.c)