# Truss Link Firmware

Firmware for the Particle-based reconfigurable Truss Links (formerly Robot Links), developed as part of the Robot Metabolism and Robot Links research projects. This firmware enables individual links to communicate, calibrate, and execute motion commands as part of a larger truss structure.

## Table of Contents

- [Project Overview](#project-overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Running the Code](#running-the-code)
  - [Compiling](#compiling)
  - [Flashing](#flashing)
- [Code Structure](#code-structure)
- [Communication Protocol](#communication-protocol)
- [Contributing](#contributing)
- [License](#license)
- [Citation](#citation)
- [Acknowledgements](#acknowledgements)

## Project Overview

The `particleTruss` project provides the firmware for modular Truss Links. Each link runs this firmware on a Particle microcontroller, allowing it to:
- Connect to a central server.
- Receive commands for calibration, position control, sinusoidal motion, and sequenced actions (lists).
- Report its status and sensor readings.
- Perform self-calibration of its servo motors.

This firmware is a key component of research into self-assembling and reconfigurable robotic systems.

## Features

- TCP/IP communication with a central server.
- Servo motor control with position feedback.
- Self-calibration routine for servo operational range.
- Execution of various motion commands:
    - Direct position/velocity control.
    - Sinusoidal motion patterns.
    - Pre-programmed sequences of actions (lists).
- Status reporting (position, battery, current command).
- CRC-15 checksum for message integrity.

## Hardware Requirements

- Particle Photon IoT Microcontroller
- Servo motors (Two per link: `srv0` and `srv1`).
- Analog feedback sensors for servo positions.
- Associated driving circuitry for motors and sensors.

## Dependencies

- **Particle Device OS**: The firmware is built against the Particle Device OS. Ensure your device has a compatible OS version.
- **Particle CLI or Workbench**: For compiling and flashing the firmware.
  - Particle CLI: [https://docs.particle.io/getting-started/developer-tools/cli/](https://docs.particle.io/getting-started/developer-tools/cli/)
  - Particle Workbench: [https://docs.particle.io/getting-started/developer-tools/workbench/](https://docs.particle.io/getting-started/developer-tools/workbench/)

This project does not require additional third-party libraries beyond those included with the Particle Device OS, as indicated by the `project.properties` file.

## Installation

1.  **Set up Particle Environment**:
    Ensure you have the Particle CLI installed and configured, or use Particle Workbench. Log in to your Particle account.
    ```bash
    particle login
    ```
2.  **Claim Your Device**:
    If your Particle device is new, claim it to your account:
    ```bash
    particle device add YOUR_DEVICE_ID
    ```
    Or using Particle Workbench's UI.
3.  **Download Repository**:
    Clone or download this repository to your local machine.
    ```bash
    git clone <repository-url>
    cd TrussLinkFirmware
    ```

## Running the Code

### Compiling

You can compile the firmware using the Particle CLI or Particle Workbench.

**Using Particle CLI**:

Navigate to the `TrussLinkFirmware` directory (where `project.properties` is located).

To compile for a specific platform (e.g., `photon`, `argon`):
```bash
particle compile <platform>
````

For example, for a Photon:

```bash
particle compile photon
```

This will produce a `firmware.bin` file in the `target` directory (or a similar directory depending on CLI version).

**Using Particle Workbench**:
Open the `TrussLinkFirmware` folder in VS Code with Particle Workbench installed. Use the "Particle: Compile application (local)" command.

### Flashing

**Prerequisites**:

  - Your Particle device must be connected to your computer via USB and in DFU mode (blinking yellow) for local flashing, or online and connected to the Particle Cloud for cloud flashing.
  - You need the Device ID of your target Particle device.

**Using `flash.sh` script (Recommended for multiple devices)**:
The `flash.sh` script is provided to help flash multiple devices.
*Review and customize `flash.sh` before use.*
The script can be configured to flash a predefined list of device IDs or all currently online devices.
To make it executable:

```bash
chmod +x flash.sh
```

Then run it (ensure you have compiled firmware):

```bash
./flash.sh <path_to_firmware_binary_if_not_defaulting_to_cloud_flash_source>
```

The script currently uses `particle flash` without specifying a binary, implying it will attempt to flash the most recent successfully compiled firmware from the cloud for your user, or you may need to modify it to point to a local `.bin` file for each `particle flash` command (e.g., `particle flash YOUR_DEVICE_ID firmware.bin`). The active line in the script `particle list | egrep "online" | ... | xargs -n1 particle flash` will attempt to flash the latest cloud-compiled firmware to *all* your online devices that match the filter.

**Using Particle CLI (Single Device)**:
To flash a compiled binary (`firmware.bin`) to a specific device via USB:

```bash
particle flash --usb firmware.bin
```

To flash to a specific device via the cloud (device must be online):

```bash
particle flash YOUR_DEVICE_ID firmware.bin
# OR, to flash the most recently compiled cloud firmware:
particle flash YOUR_DEVICE_ID
```

**Using Particle Workbench**:
Use the "Particle: Flash application (local)" or "Particle: Flash application (cloud)" commands.

## Code Structure

The firmware is organized as follows:

  * **`/src` folder**: Contains the source code for the project.
      * `particleTruss.ino` (processed into `particleTruss.cpp`): The main application file containing `setup()` and `loop()` functions. This file orchestrates the overall behavior of the link.
      * `RMLProtocol.h`: Defines the communication protocol structures, message types, and constants used for server-client interaction.
      * `hardware.h`: Manages hardware-specific functions, including servo control, calibration logic, and execution of motion commands.
      * `utility.h`: Provides utility functions, such as device ID retrieval, time synchronization helpers, and mathematical functions.
  * `project.properties`: Specifies project metadata and dependencies.
  * `flash.sh`: A shell script to assist in flashing the firmware to multiple devices.
  * `README.md`: This file.

## Communication Protocol

The device communicates with a server over TCP/IP using a custom binary protocol defined in `RMLProtocol.h`. Key aspects:

  - Messages consist of a header (`MSG_hdr`), a body (specific to message type), and a footer (`MSG_ftr`) containing a CRC-15 checksum.
  - **Message Types (Device -\> Server)**:
      - `U` (Update): Periodic status update.
  - **Message Types (Server -\> Device)**:
      - `H` (Hello): Sent by device upon connection.
      - `T` (Time): Used to send server epoch time for synchronization.
      - `C` (Calibrate): Instructs the device to perform self-calibration.
      - `P` (Position): Command for direct servo positioning with velocity control.
      - `W` (Walk): Command for a walking gait (implementation pending).
      - `S` (Sinusoidal): Command for sinusoidal motion of servos (implementation pending).
      - `L` (List): Sends a sequence of commands to be executed.

Refer to `RMLProtocol.h` for detailed structure definitions.

## Contributing

Contributions to this project are welcome. Please fork the repository and submit a pull request with your changes. Ensure that your code adheres to the existing style and that any new features or fixes are well-documented.

## License

This project is licensed under the MIT License.

## Citation

If you use this code or draw inspiration from the associated research, please cite the following publications:

1.  **For the Reconfigurable Mechanisms and Robots Conference Paper:**

    ```bibtex
    @INPROCEEDINGS{10619984,
      author={Wyder, Philippe Martin and Bakhda, Riyaan and Zhao, Meiqi and Booth, Quinn A. and Kang, Simon and Modi, Matthew Ethan and Song, Andrew and Wu, Jiahao and Patel, Priya and Kasumi, Robert T. and Yi, David and Garg, Nihar Niraj and Bhutoria, Siddharth and Tong, Evan H. and Hu, Yuhang and Mustel, Omer and Kim, Donghan and Lipson, Hod},
      booktitle={2024 6th International Conference on Reconfigurable Mechanisms and Robots (ReMAR)},
      title={Robot Links: Towards Self-Assembling Truss Robots},
      year={2024},
      volume={},
      number={},
      pages={525-531},
      keywords={Self-assembly;Magnetic separation;Sociology;Maintenance engineering;Trajectory;Topology;Sparks},
      doi={10.1109/ReMAR61031.2024.10619984}
    }
    ```

2.  **For the Main Paper on Robot Metabolism:**

    ```bibtex
    @misc{wyder2024robotmetabolismmachinesgrow,
          title={Robot Metabolism: Towards machines that can grow by consuming other machines},
          author={Philippe Martin Wyder and Riyaan Bakhda and Meiqi Zhao and Quinn A. Booth and Matthew E. Modi and Andrew Song and Simon Kang and Jiahao Wu and Priya Patel and Robert T. Kasumi and David Yi and Nihar Niraj Garg and Pranav Jhunjhunwala and Siddharth Bhutoria and Evan H. Tong and Yuhang Hu and Judah Goldfeder and Omer Mustel and Donghan Kim and Hod Lipson},
          year={2024},
          eprint={2411.11192},
          archivePrefix={arXiv},
          primaryClass={cs.RO},
          url={[https://arxiv.org/abs/2411.11192](https://arxiv.org/abs/2411.11192)}
    }
    ```

## Acknowledgements

This work was supported by [mention any funding sources or significant contributors if applicable].
The CRC-15 implementation is credited to [https://github.com/hiharin/snappro\_xboot/blob/master/board/dm3730logic/prod-id/crc-15.c](https://www.google.com/search?q=https://github.com/hiharin/snappro_xboot/blob/master/board/dm3730logic/prod-id/crc-15.c) as noted in `RMLProtocol.h`.