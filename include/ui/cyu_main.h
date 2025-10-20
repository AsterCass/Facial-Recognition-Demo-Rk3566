#pragma once
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class CyuMain : public QWidget {
    Q_OBJECT

public:
    explicit CyuMain(QWidget *parent = nullptr);

    ~CyuMain() override;

private:
    QLabel *overlayLabel;
};
