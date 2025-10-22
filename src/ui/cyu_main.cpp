// #include "ui/cyu_main.h"
// #include <QDebug>
// #include <QTimer>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <signal.h>
// #include "rk_mpi_vi.h"
// #include "rk_mpi_vpss.h"
// #include "rk_mpi_vo.h"
// #include "rk_mpi_sys.h"
// #include "rk_mpi_mb.h"
// #include "rk_comm_video.h"
//
// #define VI_DEV_ID       0
// #define VI_CHN_ID       0
// #define VPSS_GRP_ID     0
// #define VPSS_CHN_ID     0
// #define VO_DEV_ID       0
// #define VO_CHN_ID       0
//
// #define CAM_WIDTH       1920
// #define CAM_HEIGHT      1080
// #define DISP_WIDTH      1920
// #define DISP_HEIGHT     1080
// #define FRAME_RATE      30
//
// static int g_exit = 0;
//
// void signal_handler(int sig) {
//     g_exit = 1;
// }
//
// int init_vi() {
//     int ret = 0;
//     int devId = 0;
//     int pipeId = devId;
//
//     VI_DEV_ATTR_S stDevAttr;
//     VI_DEV_BIND_PIPE_S stBindPipe;
//     memset(&stDevAttr, 0, sizeof(stDevAttr));
//     memset(&stBindPipe, 0, sizeof(stBindPipe));
//     // 0. get dev config status
//     ret = RK_MPI_VI_GetDevAttr(devId, &stDevAttr);
//     if (ret == RK_ERR_VI_NOT_CONFIG) {
//         // 0-1.config dev
//         ret = RK_MPI_VI_SetDevAttr(devId, &stDevAttr);
//         if (ret != RK_SUCCESS) {
//             printf("RK_MPI_VI_SetDevAttr %x\n", ret);
//             return -1;
//         }
//     } else {
//         printf("RK_MPI_VI_SetDevAttr already\n");
//     }
//     // 1.get dev enable status
//     ret = RK_MPI_VI_GetDevIsEnable(devId);
//     if (ret != RK_SUCCESS) {
//         // 1-2.enable dev
//         ret = RK_MPI_VI_EnableDev(devId);
//         if (ret != RK_SUCCESS) {
//             printf("RK_MPI_VI_EnableDev %x\n", ret);
//             return -1;
//         }
//         // 1-3.bind dev/pipe
//         stBindPipe.u32Num = 1;
//         stBindPipe.PipeId[0] = pipeId;
//         ret = RK_MPI_VI_SetDevBindPipe(devId, &stBindPipe);
//         if (ret != RK_SUCCESS) {
//             printf("RK_MPI_VI_SetDevBindPipe %x\n", ret);
//             return -1;
//         }
//     } else {
//         printf("RK_MPI_VI_EnableDev already\n");
//     }
//
//     return 0;
// }
//
//
// int vi_chn_init(int channelId, int width, int height) {
//     int ret;
//     int buf_cnt = 2;
//     // VI init
//     VI_CHN_ATTR_S vi_chn_attr;
//     memset(&vi_chn_attr, 0, sizeof(vi_chn_attr));
//     vi_chn_attr.stIspOpt.u32BufCount = buf_cnt;
//     vi_chn_attr.stIspOpt.enMemoryType = VI_V4L2_MEMORY_TYPE_DMABUF; // VI_V4L2_MEMORY_TYPE_MMAP;
//     vi_chn_attr.stSize.u32Width = width;
//     vi_chn_attr.stSize.u32Height = height;
//     vi_chn_attr.enPixelFormat = RK_FMT_YUV420SP;
//     vi_chn_attr.enCompressMode = COMPRESS_MODE_NONE; // COMPRESS_AFBC_16x16;
//     vi_chn_attr.u32Depth = 2; //0, get fail, 1 - u32BufCount, can get, if bind to other device, must be < u32BufCount
//     ret = RK_MPI_VI_SetChnAttr(0, channelId, &vi_chn_attr);
//     ret |= RK_MPI_VI_EnableChn(0, channelId);
//     if (ret) {
//         printf("ERROR: create VI error! ret=%d\n", ret);
//         return ret;
//     }
//
//     return ret;
// }
//
//
//
// CyuMain::CyuMain(QWidget *parent) : QWidget(parent) {
//     setWindowTitle("RK3566 Camera Demo");
//     resize(800, 600);
//
//     overlayLabel = new QLabel("Home", this);
//     overlayLabel->setStyleSheet("QLabel { color: blue; font-size: 32px; font-weight: bold; }");
//     overlayLabel->setAttribute(Qt::WA_TranslucentBackground);
//     overlayLabel->move(20, 20);
//
//
//     printf("Start ... \n");
//
//     VIDEO_FRAME_INFO_S frame;
//     int ret;
//     int frame_count = 0;
//
//     signal(SIGINT, signal_handler);
//     signal(SIGTERM, signal_handler);
//
//     printf("RK3566 Rockit Demo Starting...\n");
//
//     if (RK_MPI_SYS_Init() != RK_SUCCESS) {
//         printf("SYS Init failed\n");
//         return;
//     }
//
//     printf("SYS Init success\n");
//
//     // 初始化VI
//     if (init_vi() != 0) {
//         printf("Init VI failed\n");
//         return;
//     }
//
//     printf("Init VI success\n");
//
//     vi_chn_init(0, CAM_WIDTH, CAM_HEIGHT);
//
//     printf("Init CHN success\n");
//
//     while (!g_exit) {
//         printf("Start get frame\n");
//
//         ret = RK_MPI_VI_GetChnFrame(VI_DEV_ID, VI_CHN_ID, &frame, -1);
//         if (ret != RK_SUCCESS) {
//             printf("Get frame failed: %d\n", ret);
//             usleep(100000);
//             continue;
//         }
//
//         printf("Get frame success\n");
//         printf("%d - %d", frame.stVFrame.u32Height, frame.stVFrame.u32Width);
//
//
//         RK_MPI_VI_ReleaseChnFrame(VI_DEV_ID, VI_CHN_ID, &frame);
//         usleep(100000);
//     }
//
//
//
//
// }
//
// CyuMain::~CyuMain() {
// }
