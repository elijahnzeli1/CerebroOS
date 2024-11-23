# CerebroOS Testing Framework

This directory contains the testing infrastructure for CerebroOS.

## Directory Structure

- `/unit_tests`: Individual component tests
- `/integration_tests`: Tests for component interactions
- `/system_tests`: Full system testing
- `/emulator`: Hardware emulation environment
- `/test_apps`: Sample applications for testing
- `/test_utils`: Testing utilities and helpers

## Running Tests

1. Unit Tests: `make test_unit`
2. Integration Tests: `make test_integration`
3. System Tests: `make test_system`
4. Full Test Suite: `make test_all`

## Emulator Usage

The emulator provides a virtual environment for testing CerebroOS without physical hardware.

Features:
- Virtual display
- Simulated input devices
- Memory monitoring
- CPU usage tracking
- Power management simulation
- Network simulation
