#include "workbench/embedded/adapter.hpp"
#include <iostream>
#include <cstdlib>

int main() {
    int total = 0;
    int passed = 0;

    std::cout << "[Test Suite: Embedded System Mocks]" << std::endl;

    // Test 1: Discover all three mock adapters
    total++;
    auto mcu = workbench::embedded::create_microcontroller_mock();
    auto fpga = workbench::embedded::create_fpga_mock();
    auto jtag = workbench::embedded::create_jtag_adapter_mock();

    auto d_mcu = mcu->discover();
    auto d_fpga = fpga->discover();
    auto d_jtag = jtag->discover();

    if (d_mcu.is_simulated && d_fpga.is_simulated && d_jtag.is_simulated &&
        d_mcu.type == workbench::embedded::DeviceType::Microcontroller &&
        d_fpga.type == workbench::embedded::DeviceType::Fpga &&
        d_jtag.type == workbench::embedded::DeviceType::JtagAdapter) {
        std::cout << "  [PASS] Test 1: All three mock adapters discovered and identified as SIMULATED" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 1: Device discovery failed" << std::endl;
    }

    // Test 2: Verify electrical capabilities and voltage domains
    total++;
    auto cap_mcu = mcu->capabilities();
    auto cap_fpga = fpga->capabilities();
    auto cap_jtag = jtag->capabilities();

    if (cap_mcu.voltage_reference_V == 3.3 && 
        cap_fpga.voltage_reference_V == 3.3 && 
        cap_jtag.voltage_reference_V == 3.3 &&
        !cap_mcu.supported_interfaces.empty() &&
        !cap_fpga.supported_interfaces.empty() &&
        !cap_jtag.supported_interfaces.empty()) {
        std::cout << "  [PASS] Test 2: Voltage references (3.3V) and interface capabilities properly declared" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 2: Interface capabilities mismatch" << std::endl;
    }

    // Test 3: Connect, read telemetry, disconnect lifecycle
    total++;
    bool life_ok = (mcu->state() == workbench::embedded::ConnectionState::Disconnected);
    auto conn_res = mcu->connect();
    life_ok = life_ok && conn_res.success && (mcu->state() == workbench::embedded::ConnectionState::Connected);

    auto telem_res = mcu->read_telemetry();
    life_ok = life_ok && telem_res.success && telem_res.telemetry.has_value();
    if (telem_res.telemetry.has_value()) {
        life_ok = life_ok && (telem_res.telemetry->packet_counter == 1) &&
                  (telem_res.telemetry->supply_voltage_V > 3.0);
    }

    auto disc_res = mcu->disconnect();
    life_ok = life_ok && disc_res.success && (mcu->state() == workbench::embedded::ConnectionState::Disconnected);

    if (life_ok) {
        std::cout << "  [PASS] Test 3: Standard connect -> read telemetry -> disconnect lifecycle verified" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 3: Lifecycle transition failed" << std::endl;
    }

    // Test 4: Timeout fault injection and recovery walkthrough
    total++;
    fpga->connect();
    fpga->inject_timeout(true);

    auto fail_telem = fpga->read_telemetry();
    bool fail_detected = (!fail_telem.success && fpga->state() == workbench::embedded::ConnectionState::Error);

    // Recovery
    fpga->inject_timeout(false);
    auto reconn = fpga->connect();
    auto recover_telem = fpga->read_telemetry();
    bool recovered = (reconn.success && recover_telem.success && 
                      fpga->state() == workbench::embedded::ConnectionState::Connected);

    if (fail_detected && recovered) {
        std::cout << "  [PASS] Test 4: Injected timeout fault caught and safely recovered via reconnection" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 4: Fault injection or recovery failed" << std::endl;
    }

    // Test 5: JTAG adapter designated as debug bridge only (not compute)
    total++;
    jtag->connect();
    auto jtag_telem = jtag->read_telemetry();
    if (jtag_telem.success && jtag_telem.telemetry.has_value() &&
        jtag_telem.telemetry->status_code.find("DEBUG_BRIDGE_ONLY") != std::string::npos) {
        std::cout << "  [PASS] Test 5: JTAG adapter telemetry explicitly confirms debug bridge designation" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 5: JTAG adapter telemetry missing debug bridge designation" << std::endl;
    }

    std::cout << "[Test Suite: Embedded System Mocks] Passed " << passed << " of " << total << " tests." << std::endl;
    return (passed == total) ? EXIT_SUCCESS : EXIT_FAILURE;
}
