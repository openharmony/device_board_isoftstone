/*
 * Copyright (c) 2021 iSoftStone Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_host.h"
#include "audio_control.h"
#include "audio_dai_if.h"
#include "audio_dai_base.h"
#include "audio_driver_log.h"
#include "osal_io.h"
#include "audio_stream_dispatch.h"

#include "t507_dai_ahub_impl_linux.h"
/* #include "ac107_accessory_impl_linux.h" */

#define HDF_LOG_TAG t507_dai_ahub_ops

int32_t T507AhubDeviceInit(struct AudioCard *audioCard, const struct DaiDevice *dai)
{
    int ret;
    struct DaiData *data;

    AUDIO_DRIVER_LOG_DEBUG("");

    if (dai == NULL || dai->devData == NULL) {
        AUDIO_DRIVER_LOG_ERR("dai is nullptr.");
        return HDF_FAILURE;
    }
    data = dai->devData;

    if (DaiSetConfigInfoOfControls(data) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("set config info fail.");
        return HDF_FAILURE;
    }

    ret = AudioAddControls(audioCard, data->controls, data->numControls);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("add controls failed.");
        return HDF_FAILURE;
    }

    if (data->daiInitFlag == true) {
        AUDIO_DRIVER_LOG_ERR("dai init complete!");
        return HDF_SUCCESS;
    }

    T507AhubImplDeviceInit();

    data->daiInitFlag = true;

    return HDF_SUCCESS;
}

int32_t T507AhubDeviceReadReg(const struct DaiDevice *dai, uint32_t reg, uint32_t *value)
{
    (void)dai;

    if (value == NULL) {
        AUDIO_DRIVER_LOG_ERR("param value is null.");
        return HDF_FAILURE;
    }

    T507AhubImplRegmapRead(reg, value);
    return HDF_SUCCESS;
}

int32_t T507AhubDeviceWriteReg(const struct DaiDevice *dai, uint32_t reg, uint32_t value)
{
    (void)dai;

    T507AhubImplRegmapWrite(reg, value);
    return HDF_SUCCESS;
}

int32_t T507AhubDaiTrigger(const struct AudioCard *card, int cmd, const struct DaiDevice *device)
{
    int ret;

    (void)card;
    (void)device;

    AUDIO_DRIVER_LOG_DEBUG(" cmd -> %d", cmd);

    switch (cmd) {
        case AUDIO_DRV_PCM_IOCTL_RENDER_START:
        case AUDIO_DRV_PCM_IOCTL_RENDER_RESUME:
            ret = T507AhubImplTrigger(AUDIO_RENDER_STREAM, true);
            if (ret != HDF_SUCCESS) {
                AUDIO_DRIVER_LOG_ERR("failed");
                return HDF_FAILURE;
            }
            break;
        case AUDIO_DRV_PCM_IOCTL_RENDER_STOP:
        case AUDIO_DRV_PCM_IOCTL_RENDER_PAUSE:
            ret = T507AhubImplTrigger(AUDIO_RENDER_STREAM, false);
            if (ret != HDF_SUCCESS) {
                AUDIO_DRIVER_LOG_ERR("failed");
                return HDF_FAILURE;
            }
            break;
        case AUDIO_DRV_PCM_IOCTL_CAPTURE_START:
        case AUDIO_DRV_PCM_IOCTL_CAPTURE_RESUME:
            ret = T507AhubImplTrigger(AUDIO_CAPTURE_STREAM, true);
            if (ret != HDF_SUCCESS) {
                AUDIO_DRIVER_LOG_ERR("failed");
                return HDF_FAILURE;
            }
            break;
        case AUDIO_DRV_PCM_IOCTL_CAPTURE_STOP:
        case AUDIO_DRV_PCM_IOCTL_CAPTURE_PAUSE:
            ret = T507AhubImplTrigger(AUDIO_CAPTURE_STREAM, false);
            if (ret != HDF_SUCCESS) {
                AUDIO_DRIVER_LOG_ERR("failed");
                return HDF_FAILURE;
            }
            break;
        default:
            AUDIO_DRIVER_LOG_ERR("failed");
            break;
    }

    /* for capture */
    /* ret = Ac107DaiTrigger(card, cmd, device);
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    } */

    return HDF_SUCCESS;
}

int32_t T507AhubDaiStartup(const struct AudioCard *card, const struct DaiDevice *device)
{
    int32_t ret;

    AUDIO_DRIVER_LOG_DEBUG("");

    (void)card;
    (void)device;

    /* for render */
    ret = T507AhubImplStartup(AUDIO_CAPTURE_STREAM);    /* unuse */
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    /* for capture */
    /* ret = Ac107DaiStartup(card, device);
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    } */

    return HDF_SUCCESS;
}

int32_t T507AhubDaiHwParams(const struct AudioCard *card, const struct AudioPcmHwParams *param)
{
    int ret;

    AUDIO_DRIVER_LOG_DEBUG("");

    if (card == NULL || card->rtd == NULL || card->rtd->cpuDai == NULL ||
        param == NULL || param->cardServiceName == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is nullptr.");
        return HDF_FAILURE;
    }

    /* for render */
    ret = T507AhubImplHwParams(param->streamType, param->format, param->channels, param->rate);
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    /* for capture */
    /* ret = Ac107DaiHwParams(card, param);
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    } */

    return HDF_SUCCESS;
}
