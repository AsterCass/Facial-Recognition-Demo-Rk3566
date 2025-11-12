#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "rk_mpi_vi.h"
#include "rk_mpi_vpss.h"
#include "rk_mpi_vo.h"
#include "rk_mpi_sys.h"
#include "rk_comm_video.h"

#define VI_DEV_ID       0
#define VI_PIP_ID       0
#define VI_CHN_ID       1
#define VI_H 1280
#define VI_W 720

// for 356x vo
#define RK356X_VO_DEV_HD0 0
#define RK356X_VO_DEV_HD1 1
#define RK356X_VOP_LAYER_CLUSTER_0 0
#define RK356X_VOP_LAYER_CLUSTER_1 2
#define RK356X_VOP_LAYER_ESMART_0 4
#define RK356X_VOP_LAYER_ESMART_1 5
#define RK356X_VOP_LAYER_SMART_0 6
#define RK356X_VOP_LAYER_SMART_1 7

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
        memset(&stChnAttr, 0, sizeof(stChnAttr));
        stChnAttr.stIspOpt.u32BufCount = 3;
        stChnAttr.stIspOpt.enMemoryType = VI_V4L2_MEMORY_TYPE_DMABUF;
        stChnAttr.stIspOpt.enCaptureType = VI_V4L2_CAPTURE_TYPE_VIDEO_CAPTURE;
        stChnAttr.enPixelFormat = RK_FMT_YUV420SP;
        stChnAttr.stFrameRate.s32SrcFrameRate = -1;
        stChnAttr.stFrameRate.s32DstFrameRate = -1;
        stChnAttr.u32Depth = 0;
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

    // vpss
    VPSS_GRP VpssGrp = 0;
    VPSS_CHN VpssChn = 0;
    VPSS_CHN_ATTR_S stVpssChnAttr;
    VPSS_GRP_ATTR_S stGrpVpssAttr;
    memset(&stVpssChnAttr, 0, sizeof(VPSS_CHN_ATTR_S));
    memset(&stGrpVpssAttr, 0, sizeof(VPSS_GRP_ATTR_S));

    stGrpVpssAttr.u32MaxW = 4096;
    stGrpVpssAttr.u32MaxH = 4096;
    stGrpVpssAttr.enPixelFormat = RK_FMT_YUV420SP;
    stGrpVpssAttr.stFrameRate.s32SrcFrameRate = -1;
    stGrpVpssAttr.stFrameRate.s32DstFrameRate = -1;
    stGrpVpssAttr.enCompressMode = COMPRESS_MODE_NONE;

    stVpssChnAttr.enChnMode = VPSS_CHN_MODE_USER;
    stVpssChnAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stVpssChnAttr.enPixelFormat = RK_FMT_YUV420SP;
    stVpssChnAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssChnAttr.stFrameRate.s32DstFrameRate = -1;
    stVpssChnAttr.u32Width = 720;
    stVpssChnAttr.u32Height = 1280;
    stVpssChnAttr.u32Depth = 2;
    stVpssChnAttr.enCompressMode = COMPRESS_MODE_NONE;

    s32Ret = RK_MPI_VPSS_CreateGrp(VpssGrp, &stGrpVpssAttr);
    if (s32Ret != RK_SUCCESS) {
        printf("RK_MPI_VPSS_CreateGrp fail ! ret = %d \n", s32Ret);
        return RK_FAILURE;
    }
    printf("RK_MPI_VPSS_CreateGrp success\n");
    s32Ret = RK_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stVpssChnAttr);
    if (s32Ret != RK_SUCCESS) {
        printf("RK_MPI_VPSS_SetChnAttr fail ! ret = %d \n", s32Ret);
        return RK_FAILURE;
    }
    printf("RK_MPI_VPSS_SetChnAttr success\n");
    s32Ret =  RK_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
    if (s32Ret != RK_SUCCESS) {
        printf("RK_MPI_VPSS_EnableChn fail ! ret = %d \n", s32Ret);
        return RK_FAILURE;
    }
    printf("RK_MPI_VPSS_EnableChn success\n");
    s32Ret =  RK_MPI_VPSS_StartGrp(VpssGrp);
    if (s32Ret != RK_SUCCESS) {
        printf("RK_MPI_VPSS_StartGrp fail ! ret = %d \n", s32Ret);
        return RK_FAILURE;
    }
    printf("RK_MPI_VPSS_StartGrp success\n");


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

        stLayerAttr.enPixFormat = RK_FMT_YUV420SP;
        stLayerAttr.stDispRect.s32X = 0;
        stLayerAttr.stDispRect.s32Y = 0;
        stLayerAttr.u32DispFrmRt = 30;
        stLayerAttr.stDispRect.u32Width = 720;
        stLayerAttr.stDispRect.u32Height = 1280;
        stLayerAttr.stImageSize.u32Width = 720;
        stLayerAttr.stImageSize.u32Height = 1280;

        s32Ret = RK_MPI_VO_GetPubAttr(VoDev, &VoPubAttr);
        if (s32Ret != RK_SUCCESS) {
            printf("RK_MPI_VO_GetPubAttr fail ! ret = %d \n", s32Ret);
            return RK_FAILURE;
        }
        printf("RK_MPI_VO_GetPubAttr success\n");

        VoPubAttr.enIntfType = VO_INTF_MIPI;
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
    MPP_CHN_S stSrcChn, stMidChn, stDestChn;

    stSrcChn.enModId    = RK_ID_VI;
    stSrcChn.s32DevId   = VI_DEV_ID;
    stSrcChn.s32ChnId   = VI_CHN_ID;

    stMidChn.enModId    = RK_ID_VPSS;
    stMidChn.s32DevId   = 0;
    stMidChn.s32ChnId   = 0;

    stDestChn.enModId   = RK_ID_VO;
    stDestChn.s32DevId  = VoLayer;
    stDestChn.s32ChnId  = VoChn;


    s32Ret = RK_MPI_SYS_Bind(&stSrcChn, &stMidChn);
    if (s32Ret != RK_SUCCESS) {
        printf("RK_MPI_SYS_Bind fail ! ret = %d \n", s32Ret);
        return RK_FAILURE;
    }
    printf("RK_MPI_SYS_Bind success\n");

    s32Ret = RK_MPI_SYS_Bind(&stMidChn, &stDestChn);
    if (s32Ret != RK_SUCCESS) {
        printf("RK_MPI_SYS_Bind2 fail ! ret = %d \n", s32Ret);
        return RK_FAILURE;
    }
    printf("RK_MPI_SYS_Bind2 success\n");

    s32Ret = RK_MPI_VO_EnableChn(VoLayer, VoChn);
    if (s32Ret != RK_SUCCESS) {
        printf("RK_MPI_VO_EnableChn fail ! ret = %d \n", s32Ret);
        return RK_FAILURE;
    }
    printf("RK_MPI_VO_EnableChn success\n");


    RK_S32 loopCount = 0;
    while (loopCount < 100000) {
        loopCount++;

        VIDEO_FRAME_INFO_S stViFrame;
        memset(&stViFrame, 0, sizeof(VIDEO_FRAME_INFO_S));
        // get vi frame
         s32Ret = RK_MPI_VPSS_GetChnFrame(0, 0, &stViFrame, 0);
        if(s32Ret == RK_SUCCESS) {
            printf("RK_MPI_VO_EnableChn success %d %d\n",
                stViFrame.stVFrame.u32Width, stViFrame.stVFrame.u32Height);
        } else
        {
            printf("RK_MPI_VPSS_GetChnFrame fail ! ret = %d \n", s32Ret);
        }

        s32Ret = RK_MPI_VPSS_ReleaseChnFrame(0, 0, &stViFrame);
        if (s32Ret != RK_SUCCESS) {
            printf("RK_MPI_VPSS_ReleaseChnFrame fail ! ret = %d \n", s32Ret);
        }

        usleep(33*1000);
    }

    return 0;
}
