#include "ui/cyu_main.h"
#include <QDebug>
#include <QTimer>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "rk_mpi_vi.h"
#include "rk_mpi_vpss.h"
#include "rk_mpi_vo.h"
#include "rk_mpi_sys.h"
#include "rk_mpi_mb.h"
#include "rk_comm_video.h"

#define VI_DEV_ID       0
#define VI_CHN_ID       0
#define VPSS_GRP_ID     0
#define VPSS_CHN_ID     0
#define VO_DEV_ID       0
#define VO_CHN_ID       0

#define CAM_WIDTH       1920
#define CAM_HEIGHT      1080
#define DISP_WIDTH      1920
#define DISP_HEIGHT     1080
#define FRAME_RATE      30

static int g_exit = 0;

void signal_handler(int sig) {
    g_exit = 1;
}

int init_vi() {
    VI_DEV_ATTR_S dev_attr;
    VI_CHN_ATTR_S chn_attr;

    memset(&dev_attr, 0, sizeof(VI_DEV_ATTR_S));

    if (RK_MPI_VI_SetDevAttr(VI_DEV_ID, &dev_attr) != RK_SUCCESS) {
        printf("VI SetDevAttr failed\n");
        return -1;
    }

    printf("VI SetDevAttr success\n");

    if (RK_MPI_VI_EnableDev(VI_DEV_ID) != RK_SUCCESS) {
        printf("VI EnableDev failed\n");
        return -1;
    }

    printf("VI EnableDev success\n");

    memset(&chn_attr, 0, sizeof(VI_CHN_ATTR_S));
    chn_attr.stSize.u32Width = CAM_WIDTH;
    chn_attr.stSize.u32Height = CAM_HEIGHT;
    chn_attr.enPixelFormat = RK_FMT_YUV420SP;
    chn_attr.u32Depth = 1;
    chn_attr.enCompressMode = COMPRESS_MODE_NONE;

    if (RK_MPI_VI_SetChnAttr(VI_DEV_ID, VI_CHN_ID, &chn_attr) != RK_SUCCESS) {
        printf("VI SetChnAttr failed\n");
        return -1;
    }

    printf("VI SetChnAttr success\n");

    if (RK_MPI_VI_EnableChn(VI_DEV_ID, VI_CHN_ID) != RK_SUCCESS) {
        printf("VI EnableChn failed\n");
        return -1;
    }

    printf("VI EnableChn success\n");

    return 0;
}

CyuMain::CyuMain(QWidget *parent) : QWidget(parent) {
    setWindowTitle("RK3566 Camera Demo");
    resize(800, 600);

    overlayLabel = new QLabel("Home", this);
    overlayLabel->setStyleSheet("QLabel { color: blue; font-size: 32px; font-weight: bold; }");
    overlayLabel->setAttribute(Qt::WA_TranslucentBackground);
    overlayLabel->move(20, 20);


    printf("Start ... \n");

    VIDEO_FRAME_INFO_S frame;
    int ret;
    int frame_count = 0;

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    printf("RK3566 Rockit Demo Starting...\n");

    if (RK_MPI_SYS_Init() != RK_SUCCESS) {
        printf("SYS Init failed\n");
        return;
    }

    printf("SYS Init success\n");

    // 初始化VI
    if (init_vi() != 0) {
        printf("Init VI failed\n");
        return;
    }

    printf("Init VI success\n");

    while (!g_exit) {
        printf("Start get frame\n");

        ret = RK_MPI_VI_GetChnFrame(VI_DEV_ID, VI_CHN_ID, &frame, -1);
        if (ret != RK_SUCCESS) {
            printf("Get frame failed: %d\n", ret);
            usleep(100000);
            continue;
        }

        printf("Get frame success\n");
        printf("%d - %d", frame.stVFrame.u32Height, frame.stVFrame.u32Width);


        RK_MPI_VI_ReleaseChnFrame(VI_DEV_ID, VI_CHN_ID, &frame);
        usleep(100000);
    }




}

CyuMain::~CyuMain() {
}
