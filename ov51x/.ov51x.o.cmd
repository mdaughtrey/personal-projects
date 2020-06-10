cmd_/home/mattd/data/projects/ov51x/ov51x.o := gcc -Wp,-MD,/home/mattd/data/projects/ov51x/.ov51x.o.d -nostdinc -iwithprefix include -D__KERNEL__ -Iinclude  -Wall -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -pipe -msoft-float -mpreferred-stack-boundary=2 -fno-unit-at-a-time -march=pentium3 -Iinclude/asm-i386/mach-default -O2 -fomit-frame-pointer    -DMODULE -DKBUILD_BASENAME=ov51x -DKBUILD_MODNAME=ov51x -c -o /home/mattd/data/projects/ov51x/.tmp_ov51x.o /home/mattd/data/projects/ov51x/ov51x.c

deps_/home/mattd/data/projects/ov51x/ov51x.o := \
  /home/mattd/data/projects/ov51x/ov51x.c \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/video/proc/fs.h) \
    $(wildcard include/config/kmod.h) \
  include/linux/config.h \
    $(wildcard include/config/h.h) \
  include/linux/version.h \
  include/linux/module.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/kallsyms.h) \
  include/linux/sched.h \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/preempt.h) \
  include/asm/param.h \
  include/linux/capability.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  include/asm/posix_types.h \
  include/asm/types.h \
    $(wildcard include/config/highmem64g.h) \
    $(wildcard include/config/lbd.h) \
  include/linux/compiler.h \
  include/linux/compiler-gcc3.h \
  include/linux/compiler-gcc.h \
  include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
  include/linux/preempt.h \
  include/linux/linkage.h \
  include/asm/linkage.h \
    $(wildcard include/config/x86/alignment/16.h) \
  include/linux/thread_info.h \
  include/linux/bitops.h \
  include/asm/bitops.h \
  include/asm/thread_info.h \
    $(wildcard include/config/4kstacks.h) \
    $(wildcard include/config/debug/stack/usage.h) \
  include/asm/page.h \
    $(wildcard include/config/x86/use/3dnow.h) \
    $(wildcard include/config/x86/pae.h) \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/highmem4g.h) \
    $(wildcard include/config/discontigmem.h) \
  include/asm/processor.h \
    $(wildcard include/config/mk8.h) \
    $(wildcard include/config/mk7.h) \
    $(wildcard include/config/sched/smt.h) \
  include/asm/vm86.h \
  include/asm/math_emu.h \
  include/asm/sigcontext.h \
  include/asm/segment.h \
  include/asm/cpufeature.h \
  include/asm/msr.h \
  include/asm/system.h \
    $(wildcard include/config/x86/cmpxchg.h) \
    $(wildcard include/config/x86/oostore.h) \
  include/linux/kernel.h \
    $(wildcard include/config/debug/spinlock/sleep.h) \
  /usr/lib/gcc-lib/i486-pc-linux-gnu/3.3/include/stdarg.h \
  include/asm/byteorder.h \
    $(wildcard include/config/x86/bswap.h) \
  include/linux/byteorder/little_endian.h \
  include/linux/byteorder/swab.h \
  include/linux/byteorder/generic.h \
  include/asm/bug.h \
  include/linux/cache.h \
  include/asm/cache.h \
    $(wildcard include/config/x86/l1/cache/shift.h) \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
  include/linux/stringify.h \
  include/asm/atomic.h \
  include/linux/timex.h \
    $(wildcard include/config/time/interpolation.h) \
  include/asm/timex.h \
    $(wildcard include/config/x86/elan.h) \
    $(wildcard include/config/x86/tsc.h) \
  include/linux/time.h \
  include/linux/seqlock.h \
  include/asm/div64.h \
  include/linux/jiffies.h \
  include/linux/rbtree.h \
  include/linux/cpumask.h \
    $(wildcard include/config/hotplug/cpu.h) \
  include/linux/bitmap.h \
  include/linux/string.h \
  include/asm/string.h \
  include/asm/semaphore.h \
  include/linux/wait.h \
  include/linux/list.h \
  include/linux/prefetch.h \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  include/asm/rwsem.h \
  include/asm/ptrace.h \
  include/asm/mmu.h \
  include/linux/smp.h \
  include/linux/sem.h \
    $(wildcard include/config/sysvipc.h) \
  include/linux/ipc.h \
  include/asm/ipcbuf.h \
  include/asm/sembuf.h \
  include/linux/signal.h \
  include/asm/signal.h \
  include/asm/siginfo.h \
  include/asm-generic/siginfo.h \
  include/linux/securebits.h \
  include/linux/fs_struct.h \
  include/linux/completion.h \
  include/linux/pid.h \
  include/linux/percpu.h \
  include/linux/slab.h \
    $(wildcard include/config/.h) \
  include/linux/gfp.h \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
  include/linux/numa.h \
  include/linux/topology.h \
  include/asm/topology.h \
  include/asm-generic/topology.h \
  include/linux/kmalloc_sizes.h \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/large/allocs.h) \
  include/asm/percpu.h \
  include/asm-generic/percpu.h \
  include/linux/param.h \
  include/linux/resource.h \
  include/asm/resource.h \
  include/linux/timer.h \
  include/linux/aio.h \
  include/linux/workqueue.h \
  include/linux/aio_abi.h \
  include/asm/current.h \
  include/linux/stat.h \
  include/asm/stat.h \
  include/linux/kmod.h \
    $(wildcard include/config/hotplug.h) \
  include/linux/errno.h \
  include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  include/linux/elf.h \
  include/asm/elf.h \
  include/asm/user.h \
  include/linux/utsname.h \
  include/linux/kobject.h \
  include/linux/sysfs.h \
    $(wildcard include/config/sysfs.h) \
  include/linux/moduleparam.h \
  include/linux/init.h \
  include/asm/local.h \
  include/asm/module.h \
    $(wildcard include/config/m386.h) \
    $(wildcard include/config/m486.h) \
    $(wildcard include/config/m586.h) \
    $(wildcard include/config/m586tsc.h) \
    $(wildcard include/config/m586mmx.h) \
    $(wildcard include/config/m686.h) \
    $(wildcard include/config/mpentiumii.h) \
    $(wildcard include/config/mpentiumiii.h) \
    $(wildcard include/config/mpentiumm.h) \
    $(wildcard include/config/mpentium4.h) \
    $(wildcard include/config/mk6.h) \
    $(wildcard include/config/mcrusoe.h) \
    $(wildcard include/config/mwinchipc6.h) \
    $(wildcard include/config/mwinchip2.h) \
    $(wildcard include/config/mwinchip3d.h) \
    $(wildcard include/config/mcyrixiii.h) \
    $(wildcard include/config/mviac3/2.h) \
    $(wildcard include/config/regparm.h) \
  include/linux/vmalloc.h \
  include/linux/ctype.h \
  include/linux/pagemap.h \
  include/linux/mm.h \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/debug/pagealloc.h) \
    $(wildcard include/config/arch/gate/area.h) \
  include/linux/prio_tree.h \
  include/linux/fs.h \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/auditsyscall.h) \
  include/linux/limits.h \
  include/linux/kdev_t.h \
  include/linux/ioctl.h \
  include/asm/ioctl.h \
  include/linux/dcache.h \
  include/linux/rcupdate.h \
  include/linux/radix-tree.h \
  include/linux/audit.h \
    $(wildcard include/config/audit.h) \
  include/linux/quota.h \
  include/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/nfs_fs_i.h \
  include/linux/nfs.h \
  include/linux/sunrpc/msg_prot.h \
  include/linux/fcntl.h \
  include/asm/fcntl.h \
  include/linux/err.h \
  include/asm/pgtable.h \
    $(wildcard include/config/highpte.h) \
  include/asm/fixmap.h \
    $(wildcard include/config/x86/local/apic.h) \
    $(wildcard include/config/x86/io/apic.h) \
    $(wildcard include/config/x86/visws/apic.h) \
    $(wildcard include/config/x86/f00f/bug.h) \
    $(wildcard include/config/x86/cyclone/timer.h) \
    $(wildcard include/config/acpi/boot.h) \
    $(wildcard include/config/pci/mmconfig.h) \
  include/asm/acpi.h \
    $(wildcard include/config/acpi/pci.h) \
    $(wildcard include/config/acpi/sleep.h) \
  include/asm/apicdef.h \
  include/asm/pgtable-2level-defs.h \
  include/asm/pgtable-2level.h \
  include/asm-generic/pgtable.h \
  include/linux/page-flags.h \
    $(wildcard include/config/swap.h) \
  include/linux/highmem.h \
  include/asm/cacheflush.h \
  include/asm/uaccess.h \
    $(wildcard include/config/x86/intel/usercopy.h) \
    $(wildcard include/config/x86/wp/works/ok.h) \
  /home/mattd/data/projects/ov51x/ov51x.h \
  include/linux/videodev.h \
  include/linux/videodev2.h \
  include/linux/poll.h \
  include/asm/poll.h \
  include/linux/device.h \
  include/linux/ioport.h \
  include/linux/pm.h \
    $(wildcard include/config/pm.h) \
  include/linux/smp_lock.h \
  include/linux/usb.h \
  include/linux/mod_devicetable.h \
  include/linux/usb_ch9.h \
    $(wildcard include/config/size.h) \
    $(wildcard include/config/att/one.h) \
    $(wildcard include/config/att/selfpower.h) \
    $(wildcard include/config/att/wakeup.h) \
  include/linux/delay.h \
  include/asm/delay.h \
  include/linux/interrupt.h \
  include/asm/hardirq.h \
  include/linux/irq.h \
    $(wildcard include/config/arch/s390.h) \
  include/asm/irq.h \
  include/asm-i386/mach-default/irq_vectors.h \
  include/asm-i386/mach-default/irq_vectors_limits.h \
    $(wildcard include/config/pci/msi.h) \
  include/asm/hw_irq.h \
  include/linux/profile.h \
    $(wildcard include/config/profiling.h) \
  include/asm/sections.h \
  include/asm-generic/sections.h \
  include/linux/irq_cpustat.h \
  include/linux/kref.h \

/home/mattd/data/projects/ov51x/ov51x.o: $(deps_/home/mattd/data/projects/ov51x/ov51x.o)

$(deps_/home/mattd/data/projects/ov51x/ov51x.o):
