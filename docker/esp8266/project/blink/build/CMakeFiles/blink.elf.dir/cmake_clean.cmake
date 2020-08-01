file(REMOVE_RECURSE
  "config/sdkconfig.h"
  "config/sdkconfig.cmake"
  "bootloader/bootloader.elf"
  "bootloader/bootloader.bin"
  "bootloader/bootloader.map"
  "blink.bin"
  "blink.map"
  "project_elf_src.c"
  "project_elf_src.c"
  "CMakeFiles/blink.elf.dir/project_elf_src.c.obj"
  "blink.elf.pdb"
  "blink.elf"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/blink.elf.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
