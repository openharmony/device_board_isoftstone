#!/bin/bash

# Copyright (c) 2021 iSoftStone Open Source Organization .
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# ${1}  ../../kernel/linux/linux-5.10
# ${2}  /home/jiewangdp/share/uxorious/master/out/yangfan/packages/phone/images
# ${3}  /home/jiewangdp/share/uxorious/master/device/board/isoftstone/yangfan
# ${4}  vendor/isoftstone/yangfan
# ${5}  /home/jiewangdp/share/uxorious/master
# ${6}  rockchip
# ${7}  yangfan
# ${8}  isoftstone
# ${9}  enable_ramdisk

set -e

pushd ${1}
ROOT_DIR=${5}
export PRODUCT_PATH=${4}
export DEVICE_COMPANY=${6}
export DEVICE_NAME=${7}
export PRODUCT_COMPANY=${8}
KERNEL_FORM=${9}
KERNEL_PROD=${10}

KERNEL_SOURCE=${ROOT_DIR}/kernel/linux/linux-5.10
KERNEL_OBJ_TMP_PATH=${ROOT_DIR}/out/kernel/OBJ/linux-5.10
KERNEL_SRC_TMP_PATH=${ROOT_DIR}/out/kernel/src_tmp/linux-5.10
KERNEL_PATCH_PATH=${ROOT_DIR}/kernel/linux/patches/linux-5.10

HARMONY_CONFIG_PATH=${ROOT_DIR}/kernel/linux/config/linux-5.10
DEVICE_CONFIG_PATH=${ROOT_DIR}/kernel/linux/config/linux-5.10/${DEVICE_NAME}
DEFCONFIG_BASE_FILE=${HARMONY_CONFIG_PATH}/base_defconfig
DEFCONFIG_TYPE_FILE=${HARMONY_CONFIG_PATH}/type/standard_defconfig
DEFCONFIG_FORM_FILE=${HARMONY_CONFIG_PATH}/form/${KERNEL_FORM}_defconfig
DEFCONFIG_ARCH_FILE=${DEVICE_CONFIG_PATH}/arch/arm64_defconfig
DEFCONFIG_PROC_FILE=${DEVICE_CONFIG_PATH}/product/${KERNEL_PROD}_defconfig

rm -rf ${KERNEL_SRC_TMP_PATH}
mkdir -p ${KERNEL_SRC_TMP_PATH}

rm -rf ${KERNEL_OBJ_TMP_PATH}
mkdir -p ${KERNEL_OBJ_TMP_PATH}
export KBUILD_OUTPUT=${KERNEL_OBJ_TMP_PATH}

cp -arf ${KERNEL_SOURCE}/* ${KERNEL_SRC_TMP_PATH}/

cd ${KERNEL_SRC_TMP_PATH}

# 合入HDF patch
bash ${ROOT_DIR}/drivers/hdf_core/adapter/khdf/linux/patch_hdf.sh ${ROOT_DIR} ${KERNEL_SRC_TMP_PATH}  ${KERNEL_PATCH_PATH} ${DEVICE_NAME}

# 合入kernel patch
bash ${3}/kernel/src/kernel-patch.sh ${3}/kernel/src ${KERNEL_PATCH_PATH}/${DEVICE_NAME}_patch

cp -rf ${3}/kernel/logo/logo* ${KERNEL_SRC_TMP_PATH}/

#拷贝config
if [ ! -f "$DEFCONFIG_FORM_FILE" ]; then
    DEFCONFIG_FORM_FILE=
    echo "warning no form config file $(DEFCONFIG_FORM_FILE)"
fi
if [ ! -f "$DEFCONFIG_PROC_FILE" ]; then
    DEFCONFIG_PROC_FILE=
    echo "warning no prod config file $(DEFCONFIG_PROC_FILE)"
fi
bash ${ROOT_DIR}/kernel/linux/linux-5.10/scripts/kconfig/merge_config.sh -O ${KERNEL_SRC_TMP_PATH}/arch/arm64/configs/ -m ${DEFCONFIG_TYPE_FILE} ${DEFCONFIG_FORM_FILE} ${DEFCONFIG_ARCH_FILE} ${DEFCONFIG_PROC_FILE} ${DEFCONFIG_BASE_FILE}
mv ${KERNEL_SRC_TMP_PATH}/arch/arm64/configs/.config ${KERNEL_SRC_TMP_PATH}/arch/arm64/configs/rockchip_linux_defconfig

#编译内核
if [ "enable_ramdisk" == "${11}" ]; then
    ./make-ohos.sh sapphire-rk3399 enable_ramdisk
else
    ./make-ohos.sh sapphire-rk3399 disable_ramdisk
fi

mkdir -p ${2}

if [ "enable_ramdisk" != "${11}" ]; then
	cp ${KERNEL_SRC_TMP_PATH}/boot_linux.img ${2}/boot_linux.img
fi

cd ${3}
wget http://www.swanlink.com.cn:82/archive/tools/yangfan_uboot.tar.gz
tar -zxvf yangfan_uboot.tar.gz
cd -

cp ${KERNEL_OBJ_TMP_PATH}/resource.img ${2}/resource.img
cp ${3}/bootloader/parameter.txt ${2}/parameter.txt
cp ${3}/bootloader/MiniLoaderAll.bin ${2}/MiniLoaderAll.bin
cp ${3}/bootloader/uboot.img ${2}/uboot.img
cp ${3}/bootloader/trust.img ${2}/trust.img
cp ${3}/bootloader/config.cfg ${2}/config.cfg
popd
