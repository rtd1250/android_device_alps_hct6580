# Release name
PRODUCT_RELEASE_NAME := hct6580

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_phone.mk)

# Inherit device configuration
$(call inherit-product, device/alps/hct6580/lineage_hct6580.mk)

# Device identifier. This must come after all inclusions
PRODUCT_DEVICE := hct6580
PRODUCT_NAME := lineage_hct6580
PRODUCT_BRAND := alps
PRODUCT_MODEL := hct6580
PRODUCT_MANUFACTURER := alps

PRODUCT_BUILD_PROP_OVERRIDES += \
    BUILD_FINGERPRINT=alps/hct6580/hct6580:6.0/MRA58K/1544859288:user/test-keys \
    PRIVATE_BUILD_DESC="hct6580-user 6.0 MRA58K 1544859288 test-keys"

# SuperUser
WITH_SU := false
WITH_ROOT := false