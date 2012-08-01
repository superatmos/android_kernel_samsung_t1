cmd_security/smc/bridge_pub2sec.o := /home/fuss132/cyano/sources/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-gcc -Wp,-MD,security/smc/.bridge_pub2sec.o.d  -nostdinc -isystem /home/fuss132/cyano/sources/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include -I/home/fuss132/horsestock/sources/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-omap2/include -Iarch/arm/plat-omap/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork  -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float -gdwarf-2         -c -o security/smc/bridge_pub2sec.o security/smc/bridge_pub2sec.S

source_security/smc/bridge_pub2sec.o := security/smc/bridge_pub2sec.S

deps_security/smc/bridge_pub2sec.o := \
    $(wildcard include/config/arm/errata/430973.h) \
  /home/fuss132/horsestock/sources/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \

security/smc/bridge_pub2sec.o: $(deps_security/smc/bridge_pub2sec.o)

$(deps_security/smc/bridge_pub2sec.o):
