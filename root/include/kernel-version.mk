
# Use the default kernel version if the Makefile doesn't override it
LINUX_RELEASE?=1

ifdef CONFIG_TESTING_KERNEL
  KERNEL_PATCHVER:=$(KERNEL_TESTING_PATCHVER)
endif

LINUX_VERSION-5.4 = .225
LINUX_VERSION-5.15 = .86
LINUX_VERSION-6.1 = .10

LINUX_KERNEL_HASH-5.4.132 = 8466adbfb3579e751ede683496df7bb20f258b5f882250f3dd82be63736d00ef
LINUX_KERNEL_HASH-5.4.182 = b2f1201f64f010e9e3c85d6f303a559a7944a80a0244a86b8f5035bd23f1f40d
LINUX_KERNEL_HASH-5.4.188 = 9fbc8bfdc28c9fce2307bdf7cf1172c9819df673397a411c40a5c3d0a570fdbc
LINUX_KERNEL_HASH-5.4.194 = 284157891929f26f34ddd4c447980c1ce364c78df4f89b64edeac8ff9a1d3df6
LINUX_KERNEL_HASH-5.4.203 = fc933f5b13066cfa54aacb5e86747a167bad1d8d23972e4a03ab5ee36c29798a
LINUX_KERNEL_HASH-5.15.4 = 549d0fb75e65f6158e6f4becc648f249d386843da0e1211460bde8b1ea99cbca
LINUX_KERNEL_HASH-5.15.15 = 1d3c57cf8071af174933df3e5d77da801e240a59da3c5e8406f7769de2c83a5a
LINUX_KERNEL_HASH-5.15.17 = 2787f5c0cc59984902fd97916dc604f39718c73817497c25f963141bfb70abde
LINUX_KERNEL_HASH-5.15.29 = 5905e684602c47ae95746d4003cb834335e5451aca4ac7c3013f15dd49ed876e
LINUX_KERNEL_HASH-5.15.36 = 36345db17a937c197c72ca9c7f34c262b3a12f927c237ff7770193014e29c690
LINUX_KERNEL_HASH-5.15.50 = 554d507d37a23810fe8c83912761e4a4f73c40794bc685ff7ca98042fe1bd70f
LINUX_KERNEL_HASH-5.15.63 = 6dd3cd1e5a629d0002bc6c6ec7e8ea96710104f38664122dd56c83dfd4eb7341
LINUX_KERNEL_HASH-5.15.77 = 142f841f33796a84c62fae2f2b96d2120bd8bbf9e0aac4ce157692cdb0afe9f9
LINUX_KERNEL_HASH-5.15.78 = 0db99f7347a38c27b8c155f3c9c8b260011aea0a4ded85ee95e6095b1e69a499
LINUX_KERNEL_HASH-6.1 = 2ca1f17051a430f6fed1196e4952717507171acfd97d96577212502703b25deb
LINUX_KERNEL_HASH-5.15.83 = 40590843c04c85789105157f69efbd71a4efe87ae2568e40d1b7258c3f747ff3
LINUX_KERNEL_HASH-6.1.3 = 6dc89ae7a7513e433c597c7346ed7ff4bfd115ea43a3b5e27a6bdb38c5580317
LINUX_KERNEL_HASH-5.4.225 = 59f596f6714317955cf481590babcf015aff2bc1900bd8e8dc8f7af73bc560aa
LINUX_KERNEL_HASH-5.15.86 = 80fcd9efa443502de9e2750f6dfb59e8de43a5d87a6d2be09dca748d79b5f2ee
LINUX_KERNEL_HASH-6.1.8 = b60bb53ab8ba370a270454b11e93d41af29126fc72bd6ede517673e2e57b816d
LINUX_KERNEL_HASH-6.1.10 = 0be2919ba91cf5873a4cb4d429de78aad0469120d624e333a43b4b011d74d19d

remove_uri_prefix=$(subst git://,,$(subst http://,,$(subst https://,,$(1))))
sanitize_uri=$(call qstrip,$(subst @,_,$(subst :,_,$(subst .,_,$(subst -,_,$(subst /,_,$(1)))))))

ifneq ($(call qstrip,$(CONFIG_KERNEL_GIT_CLONE_URI)),)
  LINUX_VERSION:=$(call sanitize_uri,$(call remove_uri_prefix,$(CONFIG_KERNEL_GIT_CLONE_URI)))
  ifeq ($(call qstrip,$(CONFIG_KERNEL_GIT_REF)),)
    CONFIG_KERNEL_GIT_REF:=HEAD
  endif
  LINUX_VERSION:=$(LINUX_VERSION)-$(call sanitize_uri,$(CONFIG_KERNEL_GIT_REF))
else
ifdef KERNEL_PATCHVER
  LINUX_VERSION:=$(KERNEL_PATCHVER)$(strip $(LINUX_VERSION-$(KERNEL_PATCHVER)))
endif
ifdef KERNEL_TESTING_PATCHVER
  LINUX_TESTING_VERSION:=$(KERNEL_TESTING_PATCHVER)$(strip $(LINUX_VERSION-$(KERNEL_TESTING_PATCHVER)))
endif
endif

split_version=$(subst ., ,$(1))
merge_version=$(subst $(space),.,$(1))
KERNEL_BASE=$(firstword $(subst -, ,$(LINUX_VERSION)))
KERNEL=$(call merge_version,$(wordlist 1,2,$(call split_version,$(KERNEL_BASE))))
KERNEL_PATCHVER ?= $(KERNEL)

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_HASH:=$(LINUX_KERNEL_HASH-$(strip $(LINUX_VERSION)))
LINUX_KERNEL_HASH?=x
