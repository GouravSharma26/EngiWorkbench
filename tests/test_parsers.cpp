#include "workbench/io/stl_parser.hpp"
#include "workbench/io/csv_parser.hpp"
#include <iostream>
#include <cmath>
#include <cstdlib>

int main() {
    int total = 0;
    int passed = 0;

    std::cout << "[Test Suite: ASCII STL & CSV Parsers]" << std::endl;

    // Test 1: Bundled sample tetrahedron ASCII STL
    total++;
    auto stl_res = workbench::io::parse_ascii_stl_file("data/examples/sample_tetrahedron_ascii.stl");
    if (stl_res.is_ok()) {
        const auto& mesh = *stl_res.data;
        if (mesh.facet_count() == 4 && mesh.solid_name == "sample_tetrahedron") {
            std::cout << "  [PASS] Test 1: Bundled sample tetrahedron ASCII STL parsed 4 facets successfully" << std::endl;
            passed++;
        } else {
            std::cerr << "  [FAIL] Test 1: Mismatched facet count: " << mesh.facet_count() << std::endl;
        }
    } else {
        std::cerr << "  [FAIL] Test 1: Failed to parse sample STL: " << stl_res.error_message << std::endl;
    }

    // Test 2: Binary STL rejection
    total++;
    std::string fake_binary_stl = "\x00\x01\x02\x03\x04\x05 binary header with zero bytes";
    fake_binary_stl.resize(120, '\0');
    auto bin_res = workbench::io::parse_ascii_stl_string(fake_binary_stl);
    if (bin_res.status == workbench::io::ParseStatusCode::BinaryStlDetected) {
        std::cout << "  [PASS] Test 2: Binary STL signature correctly detected and rejected" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 2: Failed to reject binary STL" << std::endl;
    }

    // Test 3: Malformed STL (truncated loop / vertex count mismatch)
    total++;
    std::string malformed_stl = 
        "solid test\n"
        "  facet normal 0.0 0.0 1.0\n"
        "    outer loop\n"
        "      vertex 0.0 0.0 0.0\n"
        "      vertex 1.0 0.0 0.0\n"
        "    endloop\n"
        "  endfacet\n"
        "endsolid test\n";
    auto mal_stl_res = workbench::io::parse_ascii_stl_string(malformed_stl);
    if (!mal_stl_res.is_ok() && mal_stl_res.status == workbench::io::ParseStatusCode::MalformedRecord) {
        std::cout << "  [PASS] Test 3: Incomplete facet (2 vertices) properly rejected" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 3: Did not reject truncated facet" << std::endl;
    }

    // Test 4: Bundled synthetic nodal temperature CSV
    total++;
    auto csv_res = workbench::io::parse_temperature_csv_file("data/examples/synthetic_temperature_nodes.csv");
    if (csv_res.is_ok()) {
        const auto& field = *csv_res.data;
        if (field.node_count() == 4 && field.is_synthetic) {
            std::cout << "  [PASS] Test 4: Bundled synthetic CSV parsed 4 nodes with provenance label" << std::endl;
            passed++;
        } else {
            std::cerr << "  [FAIL] Test 4: Unexpected node count: " << field.node_count() << std::endl;
        }
    } else {
        std::cerr << "  [FAIL] Test 4: Failed to parse synthetic CSV: " << csv_res.error_message << std::endl;
    }

    // Test 5: Reordered CSV columns support
    total++;
    std::string reordered_csv = 
        "temperature_C,z_m,node_id,x_m,y_m\n"
        "100.5,0.0,1,0.1,0.2\n"
        "85.0,0.5,2,0.3,0.4\n";
    auto reord_res = workbench::io::parse_temperature_csv_string(reordered_csv);
    if (reord_res.is_ok() && reord_res.data->node_count() == 2) {
        std::cout << "  [PASS] Test 5: Flexible CSV column ordering handled correctly" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 5: Reordered CSV columns parsing failed" << std::endl;
    }

    // Test 6: Duplicate Node ID detection
    total++;
    std::string dup_id_csv = 
        "node_id,x_m,y_m,z_m,temperature_C\n"
        "10,0.0,0.0,0.0,100.0\n"
        "10,1.0,0.0,0.0,90.0\n";
    auto dup_res = workbench::io::parse_temperature_csv_string(dup_id_csv);
    if (!dup_res.is_ok() && dup_res.status == workbench::io::ParseStatusCode::DuplicateIdentifier) {
        std::cout << "  [PASS] Test 6: Duplicate node ID rejected with actionable status" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 6: Failed to reject duplicate node ID" << std::endl;
    }

    // Test 7: Non-finite coordinate / temperature rejection
    total++;
    std::string nan_csv = 
        "node_id,x_m,y_m,z_m,temperature_C\n"
        "1,0.0,NaN,0.0,100.0\n";
    auto nan_res = workbench::io::parse_temperature_csv_string(nan_csv);
    if (!nan_res.is_ok() && nan_res.status == workbench::io::ParseStatusCode::NonFiniteNumber) {
        std::cout << "  [PASS] Test 7: Non-finite (NaN) value rejected" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 7: Failed to reject NaN coordinate" << std::endl;
    }

    // Test 8: Empty data rows rejection (header only)
    total++;
    std::string empty_data_csv = "node_id,x_m,y_m,z_m,temperature_C\n";
    auto empty_res = workbench::io::parse_temperature_csv_string(empty_data_csv);
    if (!empty_res.is_ok() && empty_res.status == workbench::io::ParseStatusCode::EmptyFile) {
        std::cout << "  [PASS] Test 8: Header-only CSV correctly rejected as empty dataset" << std::endl;
        passed++;
    } else {
        std::cerr << "  [FAIL] Test 8: Failed to reject header-only CSV" << std::endl;
    }

    std::cout << "[Test Suite: ASCII STL & CSV Parsers] Passed " << passed << " of " << total << " tests." << std::endl;
    return (passed == total) ? EXIT_SUCCESS : EXIT_FAILURE;
}

