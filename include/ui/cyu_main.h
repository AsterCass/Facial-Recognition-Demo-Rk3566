#pragma once
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

class VideoWindow : public QWidget
{
    Q_OBJECT

public:
    VideoWindow(QWidget *parent = nullptr);
    ~VideoWindow();

private:
    void initGstPipeline();
    static GstFlowReturn onNewSample(GstAppSink *appsink, gpointer user_data);

    GstElement *pipeline = nullptr;
    GstElement *appsink = nullptr;

    QLabel *overlayLabel;
};
