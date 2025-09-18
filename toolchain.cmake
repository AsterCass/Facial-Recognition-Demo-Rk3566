set(BUILDROOT_PREFIX /home/lckfb/buildroot/buildroot/output/rockchip_rk3566)

# -------------------------
# 系统和交叉编译器
# -------------------------
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER   ${BUILDROOT_PREFIX}/host/bin/aarch64-buildroot-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER ${BUILDROOT_PREFIX}/host/bin/aarch64-buildroot-linux-gnu-g++)

# -------------------------
# Sysroot
# -------------------------
set(CMAKE_SYSROOT ${BUILDROOT_PREFIX}/host/aarch64-buildroot-linux-gnu/sysroot)


# -------------------------
# pkg-config
# -------------------------
set(ENV{PKG_CONFIG} ${BUILDROOT_PREFIX}/host/bin/pkg-config)
set(ENV{PKG_CONFIG_PATH}
        ${CMAKE_SYSROOT}/usr/lib/pkgconfig:
        ${CMAKE_SYSROOT}/usr/share/pkgconfig
)
set(ENV{PKG_CONFIG_LIBDIR} $ENV{PKG_CONFIG_PATH})

