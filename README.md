# Truss Links - Self-Assembling Truss Robots

This repository contains the resources for building and working with self-assembling truss robots developed as part of the Robot Metabolism project.

For the latest version TrussLinkSimulation and TrussLinkServer, please refer to the linked repositories on [https://robotmetabolism.github.io/](https://robotmetabolism.github.io/).

## Repository Structure

```
.
├── 3D_files/                          # 3D models and mesh files for printing Truss Link components
│   ├── BodyParts/                     
│   ├── ConnectorParts/                
│   └── ExperimentEnvironment/         
├── RM_AprilTagLayoutGen/              # AprilTag generation and detection tools
│   ├── atagmain.py                    # PDF layout generator for AprilTag strips
│   ├── detect_apriltags.py            # AprilTag detection from images/PDFs
│   ├── flipcroptags.py                # Image preprocessing utility
│   ├── maketags.sh                    # Convenience script for tag generation
│   ├── tag36h11_big/                  # Raw AprilTag images
│   ├── tag36h11_big_cropped_flipped/  # Processed AprilTag images
│   └── tags.pdf                       # Example generated tag layout
├── Robot Link Build Guide.docx        # Assembly instructions (Word format)
├── Robot Link Build Guide.pdf         # Assembly instructions (PDF format)
└── TrussLinkFirmware/                 # Embedded firmware for robot links
    ├── flash.sh                       # Firmware flashing script
    ├── src/                           # Source code
    └── README.md                      # Firmware documentation
```

## Components

### 3D Files
Contains 3D models and mesh files for printing the Truss Link components. Use these files with your 3D printer to manufacture the physical robot parts.

### AprilTag Tools
Complete suite for generating and detecting AprilTag visual markers used to track robot links. See `RM_AprilTagLayoutGen/README.md` for detailed usage instructions.

### Build Guide
Step-by-step assembly instructions available in both Word and PDF formats. Follow this guide to assemble your printed components into functional robot links.

### Firmware
Embedded software for the robot links. See `TrussLinkFirmware/README.md` for compilation and flashing instructions.

## Quick Start

1. **3D Print**: Use files in `3D_files/` to print robot components
2. **Assemble**: Follow the `Robot Link Build Guide.pdf` for assembly
3. **Generate Tags**: Use tools in `RM_AprilTagLayoutGen/` to create AprilTags
4. **Program**: Flash firmware from `TrussLinkFirmware/` to your robot links

## Citation
If you use this work in your research, please cite:

```bibtex
@article{wyder2025robot,
            title        = {Robot Metabolism: Towards Machines That Can Grow by Consuming Other Machines},
            author       = {Wyder, Philippe Martin and Bakhda, Riyaan and Zhao, Meiqi and Booth, Quinn A. and Modi, Matthew E. and Song, Andrew and Kang, Simon and Wu, Jiahao and Patel, Priya and Kasumi, Robert T. and Yi, David and Garg, Nihar Niraj and Jhunjhunwala, Pranav and Bhutoria, Siddharth and Tong, Evan H. and Hu, Yuhang and Goldfeder, Judah and Mustel, Omer and Kim, Donghan and Lipson, Hod},
            journal      = {Science Advances},
            year         = {2025},
            month        = {jul},
            day          = {16},
            doi          = {10.1126/sciadv.adu6897},
            url          = {https://www.science.org/doi/10.1126/sciadv.adu6897}
          }
```

```bibtex
@INPROCEEDINGS{10619984,
  author={Wyder, Philippe Martin and Bakhda, Riyaan and Zhao, Meiqi and Booth, Quinn A. and Kang, Simon and Modi, Matthew Ethan and Song, Andrew and Wu, Jiahao and Patel, Priya and Kasumi, Robert T. and Yi, David and Garg, Nihar Niraj and Bhutoria, Siddharth and Tong, Evan H. and Hu, Yuhang and Mustel, Omer and Kim, Donghan and Lipson, Hod},
  booktitle={2024 6th International Conference on Reconfigurable Mechanisms and Robots (ReMAR)}, 
  title={Robot Links: Towards Self-Assembling Truss Robots}, 
  year={2024},
  pages={525-531},
  doi={10.1109/ReMAR61031.2024.10619984}
}
```
