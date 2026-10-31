#include "workbench/embedded/adapter.hpp"
#include <chrono>

namespace workbench {
namespace embedded {

namespace {

uint64_t current_time_millis() {
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now).count());
}

class BaseMockAdapter : public IDeviceAdapter {
public:
    explicit BaseMockAdapter(DeviceDescriptor desc, DeviceCapabilities caps)
        : desc_(std::move(desc)), caps_(std::move(caps)) {}

    DeviceDescriptor discover() override {
        return desc_;
    }

    DeviceCapabilities capabilities() const override {
        return caps_;
    }

    AdapterResult connect() override {
        if (inject_timeout_) {
            state_ = ConnectionState::Error;
            return {false, "Connection timed out: Simulated device unresponsive during handshake."};
        }
        state_ = ConnectionState::Connected;
        return {true, "Connected to " + desc_.model_name + " (SIMULATED)."};
    }

    AdapterResult disconnect() override {
        state_ = ConnectionState::Disconnected;
        return {true, "Disconnected from " + desc_.model_name + " (SIMULATED)."};
    }

    ConnectionState state() const override {
        return state_;
    }

    void inject_timeout(bool enable) override {
        inject_timeout_ = enable;
    }

protected:
    DeviceDescriptor desc_;
    DeviceCapabilities caps_;
    ConnectionState state_{ConnectionState::Disconnected};
    bool inject_timeout_{false};
    uint32_t packet_counter_{0};
};

class MicrocontrollerMock : public BaseMockAdapter {
public:
    MicrocontrollerMock()
        : BaseMockAdapter(
            {"mcu-esp32-01", DeviceType::Microcontroller, 
             "ESP32-DevKitC V4 (ESP32-WROOM-32E)", "Espressif Systems", 
             "SIM-ESP32-94821", "USB-to-UART Serial Bridge", true},
            {true, true, true, 3.3, "3.3V GPIO (Not 5V Tolerant)", {"UART", "SPI", "I2C", "GPIO"}}
        ) {}

    AdapterResult read_telemetry() override {
        if (state_ != ConnectionState::Connected) {
            return {false, "Cannot read telemetry: Device is not connected."};
        }
        if (inject_timeout_) {
            state_ = ConnectionState::Error;
            return {false, "Timeout fault: ESP32 UART bridge did not receive expected ACK within 3000ms (SIMULATED)."};
        }

        ++packet_counter_;
        TelemetryRecord rec;
        rec.timestamp_ms = current_time_millis();
        rec.core_temperature_C = 41.5 + 0.1 * (packet_counter_ % 20);
        rec.supply_voltage_V = 3.295;
        rec.current_mA = 85.0 + 2.0 * (packet_counter_ % 5);
        rec.packet_counter = packet_counter_;
        rec.status_code = "OPERATIONAL_SIMULATED";

        return {true, "Telemetry read OK", rec};
    }
};

class FpgaMock : public BaseMockAdapter {
public:
    FpgaMock()
        : BaseMockAdapter(
            {"fpga-arty-01", DeviceType::Fpga, 
             "Digilent Arty A7-35T Development Board", "Digilent", 
             "SIM-ARTY-77104", "On-board USB-JTAG/UART Port", true},
            {true, true, true, 3.3, "LVCMOS 3.3V (Pmod Headers)", {"JTAG Configuration", "Pmod LVCMOS", "UART"}}
        ) {}

    AdapterResult read_telemetry() override {
        if (state_ != ConnectionState::Connected) {
            return {false, "Cannot read telemetry: Device is not connected."};
        }
        if (inject_timeout_) {
            state_ = ConnectionState::Error;
            return {false, "Timeout fault: Arty A7 FPGA internal monitoring bus timed out (SIMULATED)."};
        }

        ++packet_counter_;
        TelemetryRecord rec;
        rec.timestamp_ms = current_time_millis();
        rec.core_temperature_C = 48.2 + 0.15 * (packet_counter_ % 15);
        rec.supply_voltage_V = 0.998; // VCCINT ~ 1.0V internal
        rec.current_mA = 320.0 + 5.0 * (packet_counter_ % 10);
        rec.packet_counter = packet_counter_;
        rec.status_code = "FABRIC_READY_SIMULATED";

        return {true, "Telemetry read OK", rec};
    }
};

class JtagAdapterMock : public BaseMockAdapter {
public:
    JtagAdapterMock()
        : BaseMockAdapter(
            {"jtag-hs2-01", DeviceType::JtagAdapter, 
             "Digilent JTAG-HS2 Programming Cable", "Digilent", 
             "SIM-HS2-30219", "USB 2.0 High Speed Interface", true},
            {true, true, true, 3.3, "1.8V - 5.0V Target VREF Referenced", {"IEEE 1149.1 JTAG TAP", "Boundary Scan"}}
        ) {}

    AdapterResult read_telemetry() override {
        if (state_ != ConnectionState::Connected) {
            return {false, "Cannot read telemetry: Device is not connected."};
        }
        if (inject_timeout_) {
            state_ = ConnectionState::Error;
            return {false, "Timeout fault: JTAG TDO line unresponsive during TAP query (SIMULATED)."};
        }

        ++packet_counter_;
        TelemetryRecord rec;
        rec.timestamp_ms = current_time_millis();
        rec.core_temperature_C = 34.0 + 0.05 * (packet_counter_ % 10);
        rec.supply_voltage_V = 3.301; // Target VREF sense
        rec.current_mA = 42.0;
        rec.packet_counter = packet_counter_;
        rec.status_code = "TAP_IDLE_SIMULATED (DEBUG_BRIDGE_ONLY)";

        return {true, "Telemetry read OK", rec};
    }
};

} // namespace

std::unique_ptr<IDeviceAdapter> create_microcontroller_mock() {
    return std::make_unique<MicrocontrollerMock>();
}

std::unique_ptr<IDeviceAdapter> create_fpga_mock() {
    return std::make_unique<FpgaMock>();
}

std::unique_ptr<IDeviceAdapter> create_jtag_adapter_mock() {
    return std::make_unique<JtagAdapterMock>();
}

} // namespace embedded
} // namespace workbench
