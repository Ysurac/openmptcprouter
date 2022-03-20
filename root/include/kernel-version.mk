
# Use the default kernel version if the Makefile doesn't override it
LINUX_RELEASE?=1

ifdef CONFIG_TESTING_KERNEL
  KERNEL_PATCHVER:=$(KERNEL_TESTING_PATCHVER)
endif

LINUX_VERSION-5.4 = .182
LINUX_VERSION-5.10 = .64
LINUX_VERSION-5.14 = .6
LINUX_VERSION-5.15 = .29

LINUX_KERNEL_HASH-5.4.132 = 8466adbfb3579e751ede683496df7bb20f258b5f882250f3dd82be63736d00ef
LINUX_KERNEL_HASH-5.4.182 = b2f1201f64f010e9e3c85d6f303a559a7944a80a0244a86b8f5035bd23f1f40d
LINUX_KERNEL_HASH-5.10.64 = 3eb84bd24a2de2b4749314e34597c02401c5d6831b055ed5224adb405c35e30a
LINUX_KERNEL_HASH-5.14.6 = 54848c1268771ee3515e4c33e29abc3f1fa90d8144894cce6d0ebc3b158bccec
LINUX_KERNEL_HASH-5.15.4 = 549d0fb75e65f6158e6f4becc648f249d386843da0e1211460bde8b1ea99cbca
LINUX_KERNEL_HASH-5.15.15 = 1d3c57cf8071af174933df3e5d77da801e240a59da3c5e8406f7769de2c83a5a
LINUX_KERNEL_HASH-5.15.17 = 2787f5c0cc59984902fd97916dc604f39718c73817497c25f963141bfb70abde
LINUX_KERNEL_HASH-5.15.29 = 5905e684602c47ae95746d4003cb834335e5451aca4ac7c3013f15dd49ed876e

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
