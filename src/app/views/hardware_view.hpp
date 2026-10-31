#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
#include <memory>
#include "workbench/embedded/adapter.hpp"

namespace workbench {
namespace app {

class DeviceCardWidget : public QWidget {
    Q_OBJECT

public:
    explicit DeviceCardWidget(std::unique_ptr<workbench::embedded::IDeviceAdapter> adapter, QWidget* parent = nullptr);

    void refresh();

private slots:
    void onConnectClicked();
    void onDisconnectClicked();
    void onInjectTimeoutToggled(bool checked);
    void onPollTelemetry();

private:
    void setupUi();
    void updateStatusDisplay();

    std::unique_ptr<workbench::embedded::IDeviceAdapter> adapter_;
    QLabel* model_label_{nullptr};
    QLabel* sim_badge_{nullptr};
    QLabel* state_label_{nullptr};
    QLabel* caps_label_{nullptr};
    QLabel* telem_label_{nullptr};
    QPushButton* connect_btn_{nullptr};
    QPushButton* disconnect_btn_{nullptr};
    QPushButton* timeout_toggle_btn_{nullptr};
    QPushButton* poll_btn_{nullptr};
    QTimer* poll_timer_{nullptr};
};

class HardwareView : public QWidget {
    Q_OBJECT

public:
    explicit HardwareView(QWidget* parent = nullptr);

private slots:
    void onDiscoverAllClicked();

private:
    void setupUi();

    DeviceCardWidget* mcu_card_{nullptr};
    DeviceCardWidget* fpga_card_{nullptr};
    DeviceCardWidget* jtag_card_{nullptr};
};

} // namespace app
} // namespace workbench
