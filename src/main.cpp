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
#define VI_CHN_ID       1
#define VI_H 1080
#define VI_W 1920

#define VO_DEV_ID       1
#define VO_LAYER_ID     4
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
    printf("%s\n", __func__);
    int ret = 0;
    int devId = VI_DEV_ID;
    int pipeId = VI_PIP_ID;

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

    printf("RK_MPI_VI_SetDevAttr success\n");

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

    printf("RK_MPI_VI_EnableDev success\n");

    // 2.chn
    VI_CHN_ATTR_S vi_chn_attr;
    memset(&vi_chn_attr, 0, sizeof(vi_chn_attr));
    vi_chn_attr.stIspOpt.u32BufCount = 2;
    vi_chn_attr.stIspOpt.enMemoryType = VI_V4L2_MEMORY_TYPE_DMABUF;
    vi_chn_attr.stSize.u32Width = VI_W;
    vi_chn_attr.stSize.u32Height = VI_H;
    vi_chn_attr.stFrameRate.s32DstFrameRate = 30;
    vi_chn_attr.stFrameRate.s32SrcFrameRate = 30;
    vi_chn_attr.enPixelFormat = RK_FMT_YUV420SP;
    vi_chn_attr.enCompressMode = COMPRESS_MODE_NONE; // COMPRESS_AFBC_16x16;
    vi_chn_attr.u32Depth = 2; //0, get fail, 1 - u32BufCount, can get, if bind to other device, must be < u32BufCount
    ret = RK_MPI_VI_SetChnAttr(VI_PIP_ID, VI_CHN_ID, &vi_chn_attr);
    if (ret != RK_SUCCESS) {
        printf("RK_MPI_VI_SetChnAttr error! ret=%d\n", ret);
        return -1;
    }
    ret = RK_MPI_VI_EnableChn(VI_PIP_ID, VI_CHN_ID);
    if (ret != RK_SUCCESS) {
        printf("RK_MPI_VI_EnableChn error! ret=%d\n", ret);
        return -1;
    }

    printf("RK_MPI_VI_EnableChn success\n");

    return 0;

//     VI_DEV_ATTR_S stDevAttr;
//     VI_DEV_BIND_PIPE_S stBindPipe;
//     VI_CHN_ATTR_S stChnAttr;
//     RK_S32 s32Ret = RK_FAILURE;
//
//     memset(&stDevAttr, 0, sizeof(stDevAttr));
//     memset(&stBindPipe, 0, sizeof(stBindPipe));
//     memset(&stChnAttr, 0, sizeof(stChnAttr));
//
//     // 0. get dev config status
//     s32Ret = RK_MPI_VI_GetDevAttr(VI_DEV_ID, &stDevAttr);
//     if (s32Ret == RK_ERR_VI_NOT_CONFIG) {
//         // 0-1.config dev
//         s32Ret = RK_MPI_VI_SetDevAttr(VI_DEV_ID, &stDevAttr);
//         if (s32Ret != RK_SUCCESS) {
//             printf("RK_MPI_VI_SetDevAttr %x\n", s32Ret);
//             goto __FAILED;
//         }
//     } else {
//         printf("RK_MPI_VI_SetDevAttr already\n");
//     }
//     // 1.get  dev enable status
//     s32Ret = RK_MPI_VI_GetDevIsEnable(VI_DEV_ID);
//     if (s32Ret != RK_SUCCESS) {
//         // 1-2.enable dev
//         s32Ret = RK_MPI_VI_EnableDev(VI_DEV_ID);
//         if (s32Ret != RK_SUCCESS) {
//             printf("RK_MPI_VI_EnableDev %x\n", s32Ret);
//             goto __FAILED;
//         }
//         // 1-3.bind dev/pipe
//         stBindPipe.u32Num = 1;
//         stBindPipe.PipeId[0] = VI_PIP_ID;
//         s32Ret = RK_MPI_VI_SetDevBindPipe(VI_DEV_ID, &stBindPipe);
//         if (s32Ret != RK_SUCCESS) {
//             printf("RK_MPI_VI_SetDevBindPipe %x", s32Ret);
//             goto __FAILED;
//         }
//     } else {
//         printf("RK_MPI_VI_EnableDev already\n");
//     }
//     // 2.config channel
//     stChnAttr.stIspOpt.u32BufCount = 2;
//     stChnAttr.stIspOpt.enMemoryType = VI_V4L2_MEMORY_TYPE_DMABUF;
//     stChnAttr.stSize.u32Width = VI_W;
//     stChnAttr.stSize.u32Height = VI_H;
//     stChnAttr.enPixelFormat = RK_FMT_YUV420SP;
//     stChnAttr.enCompressMode = COMPRESS_MODE_NONE;
//     stChnAttr.stFrameRate.s32DstFrameRate = 30;
//     stChnAttr.stFrameRate.s32SrcFrameRate = 30;
//     stChnAttr.u32Depth = 2;
//     s32Ret = RK_MPI_VI_SetChnAttr(VI_PIP_ID, VI_CHN_ID, &stChnAttr);
//     if (s32Ret != RK_SUCCESS) {
//         printf("RK_MPI_VI_SetChnAttr %x\n", s32Ret);
//         goto __FAILED;
//     }
//
//     // 3.enable channel
//     s32Ret = RK_MPI_VI_EnableChn(VI_PIP_ID, VI_CHN_ID);
//     if (s32Ret != RK_SUCCESS) {
//         printf("RK_MPI_VI_EnableChn %x\n", s32Ret);
//         goto __FAILED;
//     }
//
//
// __FAILED:
//     return s32Ret;
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

    stLayerAttr.enPixFormat = RK_FMT_YUV420SP;
    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;
    stLayerAttr.u32DispFrmRt = 30;
    stLayerAttr.stDispRect.u32Width = 720;
    stLayerAttr.stDispRect.u32Height = 1280;
    stLayerAttr.stImageSize.u32Width = 720;
    stLayerAttr.stImageSize.u32Height = 1280;
    stLayerAttr.bBypassFrame = RK_TRUE;

    s32Ret = RK_MPI_VO_GetPubAttr(VoDev, &VoPubAttr);
    if (s32Ret != RK_SUCCESS) {
        return s32Ret;
    }

    printf("RK_MPI_VO_GetPubAttr finish \n");

    VoPubAttr.enIntfType = VO_INTF_MIPI;
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

    RK_MPI_VO_UnBindLayer(0, 0);
    RK_MPI_VO_UnBindLayer(1, 0);
    RK_MPI_VO_UnBindLayer(2, 0);
    RK_MPI_VO_UnBindLayer(3, 0);
    RK_MPI_VO_UnBindLayer(4, 0);
    RK_MPI_VO_UnBindLayer(5, 0);
    RK_MPI_VO_UnBindLayer(6, 0);
    RK_MPI_VO_UnBindLayer(7, 0);
    RK_MPI_VO_UnBindLayer(0, 1);
    RK_MPI_VO_UnBindLayer(1, 1);
    RK_MPI_VO_UnBindLayer(2, 1);
    RK_MPI_VO_UnBindLayer(3, 1);
    RK_MPI_VO_UnBindLayer(4, 1);
    RK_MPI_VO_UnBindLayer(5, 1);
    RK_MPI_VO_UnBindLayer(6, 1);
    RK_MPI_VO_UnBindLayer(7, 1);

    s32Ret = RK_MPI_VO_BindLayer(VoLayer, VoDev, VO_LAYER_MODE_VIDEO);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("RK_MPI_VO_BindLayer failed,s32Ret:%d\n", s32Ret);
        return RK_FAILURE;
    }

    printf("RK_MPI_VO_BindLayer finish \n");

    s32Ret = RK_MPI_VO_SetLayerAttr(VoLayer, &stLayerAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("RK_MPI_VO_SetLayerAttr failed,s32Ret:%d\n", s32Ret);
        return RK_FAILURE;
    }

    printf("RK_MPI_VO_SetLayerAttr finish \n");

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

// int voInitHdmi() {
//     VO_PUB_ATTR_S VoPubAttr;
//     VO_VIDEO_LAYER_ATTR_S stLayerAttr;
//     RK_U32 s32Ret;
//     RK_U32 VoDev, VoLayer;
//
//     memset(&VoPubAttr, 0, sizeof(VO_PUB_ATTR_S));
//     memset(&stLayerAttr, 0, sizeof(VO_VIDEO_LAYER_ATTR_S));
//     VoPubAttr.enIntfType = VO_INTF_HDMI;
//     VoPubAttr.enIntfSync = VO_OUTPUT_1080P50;
//     VoDev = RK356X_VO_DEV_HD0;
//     VoLayer = RK356X_VOP_LAYER_CLUSTER0;
//
//     RK_MPI_VO_BindLayer(VoLayer, VoDev, VO_LAYER_MODE_VIDEO);
//     s32Ret = RK_MPI_VO_SetPubAttr(VoDev, &VoPubAttr);
//     if (s32Ret != RK_SUCCESS)
//     {
//         return -1;
//     }
//     s32Ret = RK_MPI_VO_Enable(VoDev);
//     if (s32Ret != RK_SUCCESS) {
//         return -1;
//     }
//     stLayerAttr.stDispRect.s32X = 0;
//     stLayerAttr.stDispRect.s32Y = 0;
//     stLayerAttr.stDispRect.u32Width = 1920;
//     stLayerAttr.stDispRect.u32Height = 1080;
//     stLayerAttr.stImageSize.u32Width = 1920;
//     stLayerAttr.stImageSize.u32Height = 1080;
//     stLayerAttr.u32DispFrmRt = 25;
//     stLayerAttr.enPixFormat = RK_FMT_YUV420SP;
//     stLayerAttr.enCompressMode = COMPRESS_AFBC_16x16;
//     s32Ret = RK_MPI_VO_SetLayerAttr(VoLayer, &stLayerAttr);
//     if (s32Ret != RK_SUCCESS) {
//         return -1;
//     }
//     s32Ret = RK_MPI_VO_EnableLayer(VoLayer);
//     if (s32Ret != RK_SUCCESS) {
//         return -1;
//     }
//
//
// }

int main(int argc, char *argv[]) {
    RK_S32 s32Ret = RK_FAILURE;

    printf("RK_MPI_SYS_Init start\n");
    s32Ret = RK_MPI_SYS_Init();
    if (s32Ret != RK_SUCCESS) {
        printf("RK_MPI_SYS_Init fail ! ret = %d \n", s32Ret);
        RK_MPI_SYS_Exit();
        return RK_FAILURE;
    }
    printf("RK_MPI_SYS_Init success\n");

    // vi
    {
        // 1.get  dev enable status
        s32Ret = RK_MPI_VI_GetDevIsEnable(VI_DEV_ID);
        if (s32Ret != RK_SUCCESS) {
            // 1-2.enable dev
            s32Ret = RK_MPI_VI_EnableDev(VI_DEV_ID);
            if (s32Ret != RK_SUCCESS) {
                printf("RK_MPI_VI_GetDevIsEnable fail ! ret = %d \n", s32Ret);
                return RK_FAILURE;
            }
            printf("RK_MPI_VI_EnableDev success\n");

            // 1-3.bind dev/pipe
            VI_DEV_BIND_PIPE_S stBindPipe;
            stBindPipe.u32Num = 1;
            // todo stBindPipe.u32Num = 0;
            stBindPipe.PipeId[0] = VI_PIP_ID;
            s32Ret = RK_MPI_VI_SetDevBindPipe(VI_DEV_ID, &stBindPipe);
            if (s32Ret != RK_SUCCESS) {
                printf("RK_MPI_VI_SetDevBindPipe fail ! ret = %d \n", s32Ret);
                return RK_FAILURE;
            }
            printf("RK_MPI_VI_SetDevBindPipe success\n");
        } else {
            printf("RK_MPI_VI_GetDevIsEnable already");
        }
        printf("RK_MPI_VI_GetDevIsEnable success\n");

        // 2.config channel
        VI_CHN_ATTR_S stChnAttr;
        stChnAttr.stSize.u32Width = VI_W;
        stChnAttr.stSize.u32Height = VI_H;
        stChnAttr.enCompressMode = COMPRESS_MODE_NONE;
        s32Ret = RK_MPI_VI_SetChnAttr(VI_PIP_ID, VI_CHN_ID, &stChnAttr);
        if (s32Ret != RK_SUCCESS) {
            printf("RK_MPI_VI_SetChnAttr fail ! ret = %d \n", s32Ret);
            return RK_FAILURE;
        }
        printf("RK_MPI_VI_SetChnAttr success\n");

        // 3.enable channel
        s32Ret = RK_MPI_VI_EnableChn(VI_PIP_ID, VI_CHN_ID);
        if (s32Ret != RK_SUCCESS) {
            printf("RK_MPI_VI_EnableChn fail ! ret = %d \n", s32Ret);
            return RK_FAILURE;
        }
        printf("RK_MPI_VI_EnableChn success\n");
    }

    // vo
    VO_LAYER VoLayer = RK356X_VOP_LAYER_CLUSTER_0;
    VO_DEV VoDev = RK356X_VO_DEV_HD0;
    VO_CHN VoChn = 0;
    {
        VO_PUB_ATTR_S VoPubAttr;
        VO_VIDEO_LAYER_ATTR_S stLayerAttr;
        VO_CHN_ATTR_S stChnAttr;


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
            printf("RK_MPI_VO_GetPubAttr fail ! ret = %d \n", s32Ret);
            return RK_FAILURE;
        }
        printf("RK_MPI_VO_GetPubAttr success\n");

        VoPubAttr.enIntfType = VO_INTF_HDMI;
        VoPubAttr.enIntfSync = VO_OUTPUT_DEFAULT;

        s32Ret = RK_MPI_VO_SetPubAttr(VoDev, &VoPubAttr);
        if (s32Ret != RK_SUCCESS) {
            printf("RK_MPI_VO_SetPubAttr fail ! ret = %d \n", s32Ret);
            return RK_FAILURE;
        }
        printf("RK_MPI_VO_SetPubAttr success\n");

        s32Ret = RK_MPI_VO_Enable(VoDev);
        if (s32Ret != RK_SUCCESS) {
            printf("RK_MPI_VO_Enable fail ! ret = %d \n", s32Ret);
            return RK_FAILURE;
        }
        printf("RK_MPI_VO_Enable success\n");

        s32Ret = RK_MPI_VO_SetLayerAttr(VoLayer, &stLayerAttr);
        if (s32Ret != RK_SUCCESS) {
            printf("RK_MPI_VO_SetLayerAttr fail ! ret = %d \n", s32Ret);
            return RK_FAILURE;
        }
        printf("RK_MPI_VO_SetLayerAttr success\n");

        s32Ret = RK_MPI_VO_BindLayer(VoLayer, VoDev, VO_LAYER_MODE_GRAPHIC);
        if (s32Ret != RK_SUCCESS) {
            printf("RK_MPI_VO_BindLayer fail ! ret = %d \n", s32Ret);
            return RK_FAILURE;
        }
        printf("RK_MPI_VO_BindLayer success\n");

        s32Ret = RK_MPI_VO_EnableLayer(VoLayer);
        if (s32Ret != RK_SUCCESS) {
            printf("RK_MPI_VO_EnableLayer fail ! ret = %d \n", s32Ret);
            return RK_FAILURE;
        }
        printf("RK_MPI_VO_EnableLayer success\n");

        stChnAttr.stRect.s32X = 0;
        stChnAttr.stRect.s32Y = 0;
        stChnAttr.stRect.u32Width = stLayerAttr.stImageSize.u32Width;
        stChnAttr.stRect.u32Height = stLayerAttr.stImageSize.u32Height;
        stChnAttr.u32Priority = 0;
        stChnAttr.u32FgAlpha = 128;
        stChnAttr.u32BgAlpha = 0;

        s32Ret = RK_MPI_VO_SetChnAttr(VoLayer, VoChn, &stChnAttr);
        if (s32Ret != RK_SUCCESS) {
            printf("RK_MPI_VO_SetChnAttr fail ! ret = %d \n", s32Ret);
            return RK_FAILURE;
        }
        printf("RK_MPI_VO_SetChnAttr success\n");

    }

    // bind vi to vo
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId    = RK_ID_VI;
    stSrcChn.s32DevId   = VI_DEV_ID;
    stSrcChn.s32ChnId   = VI_CHN_ID;

    stDestChn.enModId   = RK_ID_VO;
    stDestChn.s32DevId  = VoLayer;
    stDestChn.s32ChnId  = VoChn;


    s32Ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != RK_SUCCESS) {
        printf("RK_MPI_SYS_Bind fail ! ret = %d \n", s32Ret);
        return RK_FAILURE;
    }
    printf("RK_MPI_SYS_Bind success\n");

    s32Ret = RK_MPI_VO_EnableChn(VoLayer, VoChn);
    if (s32Ret != RK_SUCCESS) {
        printf("RK_MPI_VO_EnableChn fail ! ret = %d \n", s32Ret);
        return RK_FAILURE;
    }
    printf("RK_MPI_VO_EnableChn success\n");


    RK_S32 loopCount = 0;
    while (loopCount < 100000) {
        loopCount++;
        //RK_LOGE("loopCount:%d", loopCount);
        // can not get the vo frameout count . so here regard as 33ms one frame.
        usleep(33*1000);
    }

    return 0;
}
