#include "ui/cyu_main.h"
#include <QDebug>
#include <QTimer>
#include <gst/video/videooverlay.h>

VideoWindow::VideoWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("RK3566 Camera Demo");
    resize(800, 600);


    overlayLabel = new QLabel("Home", this);
    overlayLabel->setStyleSheet("QLabel { color: yellow; font-size: 32px; font-weight: bold; }");
    overlayLabel->setAttribute(Qt::WA_TranslucentBackground);
    overlayLabel->move(20, 20);

    // 初始化 GStreamer
    gst_init(nullptr, nullptr);
    initGstPipeline();
}

VideoWindow::~VideoWindow() {
    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
    }
}

void VideoWindow::initGstPipeline() {
    // 构建 pipeline: v4l2src → videoconvert → tee → (qtvideosink, appsink)
    pipeline = gst_parse_launch(
        "v4l2src device=/dev/video0 ! videoconvert ! tee name=t "
        "t. ! queue ! waylandsink name=qtvideosink "
        "t. ! queue ! appsink name=appsink emit-signals=true sync=false",
        nullptr);

    if (!pipeline) {
        qWarning() << "Failed to create GStreamer pipeline";
        return;
    }

    // 获取 appsink
    appsink = gst_bin_get_by_name(GST_BIN(pipeline), "appsink");
    if (appsink) {
        g_signal_connect(appsink, "new-sample", G_CALLBACK(VideoWindow::onNewSample), this);
    }

    // 绑定 qtvideosink 到 Qt 窗口
    GstElement *qtvideosink = gst_bin_get_by_name(GST_BIN(pipeline), "qtvideosink");
    if (qtvideosink) {
        WId winId = this->winId();
        gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(qtvideosink), (guintptr) winId);
        gst_object_unref(qtvideosink);
    }

    // 启动 pipeline
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

GstFlowReturn VideoWindow::onNewSample(GstAppSink *appsink, gpointer user_data) {
    VideoWindow *self = static_cast<VideoWindow *>(user_data);

    GstSample *sample = gst_app_sink_pull_sample(appsink);
    if (sample) {
        GstBuffer *buffer = gst_sample_get_buffer(sample);
        if (buffer) {
            qDebug() << "Got frame with size:" << gst_buffer_get_size(buffer);
        }
        gst_sample_unref(sample);
    }

    return GST_FLOW_OK;
}
