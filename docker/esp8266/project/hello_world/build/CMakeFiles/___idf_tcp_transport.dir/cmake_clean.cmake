file(REMOVE_RECURSE
  "config/sdkconfig.h"
  "config/sdkconfig.cmake"
  "bootloader/bootloader.elf"
  "bootloader/bootloader.bin"
  "bootloader/bootloader.map"
  "hello-world.bin"
  "hello-world.map"
  "project_elf_src.c"
  "CMakeFiles/___idf_tcp_transport"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/___idf_tcp_transport.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
