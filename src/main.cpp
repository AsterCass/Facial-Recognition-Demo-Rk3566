#include <QApplication>
#include "ui/cyu_main.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    CyuMain window;
    window.show();

    return app.exec();
}
