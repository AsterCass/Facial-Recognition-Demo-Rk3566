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
#define VI_PIP_ID       0
#define VI_CHN_ID       0
#define VI_H 1080
#define VI_W 1920

#define VO_DEV_ID       0
#define VO_LAYER_ID     0
#define VO_CHN_ID       0


// for 356x vo
#define RK356X_VO_DEV_HD0 0
#define RK356X_VO_DEV_HD1 1
#define RK356X_VOP_LAYER_CLUSTER_0 0
#define RK356X_VOP_LAYER_CLUSTER_1 2
#define RK356X_VOP_LAYER_ESMART_0 4
#define RK356X_VOP_LAYER_ESMART_1 5
#define RK356X_VOP_LAYER_SMART_0 6
#define RK356X_VOP_LAYER_SMART_1 7


int viInit() {
    VI_DEV_ATTR_S stDevAttr;
    VI_DEV_BIND_PIPE_S stBindPipe;
    VI_CHN_ATTR_S stChnAttr;
    RK_S32 s32Ret = RK_FAILURE;

    // 0. get dev config status
    s32Ret = RK_MPI_VI_GetDevAttr(VI_DEV_ID, &stDevAttr);
    if (s32Ret == RK_ERR_VI_NOT_CONFIG) {
        // 0-1.config dev
        s32Ret = RK_MPI_VI_SetDevAttr(VI_DEV_ID, &stDevAttr);
        if (s32Ret != RK_SUCCESS) {
            printf("RK_MPI_VI_SetDevAttr %x\n", s32Ret);
            goto __FAILED;
        }
    } else {
        printf("RK_MPI_VI_SetDevAttr already\n");
    }
    // 1.get  dev enable status
    s32Ret = RK_MPI_VI_GetDevIsEnable(VI_DEV_ID);
    if (s32Ret != RK_SUCCESS) {
        // 1-2.enable dev
        s32Ret = RK_MPI_VI_EnableDev(VI_DEV_ID);
        if (s32Ret != RK_SUCCESS) {
            printf("RK_MPI_VI_EnableDev %x\n", s32Ret);
            goto __FAILED;
        }
        // 1-3.bind dev/pipe
        stBindPipe.u32Num = VI_PIP_ID;
        stBindPipe.PipeId[0] = VI_PIP_ID;
        s32Ret = RK_MPI_VI_SetDevBindPipe(VI_DEV_ID, &stBindPipe);
        if (s32Ret != RK_SUCCESS) {
            printf("RK_MPI_VI_SetDevBindPipe %x", s32Ret);
            goto __FAILED;
        }
    } else {
        printf("RK_MPI_VI_EnableDev already\n");
    }
    // 2.config channel
    stChnAttr.stIspOpt.u32BufCount = 2;
    stChnAttr.stIspOpt.enMemoryType = VI_V4L2_MEMORY_TYPE_DMABUF;
    stChnAttr.stSize.u32Width = VI_W;
    stChnAttr.stSize.u32Height = VI_H;
    stChnAttr.enPixelFormat = RK_FMT_YUV420SP;
    stChnAttr.enCompressMode = COMPRESS_MODE_NONE;
    stChnAttr.u32Depth = 2;
    s32Ret = RK_MPI_VI_SetChnAttr(VI_PIP_ID, VI_CHN_ID, &stChnAttr);
    if (s32Ret != RK_SUCCESS) {
        printf("RK_MPI_VI_SetChnAttr %x\n", s32Ret);
        goto __FAILED;
    }

    // 3.enable channel
    s32Ret = RK_MPI_VI_EnableChn(VI_PIP_ID, VI_CHN_ID);
    if (s32Ret != RK_SUCCESS) {
        printf("RK_MPI_VI_EnableChn %x\n", s32Ret);
        goto __FAILED;
    }


__FAILED:
    return s32Ret;
}

int voInit() {
    /* Enable VO */
    VO_PUB_ATTR_S VoPubAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    RK_S32 s32Ret = RK_SUCCESS;
    VO_CHN_ATTR_S stChnAttr;
    VO_LAYER VoLayer = VO_LAYER_ID;
    VO_DEV VoDev = VO_DEV_ID;

    RK_MPI_VO_DisableLayer(VoLayer);
    RK_MPI_VO_DisableLayer(RK356X_VOP_LAYER_ESMART_0);
    RK_MPI_VO_DisableLayer(RK356X_VOP_LAYER_ESMART_1);
    RK_MPI_VO_DisableLayer(RK356X_VOP_LAYER_SMART_0);
    RK_MPI_VO_DisableLayer(RK356X_VOP_LAYER_SMART_1);
    RK_MPI_VO_Disable(VoDev);

    memset(&VoPubAttr, 0, sizeof(VO_PUB_ATTR_S));
    memset(&stLayerAttr, 0, sizeof(VO_VIDEO_LAYER_ATTR_S));

    stLayerAttr.enPixFormat = RK_FMT_RGB888;
    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;
    stLayerAttr.u32DispFrmRt = 30;
    stLayerAttr.stDispRect.u32Width = 1920;
    stLayerAttr.stDispRect.u32Height = 1080;
    stLayerAttr.stImageSize.u32Width = 1920;
    stLayerAttr.stImageSize.u32Height = 1080;

    s32Ret = RK_MPI_VO_GetPubAttr(VoDev, &VoPubAttr);
    if (s32Ret != RK_SUCCESS) {
        return s32Ret;
    }

    printf("RK_MPI_VO_GetPubAttr finish \n");

    VoPubAttr.enIntfType = VO_INTF_HDMI;
    VoPubAttr.enIntfSync = VO_OUTPUT_DEFAULT;

    s32Ret = RK_MPI_VO_SetPubAttr(VoDev, &VoPubAttr);
    if (s32Ret != RK_SUCCESS) {
        return s32Ret;
    }

    printf("RK_MPI_VO_SetPubAttr finish \n");

    s32Ret = RK_MPI_VO_Enable(VoDev);
    if (s32Ret != RK_SUCCESS) {
        return s32Ret;
    }

    printf("RK_MPI_VO_Enable finish \n");

    s32Ret = RK_MPI_VO_SetLayerAttr(VoLayer, &stLayerAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("RK_MPI_VO_SetLayerAttr failed,s32Ret:%d\n", s32Ret);
        return RK_FAILURE;
    }

    printf("RK_MPI_VO_SetLayerAttr finish \n");

    s32Ret = RK_MPI_VO_BindLayer(VoLayer, VoDev, VO_LAYER_MODE_GRAPHIC);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("RK_MPI_VO_BindLayer failed,s32Ret:%d\n", s32Ret);
        return RK_FAILURE;
    }

    printf("RK_MPI_VO_BindLayer finish \n");


    s32Ret = RK_MPI_VO_EnableLayer(VoLayer);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("RK_MPI_VO_EnableLayer failed,s32Ret:%d\n", s32Ret);
        return RK_FAILURE;
    }

    printf("RK_MPI_VO_EnableLayer finish \n");

    stChnAttr.stRect.s32X = 0;
    stChnAttr.stRect.s32Y = 0;
    stChnAttr.stRect.u32Width = stLayerAttr.stImageSize.u32Width;
    stChnAttr.stRect.u32Height = stLayerAttr.stImageSize.u32Height;
    stChnAttr.u32Priority = 0;
    stChnAttr.u32FgAlpha = 128;
    stChnAttr.u32BgAlpha = 0;

    s32Ret = RK_MPI_VO_SetChnAttr(VoLayer, VO_CHN_ID, &stChnAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("set chn Attr failed,s32Ret:%d\n", s32Ret);
        return RK_FAILURE;
    }

    printf("RK_MPI_VO_SetChnAttr finish \n");

    return s32Ret;
}

int main(int argc, char *argv[]) {

    printf("sys init start\n");
    RK_S32 s32Ret = RK_FAILURE;
    MPP_CHN_S stSrcChn, stDestChn;
    RK_S32 loopCount = 0;

    s32Ret = RK_MPI_SYS_Init();
    if (s32Ret != RK_SUCCESS) {
        printf("rk mpi sys init fail! \n");
        return -1;
    }

    printf("sys init finish\n");

    s32Ret = viInit();
    if (s32Ret != RK_SUCCESS) {
        printf("vi %d:%d init failed:%x \n", VI_DEV_ID, VI_CHN_ID, s32Ret);
        goto __FAILED;
    }

    printf("vi %d:%d success!\n", VI_DEV_ID, VI_CHN_ID);

    s32Ret = voInit();
    if (s32Ret != RK_SUCCESS) {
        printf("create vo failed \n");
        goto __FAILED;
    }

    printf("vo %d:%d success!\n", VO_DEV_ID, VO_CHN_ID);


    // bind vi to vo
    stSrcChn.enModId = RK_ID_VI;
    stSrcChn.s32DevId = VI_DEV_ID;
    stSrcChn.s32ChnId = VI_CHN_ID;

    stDestChn.enModId = RK_ID_VO;
    stDestChn.s32DevId = VO_DEV_ID;
    stDestChn.s32ChnId = VO_CHN_ID;

    s32Ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != RK_SUCCESS) {
        printf("vi band vo fail:%x \n", s32Ret);
        goto __FAILED;
    }

    printf("bind success!\n");

    while (loopCount < 3000) {
        loopCount++;
        printf("loopCount:%d \n", loopCount);
        // can not get the vo frameout count . so here regard as 33ms one frame.
        usleep(33 * 1000);
    }


__FAILED:
    printf("Process finish %x\n", s32Ret);
    s32Ret = RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != RK_SUCCESS) {
        printf("RK_MPI_SYS_UnBind fail %x\n", s32Ret);
    }
    // disable vo
    RK_MPI_VO_DisableLayer(VO_LAYER_ID);
    RK_MPI_VO_DisableLayer(RK356X_VOP_LAYER_ESMART_0);
    RK_MPI_VO_DisableLayer(RK356X_VOP_LAYER_ESMART_1);
    RK_MPI_VO_DisableLayer(RK356X_VOP_LAYER_SMART_0);
    RK_MPI_VO_DisableLayer(RK356X_VOP_LAYER_SMART_1);
    RK_MPI_VO_DisableChn(VO_LAYER_ID, VO_CHN_ID);
    RK_MPI_VO_Disable(VO_DEV_ID);

    // 5. disable one chn
    s32Ret = RK_MPI_VI_DisableChn(VI_PIP_ID, VI_CHN_ID);
    printf("RK_MPI_VI_DisableChn %x\n", s32Ret);

    RK_MPI_VO_DisableChn(VO_LAYER_ID, VO_CHN_ID);

    // 6.disable dev(will diabled all chn)
    s32Ret = RK_MPI_VI_DisableDev(VI_DEV_ID);
    printf("RK_MPI_VI_DisableDev %x\n", s32Ret);

    return 0;
}
