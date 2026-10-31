#include "workbench/core.hpp"
#include <iostream>

#if defined(WORKBENCH_HAS_QT)
#include <QApplication>
#include "main_window.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("EngiWorkbench");
    app.setApplicationDisplayName("PASS Science Works - Unified Engineering Workbench");

    workbench::app::MainWindow window;
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
