#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

#undef unix
struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = __stringify(KBUILD_MODNAME),
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0x4cdc490c, "cleanup_module" },
	{ 0xcd58c830, "init_module" },
	{ 0xc7ad9005, "struct_module" },
	{ 0x1ffaffc6, "mem_map" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0xc9ea30e4, "usb_altnum_to_altsetting" },
	{ 0xed1031c3, "autoremove_wake_function" },
	{ 0x2471f754, "video_device_release" },
	{ 0x65524e57, "video_usercopy" },
	{ 0x3de88ff0, "malloc_sizes" },
	{ 0x2240b497, "boot_cpu_data" },
	{ 0x24cc1de4, "no_llseek" },
	{ 0x2fd1d81c, "vfree" },
	{ 0x1d26aa98, "sprintf" },
	{ 0x753853f2, "usb_unlink_urb" },
	{ 0x9fa27f1c, "remap_page_range" },
	{ 0x9263e7c, "video_register_device" },
	{ 0xec5c44e0, "video_device_alloc" },
	{ 0x8135c226, "usb_deregister" },
	{ 0x1b7d4074, "printk" },
	{ 0x6a1ffe6c, "video_unregister_device" },
	{ 0x6c8ef280, "usb_set_interface" },
	{ 0x2f287f0d, "copy_to_user" },
	{ 0xaa076d8d, "usb_control_msg" },
	{ 0x28c3bbf5, "__down_failed_interruptible" },
	{ 0x707f93dd, "preempt_schedule" },
	{ 0x2b401436, "usb_submit_urb" },
	{ 0x6a6f0027, "kmem_cache_alloc" },
	{ 0x4784e424, "__get_free_pages" },
	{ 0xf533d09, "video_devdata" },
	{ 0x4292364c, "schedule" },
	{ 0x17d59d01, "schedule_timeout" },
	{ 0xa0b04675, "vmalloc_32" },
	{ 0x1f5c57d1, "wake_up_process" },
	{ 0xbf706d0a, "usb_register" },
	{ 0x9941ccb8, "free_pages" },
	{ 0xbce89f77, "__wake_up" },
	{ 0x72270e35, "do_gettimeofday" },
	{ 0x37a0cba, "kfree" },
	{ 0x2e60bace, "memcpy" },
	{ 0xf323ac3b, "prepare_to_wait" },
	{ 0x47da1ca6, "finish_wait" },
	{ 0xd22b546, "__up_wakeup" },
	{ 0x591ba9fe, "usb_ifnum_to_if" },
	{ 0x25da070, "snprintf" },
	{ 0x8235805b, "memmove" },
	{ 0xd5990f11, "vmalloc_to_page" },
	{ 0xf2520b76, "__down_failed" },
	{ 0x3b2aeb6b, "usb_free_urb" },
	{ 0xe66ea72a, "usb_alloc_urb" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=videodev";

MODULE_ALIAS("usb:v05A9p0511dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9pA511dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9p0518dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9pA518dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9p0519dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9p1519dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9p2519dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9p3519dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9p4519dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9p5519dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9p6519dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9p7519dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9p8519dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9p9519dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9pA519dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9pB519dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9pC519dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9pD519dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9pE519dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05A9pF519dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp0154dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp0155dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0813p0002dl*dh*dc*dsc*dp*ic*isc*ip*");
