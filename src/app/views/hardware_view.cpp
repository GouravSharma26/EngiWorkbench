#include "workbench/views/hardware_view.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>

namespace workbench {
namespace app {

DeviceCardWidget::DeviceCardWidget(std::unique_ptr<workbench::embedded::IDeviceAdapter> adapter, QWidget* parent)
    : QWidget(parent), adapter_(std::move(adapter)) {
    setupUi();
    refresh();

    poll_timer_ = new QTimer(this);
    connect(poll_timer_, &QTimer::timeout, this, &DeviceCardWidget::onPollTelemetry);
}

void DeviceCardWidget::setupUi() {
    auto* main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(10, 10, 10, 10);
    main_layout->setSpacing(8);

    auto* box = new QGroupBox(this);
    auto* box_layout = new QVBoxLayout(box);

    // Title row with model and SIMULATED badge
    auto* title_row = new QHBoxLayout();
    model_label_ = new QLabel("Device", box);
    model_label_->setStyleSheet("font-size: 13px; font-weight: bold; color: #cdd6f4;");

    sim_badge_ = new QLabel(" SIMULATED ", box);
    sim_badge_->setStyleSheet("background-color: #fab387; color: #11111b; font-weight: bold; border-radius: 4px; padding: 2px 6px;");

    title_row->addWidget(model_label_);
    title_row->addStretch();
    title_row->addWidget(sim_badge_);
    box_layout->addLayout(title_row);

    // State & Capabilities Labels
    state_label_ = new QLabel("Connection: Disconnected", box);
    state_label_->setStyleSheet("color: #bac2de; font-size: 11px;");
    box_layout->addWidget(state_label_);

    caps_label_ = new QLabel("Capabilities: -", box);
    caps_label_->setWordWrap(true);
    caps_label_->setStyleSheet("color: #a6adc8; font-size: 11px;");
    box_layout->addWidget(caps_label_);

    // Telemetry display
    telem_label_ = new QLabel("Telemetry: None (Connect device to poll)", box);
    telem_label_->setWordWrap(true);
    telem_label_->setStyleSheet("background-color: #181825; color: #89b4fa; padding: 8px; border-radius: 4px; font-family: monospace; font-size: 11px;");
    box_layout->addWidget(telem_label_);

    // Controls Row
    auto* btn_row = new QHBoxLayout();
    connect_btn_ = new QPushButton("Connect", box);
    connect_btn_->setStyleSheet("background-color: #2b3145; color: #a6e3a1; font-weight: bold;");

    disconnect_btn_ = new QPushButton("Disconnect", box);
    disconnect_btn_->setEnabled(false);
    disconnect_btn_->setStyleSheet("background-color: #2b3145; color: #f38ba8;");

    poll_btn_ = new QPushButton("Poll Telemetry", box);
    poll_btn_->setEnabled(false);

    timeout_toggle_btn_ = new QPushButton("Inject Timeout Fault", box);
    timeout_toggle_btn_->setCheckable(true);
    timeout_toggle_btn_->setStyleSheet("background-color: #2b3145; color: #f9e2af;");

    btn_row->addWidget(connect_btn_);
    btn_row->addWidget(disconnect_btn_);
    btn_row->addWidget(poll_btn_);
    btn_row->addWidget(timeout_toggle_btn_);
    box_layout->addLayout(btn_row);

    main_layout->addWidget(box);

    connect(connect_btn_, &QPushButton::clicked, this, &DeviceCardWidget::onConnectClicked);
    connect(disconnect_btn_, &QPushButton::clicked, this, &DeviceCardWidget::onDisconnectClicked);
    connect(poll_btn_, &QPushButton::clicked, this, &DeviceCardWidget::onPollTelemetry);
    connect(timeout_toggle_btn_, &QPushButton::toggled, this, &DeviceCardWidget::onInjectTimeoutToggled);
}

void DeviceCardWidget::refresh() {
    if (!adapter_) return;
    auto desc = adapter_->discover();
    auto caps = adapter_->capabilities();

    model_label_->setText(QString::fromStdString(desc.model_name));
    
    QString ifaces;
    for (const auto& iface : caps.supported_interfaces) {
        if (!ifaces.empty()) ifaces += ", ";
        ifaces += QString::fromStdString(iface);
    }
    caps_label_->setText(QString("Vendor: %1 | Logic: %2 (%3V Reference)\nInterfaces: %4")
        .arg(QString::fromStdString(desc.vendor))
        .arg(QString::fromStdString(caps.logic_standard))
        .arg(caps.voltage_reference_V, 0, 'f', 1)
        .arg(ifaces));

    updateStatusDisplay();
}

void DeviceCardWidget::updateStatusDisplay() {
    auto s = adapter_->state();
    switch (s) {
    case embedded::ConnectionState::Connected:
        state_label_->setText("Connection: <font color='#a6e3a1'>Connected (Simulated Host Transport)</font>");
        connect_btn_->setEnabled(false);
        disconnect_btn_->setEnabled(true);
        poll_btn_->setEnabled(true);
        break;
    case embedded::ConnectionState::Disconnected:
        state_label_->setText("Connection: <font color='#a6adc8'>Disconnected</font>");
        connect_btn_->setEnabled(true);
        disconnect_btn_->setEnabled(false);
        poll_btn_->setEnabled(false);
        telem_label_->setText("Telemetry: Disconnected.");
        break;
    case embedded::ConnectionState::Error:
        state_label_->setText("Connection: <font color='#f38ba8'>ERROR / TIMEOUT (Recoverable)</font>");
        connect_btn_->setEnabled(true); // Allow reconnect
        connect_btn_->setText("Reconnect & Recover");
        disconnect_btn_->setEnabled(true);
        poll_btn_->setEnabled(false);
        break;
    default:
        break;
    }
}

void DeviceCardWidget::onConnectClicked() {
    connect_btn_->setText("Connect");
    auto res = adapter_->connect();
    if (res.success) {
        poll_timer_->start(1000); // 1Hz live telemetry polling
        onPollTelemetry();
    } else {
        poll_timer_->stop();
        telem_label_->setText(QString("<font color='#f38ba8'>%1</font>").arg(QString::fromStdString(res.message)));
    }
    updateStatusDisplay();
}

void DeviceCardWidget::onDisconnectClicked() {
    poll_timer_->stop();
    adapter_->disconnect();
    updateStatusDisplay();
}

void DeviceCardWidget::onInjectTimeoutToggled(bool checked) {
    adapter_->inject_timeout(checked);
    if (checked) {
        timeout_toggle_btn_->setText("Fault Injected (Active)");
        timeout_toggle_btn_->setStyleSheet("background-color: #f38ba8; color: #11111b; font-weight: bold;");
    } else {
        timeout_toggle_btn_->setText("Inject Timeout Fault");
        timeout_toggle_btn_->setStyleSheet("background-color: #2b3145; color: #f9e2af;");
    }
}

void DeviceCardWidget::onPollTelemetry() {
    auto res = adapter_->read_telemetry();
    if (res.success && res.telemetry.has_value()) {
        const auto& t = *res.telemetry;
        telem_label_->setText(
            QString("Packet #%1 | Time: %2 ms\n"
                    "Temp: %3 °C | Voltage: %4 V | Current: %5 mA\n"
                    "Status: %6")
                .arg(t.packet_counter)
                .arg(t.timestamp_ms)
                .arg(t.core_temperature_C, 0, 'f', 2)
                .arg(t.supply_voltage_V, 0, 'f', 3)
                .arg(t.current_mA, 0, 'f', 1)
                .arg(QString::fromStdString(t.status_code))
        );
    } else {
        poll_timer_->stop();
        telem_label_->setText(
            QString("<font color='#f38ba8'>FAULT DETECTED:\n%1\nClick 'Reconnect & Recover' to resume normal operation.</font>")
                .arg(QString::fromStdString(res.message))
        );
        updateStatusDisplay();
    }
}

HardwareView::HardwareView(QWidget* parent)
    : QWidget(parent) {
    setupUi();
}

void HardwareView::setupUi() {
    auto* main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(16, 16, 16, 16);
    main_layout->setSpacing(12);

    // Header banner with safety notice
    auto* header_group = new QGroupBox("Embedded System Integration Design (Host-Connected Mocks)", this);
    auto* header_layout = new QVBoxLayout(header_group);

    auto* notice_label = new QLabel(
        "<b>Important Notice:</b> All device interactions in this prototype are strictly <b>SIMULATED</b>. "
        "No physical microcontrollers, FPGAs, or JTAG adapters are powered or detected.<br>"
        "<b>JTAG Adapter Designation:</b> The JTAG-HS2 is an external debug/programming interface (IEEE 1149.1 TAP), "
        "not a generic compute board. All computational workloads execute on the host PC.",
        header_group
    );
    notice_label->setWordWrap(true);
    notice_label->setStyleSheet("color: #fab387; font-size: 12px;");

    auto* walkthrough_label = new QLabel(
        "<b>Fault Injection Demonstration Walkthrough:</b> "
        "1. Click 'Connect' on any device -> 2. Click 'Inject Timeout Fault' -> "
        "3. Observe simulated timeout error transition -> 4. Untoggle fault and click 'Reconnect & Recover' to restore live telemetry.",
        header_group
    );
    walkthrough_label->setWordWrap(true);
    walkthrough_label->setStyleSheet("color: #a6adc8; font-size: 11px;");

    header_layout->addWidget(notice_label);
    header_layout->addWidget(walkthrough_label);
    main_layout->addWidget(header_group);

    // Top action bar
    auto* top_bar = new QHBoxLayout();
    auto* refresh_all_btn = new QPushButton("Rediscover All Device Mocks", this);
    refresh_all_btn->setStyleSheet("background-color: #2b3145; color: #89b4fa;");
    top_bar->addWidget(refresh_all_btn);
    top_bar->addStretch();
    main_layout->addLayout(top_bar);

    // Three Device Cards
    mcu_card_ = new DeviceCardWidget(embedded::create_microcontroller_mock(), this);
    fpga_card_ = new DeviceCardWidget(embedded::create_fpga_mock(), this);
    jtag_card_ = new DeviceCardWidget(embedded::create_jtag_adapter_mock(), this);

    main_layout->addWidget(mcu_card_);
    main_layout->addWidget(fpga_card_);
    main_layout->addWidget(jtag_card_);
    main_layout->addStretch();

    connect(refresh_all_btn, &QPushButton::clicked, this, &HardwareView::onDiscoverAllClicked);
}

void HardwareView::onDiscoverAllClicked() {
    mcu_card_->refresh();
    fpga_card_->refresh();
    jtag_card_->refresh();
}

} // namespace app
} // namespace workbench

