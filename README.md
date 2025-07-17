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
@article{
doi:10.1126/sciadv.adu6897,
author = {Philippe Martin Wyder  and Riyaan Bakhda  and Meiqi Zhao  and Quinn A. Booth  and Matthew E. Modi  and Andrew Song  and Simon Kang  and Jiahao Wu  and Priya Patel  and Robert T. Kasumi  and David Yi  and Nihar Niraj Garg  and Pranav Jhunjhunwala  and Siddharth Bhutoria  and Evan H. Tong  and Yuhang Hu  and Judah Goldfeder  and Omer Mustel  and Donghan Kim  and Hod Lipson },
title = {Robot metabolism: Toward machines that can grow by consuming other machines},
journal = {Science Advances},
volume = {11},
number = {29},
pages = {eadu6897},
year = {2025},
doi = {10.1126/sciadv.adu6897},
URL = {https://www.science.org/doi/abs/10.1126/sciadv.adu6897},
eprint = {https://www.science.org/doi/pdf/10.1126/sciadv.adu6897},
abstract = {Biological lifeforms can heal, grow, adapt, and reproduce, which are abilities essential for sustained survival and development. In contrast, robots today are primarily monolithic machines with limited ability to self-repair, physically develop, or incorporate material from their environments. While robot minds rapidly evolve new behaviors through artificial intelligence, their bodies remain closed systems, unable to systematically integrate material to grow or heal. We argue that open-ended physical adaptation is only possible when robots are designed using a small repertoire of simple modules. This allows machines to mechanically adapt by consuming parts from other machines or their surroundings and shed broken components. We demonstrate this principle on a truss modular robot platform. We show how robots can grow bigger, faster, and more capable by consuming materials from their environment and other robots. We suggest that machine metabolic processes like those demonstrated here will be an essential part of any sustained future robot ecology. Robots grow bigger, faster, and more capable by absorbing and integrating material in their environment.}
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
