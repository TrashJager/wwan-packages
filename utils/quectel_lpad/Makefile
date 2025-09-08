include $(TOPDIR)/rules.mk

PKG_NAME:=quectel-lpad
PKG_VERSION:=1.0.7
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/quectel-lpad
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=WWAN
  TITLE:=Quectel LPAD Utility
  DEPENDS:=+libcurl +libopenssl
endef

define Package/quectel-lpad/description
  Quectel LPAD utility for managing eSIMs on Quectel WWAN modules.
endef

# Tell OpenWRT how to compile (your upstream Makefile already handles CROSS_COMPILE)
define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		LD="$(TARGET_LD)" \
		CROSS_COMPILE="$(TARGET_CROSS)"
endef

define Package/quectel-lpad/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/quectel_lpad $(1)/usr/bin/
endef

$(eval $(call BuildPackage,quectel-lpad))
