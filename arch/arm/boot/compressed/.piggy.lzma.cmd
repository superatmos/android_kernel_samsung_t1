cmd_arch/arm/boot/compressed/piggy.lzma := (cat arch/arm/boot/compressed/../Image | lzma -9 && printf \324\212\200\000) > arch/arm/boot/compressed/piggy.lzma || (rm -f arch/arm/boot/compressed/piggy.lzma ; false)
