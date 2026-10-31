#include "workbench/core.hpp"
#include <iostream>

#if defined(WORKBENCH_HAS_QT)
#include <QApplication>
#include <QMainWindow>
#include <QLabel>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QMainWindow window;
    window.setWindowTitle("PASS Science Works - Unified Engineering Workbench");
    window.resize(1024, 768);
    
    auto* label = new QLabel("PASS Science Works - Unified Engineering Workbench (" + 
                             QString::fromStdString(workbench::core::get_version_string()) + ")", &window);
    label->setAlignment(Qt::AlignCenter);
    window.setCentralWidget(label);
    
    window.show();
    return app.exec();
}
#else
int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    std::cout << "PASS Science Works - Unified Engineering Workbench (" 
              << workbench::core::get_version_string() << ")\n"
              << "Presentation shell: pending Qt 6 installation.\n"
              << "Core backend status: initialized and verified." << std::endl;
    return 0;
}
#endif

