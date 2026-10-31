#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <cstdint>

namespace workbench {
namespace embedded {

enum class DeviceType {
    Microcontroller,
    Fpga,
    JtagAdapter
};

enum class ConnectionState {
    Disconnected,
    Connecting,
    Connected,
    Error
};

struct DeviceCapabilities {
    bool can_read_telemetry{true};
    bool can_program_target{false};
    bool can_reset_target{false};
    double voltage_reference_V{3.3};
    std::string logic_standard{"LVCMOS 3.3V"};
    std::vector<std::string> supported_interfaces;
};

struct DeviceDescriptor {
    std::string id;
    DeviceType type{DeviceType::Microcontroller};
    std::string model_name;
    std::string vendor;
    std::string serial_number;
    std::string simulated_transport;
    bool is_simulated{true}; // Always true for this prototype
};

struct TelemetryRecord {
    uint64_t timestamp_ms{0};
    double core_temperature_C{0.0};
    double supply_voltage_V{0.0};
    double current_mA{0.0};
    uint32_t packet_counter{0};
    std::string status_code{"OK"};
};

struct AdapterResult {
    bool success{false};
    std::string message;
    std::optional<TelemetryRecord> telemetry{std::nullopt};
};

/// Pure virtual contract for embedded hardware adapters.
class IDeviceAdapter {
public:
    virtual ~IDeviceAdapter() = default;

    /// Discovers and returns device descriptor.
    virtual DeviceDescriptor discover() = 0;

    /// Returns device electrical and interface capabilities.
    virtual DeviceCapabilities capabilities() const = 0;

    /// Connects to the simulated device.
    virtual AdapterResult connect() = 0;

    /// Disconnects from the simulated device.
    virtual AdapterResult disconnect() = 0;

    /// Reads simulated telemetry from the device.
    virtual AdapterResult read_telemetry() = 0;

    /// Current connection state.
    virtual ConnectionState state() const = 0;

    /// Injects a deterministic simulated timeout fault.
    virtual void inject_timeout(bool enable) = 0;
};

/// Factory functions for the three reference devices:
std::unique_ptr<IDeviceAdapter> create_microcontroller_mock();
std::unique_ptr<IDeviceAdapter> create_fpga_mock();
std::unique_ptr<IDeviceAdapter> create_jtag_adapter_mock();

} // namespace embedded
} // namespace workbench

