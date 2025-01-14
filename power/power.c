/*
 * Copyright (C) 2016 The LineageOS Project
 * Copyright (C) 2018 SaMad SegMane
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "PowerHAL"

#include <hardware/hardware.h>
#include <hardware/power.h>

#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cutils/properties.h>
#include <utils/Log.h>

#include "power.h"

#define PROFILE_CHANGED "0"
#define PROFILE_CHANGING "1"

#define POWER_PROFILE_PROPERTY "sys.perf.profile.status"
#define CPUFREQ_PATH "/sys/devices/system/cpu/cpu0/cpufreq/"

#define CPU_1_PATH "/sys/devices/system/cpu/cpu0/"
#define CPU_2_PATH "/sys/devices/system/cpu/cpu1/"
#define CPU_3_PATH "/sys/devices/system/cpu/cpu2/"
#define CPU_4_PATH "/sys/devices/system/cpu/cpu3/"

#define INTERACTIVE_PATH "/sys/devices/system/cpu/cpufreq/interactive/"

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static int boostpulse_fd = -1;

static int current_power_profile = -1;
static int requested_power_profile = -1;

static int sysfs_write_str(char *path, char *s)
{
    char buf[80];
    int len;
    int ret = 0;
    int fd;

    fd = open(path, O_WRONLY);
    if (fd < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error opening %s: %s\n", path, buf);
        return -1 ;
    }

    len = write(fd, s, strlen(s));
    if (len < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error writing to %s: %s\n", path, buf);
        ret = -1;
    }

    close(fd);

    return ret;
}

static int sysfs_write_int(char *path, int value)
{
    char buf[80];
    snprintf(buf, 80, "%d", value);
    return sysfs_write_str(path, buf);
}

static bool check_governor(void)
{
    struct stat s;
    int err = stat(INTERACTIVE_PATH, &s);
    if (err != 0) return false;
    if (S_ISDIR(s.st_mode)) return true;
    return false;
}

static int is_profile_valid(int profile)
{
    return profile >= 0 && profile < PROFILE_MAX;
}

static void power_init(__attribute__((unused)) struct power_module *module)
{
    ALOGI("%s", __func__);
}

static int boostpulse_open()
{
    pthread_mutex_lock(&lock);
    if (boostpulse_fd < 0) {
        boostpulse_fd = open(INTERACTIVE_PATH "boostpulse", O_WRONLY);
    }
    pthread_mutex_unlock(&lock);

    return boostpulse_fd;
}

static void power_set_interactive(__attribute__((unused)) struct power_module *module, int on)
{
    if (!is_profile_valid(current_power_profile)) {
        ALOGD("%s: no power profile selected yet", __func__);
        return;
    }

    // break out early if governor is not interactive
    if (!check_governor()) return;

    if (on) {
        sysfs_write_int(INTERACTIVE_PATH "hispeed_freq",
                        profiles[current_power_profile].hispeed_freq);
        sysfs_write_int(INTERACTIVE_PATH "go_hispeed_load",
                        profiles[current_power_profile].go_hispeed_load);
        sysfs_write_str(INTERACTIVE_PATH "target_loads",
                        profiles[current_power_profile].target_loads);
    } else {
        sysfs_write_int(INTERACTIVE_PATH "hispeed_freq",
                        profiles[current_power_profile].hispeed_freq_off);
        sysfs_write_int(INTERACTIVE_PATH "go_hispeed_load",
                        profiles[current_power_profile].go_hispeed_load_off);
        sysfs_write_str(INTERACTIVE_PATH "target_loads",
                        profiles[current_power_profile].target_loads_off);
    }
}

static void set_power_profile(int profile)
{
    if (!is_profile_valid(profile)) {
        ALOGE("%s: unknown profile: %d", __func__, profile);
        return;
    }
    // break out early if governor is not interactive
    if (!check_governor()) return;
 
    if (profile == current_power_profile)
        return;

    // profile changing
    property_set(POWER_PROFILE_PROPERTY, PROFILE_CHANGING);

    ALOGD("%s: setting profile %d", __func__, profile);

    sysfs_write_int(INTERACTIVE_PATH "boost",
                    profiles[profile].boost);
    sysfs_write_int(INTERACTIVE_PATH "boostpulse_duration",
                    profiles[profile].boostpulse_duration);
    sysfs_write_int(INTERACTIVE_PATH "go_hispeed_load",
                    profiles[profile].go_hispeed_load);
    sysfs_write_int(INTERACTIVE_PATH "hispeed_freq",
                    profiles[profile].hispeed_freq);
    sysfs_write_int(INTERACTIVE_PATH "io_is_busy",
                    profiles[profile].io_is_busy);
    sysfs_write_int(INTERACTIVE_PATH "min_sample_time",
                    profiles[profile].min_sample_time);
    sysfs_write_str(INTERACTIVE_PATH "target_loads",
                    profiles[profile].target_loads);    
    sysfs_write_int(CPUFREQ_PATH "scaling_max_freq",
                    profiles[profile].scaling_max_freq);
    sysfs_write_int(CPUFREQ_PATH "scaling_min_freq",
                    profiles[profile].scaling_min_freq);

    sysfs_write_int(CPU_1_PATH "online", profiles[profile].online_1);
    sysfs_write_int(CPU_2_PATH "online", profiles[profile].online_2);
    sysfs_write_int(CPU_3_PATH "online", profiles[profile].online_3);
    sysfs_write_int(CPU_4_PATH "online", profiles[profile].online_4);

    // profile changed
    property_set(POWER_PROFILE_PROPERTY, PROFILE_CHANGED);
    current_power_profile = profile;
}

static void power_hint(__attribute__((unused)) struct power_module *module,
                       power_hint_t hint, void *data)
{
    char buf[80];
    int len;

    switch (hint) {
    case POWER_HINT_INTERACTION:
        if (!is_profile_valid(current_power_profile)) {
            ALOGD("%s: no power profile selected yet", __func__);
            return;
        }

        if (!profiles[current_power_profile].boostpulse_duration)
            return;

        // break out early if governor is not interactive
        if (!check_governor()) return;

        if (boostpulse_open() >= 0) {
            snprintf(buf, sizeof(buf), "%d", 1);
            len = write(boostpulse_fd, &buf, sizeof(buf));
            if (len < 0) {
                strerror_r(errno, buf, sizeof(buf));
                ALOGE("Error writing to boostpulse: %s\n", buf);

                pthread_mutex_lock(&lock);
                close(boostpulse_fd);
                boostpulse_fd = -1;
                pthread_mutex_unlock(&lock);
            }
        }
        break;
    case POWER_HINT_SET_PROFILE:
        pthread_mutex_lock(&lock);

        set_power_profile(*(int32_t *)data);
        pthread_mutex_unlock(&lock);
        break;
    case POWER_HINT_LOW_POWER:
        /* This hint is handled by the framework */
        break;
    default:
        break;
    }
};
static struct hw_module_methods_t power_module_methods = {
    .open = NULL,
};

static int get_feature(__attribute__((unused)) struct power_module *module,
                       feature_t feature)
{
    if (feature == POWER_FEATURE_SUPPORTED_PROFILES) {
        return PROFILE_MAX;
    }
    return -1;
}

struct power_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = POWER_MODULE_API_VERSION_0_2,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = POWER_HARDWARE_MODULE_ID,
        .name = "MT6580 Power Hal",
        .author = "Android Open Source Project",
        .methods = &power_module_methods,
    },

    .init = power_init,
    .setInteractive = power_set_interactive,
    .powerHint = power_hint,
    .getFeature = get_feature
};
