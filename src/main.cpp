#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "rk_mpi_vi.h"
#include "rk_mpi_vpss.h"
#include "rk_mpi_vo.h"
#include "rk_mpi_sys.h"
#include "rk_mpi_mb.h"
#include "rk_comm_video.h"

#define VI_DEV_ID       0
#define VI_CHN_ID       0

int vi_dev_init() {
    printf("%s\n", __func__);
    int ret = 0;
    int devId = VI_DEV_ID;
    int pipeId = devId;

    VI_DEV_ATTR_S stDevAttr;
    VI_DEV_BIND_PIPE_S stBindPipe;
    memset(&stDevAttr, 0, sizeof(stDevAttr));
    memset(&stBindPipe, 0, sizeof(stBindPipe));
    // 0. get dev config status
    ret = RK_MPI_VI_GetDevAttr(devId, &stDevAttr);
    if (ret == RK_ERR_VI_NOT_CONFIG) {
        // 0-1.config dev
        ret = RK_MPI_VI_SetDevAttr(devId, &stDevAttr);
        if (ret != RK_SUCCESS) {
            printf("RK_MPI_VI_SetDevAttr %x\n", ret);
            return -1;
        }
    } else {
        printf("RK_MPI_VI_SetDevAttr already\n");
    }
    // 1.get dev enable status
    ret = RK_MPI_VI_GetDevIsEnable(devId);
    if (ret != RK_SUCCESS) {
        // 1-2.enable dev
        ret = RK_MPI_VI_EnableDev(devId);
        if (ret != RK_SUCCESS) {
            printf("RK_MPI_VI_EnableDev %x\n", ret);
            return -1;
        }
        // 1-3.bind dev/pipe
        stBindPipe.u32Num = 1;
        stBindPipe.PipeId[0] = pipeId;
        ret = RK_MPI_VI_SetDevBindPipe(devId, &stBindPipe);
        if (ret != RK_SUCCESS) {
            printf("RK_MPI_VI_SetDevBindPipe %x\n", ret);
            return -1;
        }
    } else {
        printf("RK_MPI_VI_EnableDev already\n");
    }

    return 0;
}

int vi_chn_init(int channelId, int width, int height) {
    int ret;
    // VI init
    VI_CHN_ATTR_S vi_chn_attr;
    memset(&vi_chn_attr, 0, sizeof(vi_chn_attr));
    vi_chn_attr.stIspOpt.u32BufCount = 2;
    vi_chn_attr.stIspOpt.enMemoryType = VI_V4L2_MEMORY_TYPE_DMABUF;
    vi_chn_attr.stSize.u32Width = width;
    vi_chn_attr.stSize.u32Height = height;
    vi_chn_attr.enPixelFormat = RK_FMT_YUV420SP;
    vi_chn_attr.enCompressMode = COMPRESS_MODE_NONE;
    vi_chn_attr.u32Depth = 2;
    ret = RK_MPI_VI_SetChnAttr(0, channelId, &vi_chn_attr);
    ret |= RK_MPI_VI_EnableChn(0, channelId);
    if (ret) {
        printf("ERROR: create VI error! ret=%d\n", ret);
        return ret;
    }

    return ret;
}


int vo_init(int channelId, int width, int height) {
    int ret;

    VO_PUB_ATTR_S voat = {0};

    ret = RK_MPI_VO_GetPubAttr(0, &voat);

    printf("RK_MPI_VO_GetPubAttr ret =  %d\n", ret);

    voat.enIntfType = VO_INTF_MIPI;
    voat.enIntfSync = VO_OUTPUT_1080P30;
    ret = RK_MPI_VO_SetPubAttr(0, &voat);

    printf("RK_MPI_VO_SetPubAttr ret =  %d\n", ret);

    ret = RK_MPI_VO_Enable(0);

    printf("RK_MPI_VO_Enable ret = %d\n", ret);

    ret = RK_MPI_VO_BindLayer(0, 0, VO_LAYER_MODE_VIDEO);

    printf("RK_MPI_VO_BindLayer ret =  %d\n", ret);

    VO_VIDEO_LAYER_ATTR_S volat = {};
    volat.enCompressMode = COMPRESS_MODE_NONE;
    volat.enPixFormat = RK_FMT_YUV420SP;
    volat.u32DispFrmRt = 30;
    volat.stDispRect.u32Width = width;
    volat.stDispRect.u32Height = height;
    volat.stImageSize.u32Width = width;
    volat.stImageSize.u32Height = height;;
    volat.bBypassFrame = RK_TRUE;
    ret = RK_MPI_VO_SetLayerAttr(0, &volat);

    printf("RK_MPI_VO_SetLayerAttr ret =  %d\n", ret);

    ret = RK_MPI_VO_EnableLayer(0);

    printf("RK_MPI_VO_EnableLayer ret =  %d\n", ret);


    VO_CHN_ATTR_S vocat = {0};
    vocat.stRect.u32Width = width;
    vocat.stRect.u32Height = height;

    ret =RK_MPI_VO_SetChnAttr(0, channelId, &vocat);

    printf("RK_MPI_VO_SetChnAttr ret =  %d\n", ret);

    ret = RK_MPI_VO_EnableChn(0, channelId);

    printf("RK_MPI_VO_EnableChn ret =  %d\n", ret);

    return ret;
}

int main(int argc, char *argv[]) {
    VIDEO_FRAME_INFO_S frame;
    int ret;

    if (RK_MPI_SYS_Init() != RK_SUCCESS) {
        RK_LOGI("rk mpi sys init fail!");
        return -1;
    }

    // vi init
    vi_dev_init();
    RK_LOGI("vi init finish");
    vi_chn_init(VI_CHN_ID, 1920, 1080);

    RK_LOGI("chn init finish");

    vo_init(VI_CHN_ID, 1920, 1080);

    RK_LOGI("vo init finish");

    while (true) {
        RK_LOGI("get frame");
        ret = RK_MPI_VI_GetChnFrame(VI_DEV_ID, VI_CHN_ID, &frame, -1);
        if (ret != RK_SUCCESS) {
            printf("Get frame failed: %d\n", ret);
            usleep(100000);
            continue;
        }

        printf("Get frame success\n");
        printf("%d - %d\n", frame.stVFrame.u32Height, frame.stVFrame.u32Width);


        RK_MPI_VI_ReleaseChnFrame(VI_DEV_ID, VI_CHN_ID, &frame);
        usleep(100000);
    }

    return 0;
}
