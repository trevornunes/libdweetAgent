cmake_minimum_required (VERSION 2.8)
project (DweetAgent)

add_library(dweetAgent src/cJSON.c src/dweetApi.cpp src/dweet.c)

