# Ubuntu 24.04.4 Development Environment Setup Guide

This guide covers setting up a full development environment on Ubuntu 24.04.4, split into
convenient evening sessions. Each session is self-contained with a clear stopping point.

> **Note on PDF / email:** To convert this guide to PDF, run:
> sudo apt install pandoc texlive-xetex
> pandoc ubuntu-dev-setup-guide.md -o ubuntu-dev-setup-guide.pdf --pdf-engine=xelatex

---

## Prerequisites (do once, before any session)

```bash
sudo apt update && sudo apt upgrade -y
sudo apt install -y build-essential git curl wget python3 python3-pip python3-venv \
    software-properties-common apt-transport-https ca-certificates gnupg lsb-release \
    minicom picocom screen usbutils
```

### FTDI USB/Serial access (required for all retro boards)

Ubuntu 24.04 includes the FTDI kernel driver (`ftdi_sio`) out of the box.
Add yourself to the `dialout` group so you can access serial ports without `sudo`:

```bash
sudo usermod -aG dialout $USER
# Log out and back in, or run:
newgrp dialout
```

Verify the board is detected after plugging in the USB cable:

```bash
lsusb | grep FTDI
ls /dev/ttyUSB*   # or /dev/ttyACM*
```

Connect to any serial monitor (replace `/dev/ttyUSB0` and baud rate as needed):

```bash
picocom -b 115200 /dev/ttyUSB0
# Exit picocom with Ctrl-A then Ctrl-X
```

---

## Session 1 — 6502 Development

**Estimated time: 1–2 hours**

### Tools

| Tool | Purpose |
|------|---------|
| `cc65` | C compiler and linker suite for 6502/65C02/65816 |
| `xa65` | Fast 6502 assembler (alternative) |
| `minipro` / WozMon | ROM programming / on-board monitor |

### Install cc65

```bash
sudo apt install -y cc65
cc65 --version
```

Or build from source for the latest version:

```bash
git clone https://github.com/cc65/cc65.git
cd cc65
make
sudo make install PREFIX=/usr/local
```

### Install xa65 (optional alternative assembler)

```bash
sudo apt install -y xa65
xa --version
```

### Using WozMon over serial

WozMon uses a plain ASCII protocol. Connect with picocom:

```bash
picocom -b 115200 /dev/ttyUSB0   # adjust baud to your board
```

#### Assemble + link + prepare binary (6502)

Create a tiny example in `hello.s`:

```asm
        .org $0200
        lda #$42
        sta $d020
        rts
```

Assemble and link with cc65/ca65/ld65 (or `cl65`):

```bash
ca65 hello.s -o hello.o
ld65 -t none -o hello.bin hello.o
# or with cl65 in one step:
# cl65 -t none -o hello.bin hello.s
```

Confirm byte length:

```bash
ls -l hello.bin
```

Convert binary to text bytes suitable for WozMon `AAAA: DD DD ...` input using the provided helper script:

```bash
cd /home/mstewart/Projects/Clock/6502
./BinToWoz.py hello.bin
# This writes hello.txt by default (same name + .txt)
cat hello.txt
```

If you prefer manual conversion, use Python as shown earlier.

### Upload via `WozMonLoader.py`

A convenient script can automatically upload `hello.txt` to the board by sending one line at a time and waiting for response before each send:

```bash
python3 /home/mstewart/Projects/Clock/6502/WozMonLoader.py /dev/ttyUSB0 hello.txt --baud 115200 --timeout 1.0
```

Then switch to your terminal emulator for WozMon commands (`AAAA` / `AAAAR`).

Copy each line and paste it into the WozMon prompt. WozMon accepts the same address/bytes syntax.

#### In WozMon

- `AAAA: DD DD ...` writes bytes starting at address `AAAA`.
- `AAAA` alone examines memory at that address.
- `AAAAR` runs code at address `AAAA`.

Example (after program upload):

```text
0200: A9 42 8D 20 D0 60
0200R
```

This writes the assembled machine code to $0200 and runs it.

WozMon does not require a separate file upload step when using the ASCII debugger protocol; instead the host sends writes directly.

WozMon upload notes:

- Keep lines short (16 bytes) and avoid line wrapping in your terminal emulator.
- If your board supports a block loader, use that instead for larger binaries.

WozMon quick reference:

| Command | Action |
|---------|--------|
| `AAAA`            | Examine memory at address AAAA |
| `AAAA: DD DD …`   | Write bytes DD… starting at AAAA |
| `AAAA.BBBB`       | Dump memory from AAAA to BBBB |
| `AAAAR`           | Run program at address AAAA |

### VS Code / CLion setup for 6502

Install the **65xx Assembly** VS Code extension for syntax highlighting:

```raw
Ext ID: tlgkccampbell.code-6502
```

---

**⏹ Session 1 stopping point** — 6502 toolchain installed and tested.

---

## Session 2 — 6809 Development

**Estimated time: 1–2 hours**

### Tools

| Tool | Purpose |
|------|---------|
| `lwtools` | Cross-assembler and linker for 6809/6309 |
| `cmoc` | C compiler for 6809 (Motorola 6809) |

### Install lwtools (assembler/linker)

```bash
sudo apt install -y lwtools
lwasm --version
```

Or build from source:

```bash
sudo apt install -y libpopt-dev
wget http://www.lwtools.ca/releases/lwtools/lwtools-4.23.tar.gz
tar xzf lwtools-4.23.tar.gz
cd lwtools-4.23
make
sudo make install
```

### Install cmoc (C compiler for 6809)

```bash
sudo apt install -y cmoc
cmoc --version
```

### Using Assist09 over serial

Assist09 uses a simple text protocol at 115200 baud:

```bash
picocom -b 115200 /dev/ttyUSB0
```

Assist09 quick reference:

| Command | Action |
|---------|--------|
| `M AAAA`       | Examine/modify memory at address AAAA |
| `G AAAA`       | Go (run) at address AAAA |
| `D AAAA BBBB`  | Dump memory from AAAA to BBBB |
| `L`            | Load S-record (Motorola SREC) via serial |

#### Assemble + link + prepare SREC (6809)

Create an example in `hello.asm`:

```asm
* = $1000
        LDA #$42
        STA $1000
        RTS
```

Assemble + link with lwtools:

```bash
lwasm -o hello.o hello.asm
lwlink -o hello.bin hello.o
```

Optionally use cmoc orbit for C source (single command):

```bash
cmoc -o hello.bin hello.c
```

Convert binary to Motorola S-record for Assist09:

```bash
srec_cat hello.bin -binary -offset 0x1000 -o hello.s19 -motorola
```

Upload via `L` in Assist09:

```text
L
# sends hello.s19 over serial with picocom / sx / cat as appropriate
```

Or manually use `D`/`M` commands for small payloads with byte writes.

Convert a binary to Motorola S-record format for upload:

```bash
srec_cat input.bin -Binary -offset 0x1000 -o output.s19 -Motorola
# srec_cat is part of the srecord package:
sudo apt install -y srecord
```

---

**⏹ Session 2 stopping point** — 6809 toolchain installed and tested.

---

## Session 3 — Z80 Development

**Estimated time: 1–2 hours**

### Tools

| Tool | Purpose |
|------|---------|
| `nasm` / `z80asm` | Z80 assembler |
| `sdcc` | Small Device C Compiler (supports Z80) |
| `z88dk` | Comprehensive Z80 development kit |
| `srecord` | Intel HEX file utilities |

### Install z88dk (recommended — full C + assembler suite)

```bash
sudo apt install -y z88dk
# Verify
zcc +z80 --version
```

Or install from source for the latest version:

```bash
sudo apt install -y libboost-dev
git clone --recursive https://github.com/z88dk/z88dk.git
cd z88dk
chmod +x build.sh
./build.sh
sudo make install
```

### Install SDCC (Small Device C Compiler)

```bash
sudo apt install -y sdcc
sdcc --version
```

### Install srecord (Intel HEX tools)

```bash
sudo apt install -y srecord
srec_info --version
```

Convert binary to Intel HEX for upload via the simple Z80 monitor:

```bash
srec_cat input.bin -Binary -offset 0x0100 -o output.hex -Intel
```

### Uploading Intel HEX to the simple Z80 monitor

Most simple Z80 monitors accept Intel HEX via serial. After connecting:

```bash
picocom -b 115200 /dev/ttyUSB0
```

Use picocom's send-file feature to transmit the HEX file:
Press `Ctrl-A`, then `Ctrl-S`, then enter the path to your `.hex` file.

Alternatively, use `sx` (part of `lrzsz`):

```bash
sudo apt install -y lrzsz
sx output.hex > /dev/ttyUSB0 < /dev/ttyUSB0
```

### Z80 monitor commands (Mon >)

On power-up you get a signon screen; press `M` to enter the monitor and use direct commands at `Mon >`.

- No backspace; last four digits before Return are used as the argument.
- If mistaken, continue typing the correct four digits (e.g., typing `99579958` is taken as `9958`).

Commands:

- `A` : arithmetic. Enter `Mon >A`, then `x` + Return, `y` + Return. Output: `SUM DIFF RJ`.
- `B` : return to BASIC (no args).
- `C` : copy memory. Syntax:
  - `Mon >C` + Return
  - source address + Return (e.g., `9000`)
  - dest address + Return (e.g., `9800`)
  - length (hex bytes) + Return (e.g., `80` for 0x80 bytes)
- `D` : display memory. `Mon >D9000` + Return shows 8 lines × 16 bytes.
- `G` : go/run. `Mon >G9000` + Return starts executing at 0x9000; abort with `.` if unintentional.
- `I` : Intel HEX load. Type `I` then send the file over serial; loader validates checksums, non-hex chars, and prints `.` per line.
- `M` : modify memory. `Mon >M9000` + Return enters prompted modify mode.
  - existing byte shown at each address
  - type new byte (2 hex digits) + Return to change, or Return to keep
  - `.` alone + Return exits to `Mon >`.

Argument handling: all command parameters use the last four digits before Enter; for bytes the last two digits.

#### Assemble/link/upload for Z80

Create an example `hello.asm`:

```asm
.org 0x9000
ld hl,0x9000
ld (0x9002),hl
ret
```

Assemble/link with `z80asm`/`sdcc`/`z88dk`:

```bash
z80asm -o hello.bin hello.asm
# or zcc +z80 -o hello.bin hello.c
```

Convert to Intel HEX:

```bash
srec_cat hello.bin -Binary -o hello.hex -Intel -offset 0x9000
```

Load into monitor with `I` and file transfer:

```bash
picocom -b 115200 /dev/ttyUSB0
# in monitor: I
# then send hello.hex via picocom Ctrl-A Ctrl-S or using sx / cat pipe
```

After load, run with:

```text
Mon >G9000
```

---

**⏹ Session 3 stopping point** — Z80 toolchain installed and tested.

---

## Session 4 — ESP32 Development

**Estimated time: 2–3 hours**

This is directly relevant to the **SparkFun Thing Plus - ESP32 WROOM (Micro-B)** board
and to this Clock project (which uses PlatformIO).

### 4a — PlatformIO

PlatformIO is already used in this project. Install the CLI and VS Code extension:

```bash
# Python virtual environment (recommended)
python3 -m venv ~/.platformio-venv
source ~/.platformio-venv/bin/activate
pip install platformio
pio --version
```

Install the **PlatformIO IDE** extension in VS Code:

```raw
Ext ID: platformio.platformio-ide
```

After installation, PlatformIO will auto-download the espressif32 platform and toolchain
on first build.

For the SparkFun Thing Plus board, use `board = sparkfun_esp32thing_plus` in
`platformio.ini`:

```ini
[env:sparkfun_thing_plus]
platform = espressif32
board = sparkfun_esp32thing_plus
framework = arduino
monitor_speed = 115200
```

### 4b — ESP-IDF (Espressif SDK)

Install the required dependencies:

```bash
sudo apt install -y git wget flex bison gperf python3 python3-pip python3-venv \
    cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
```

Clone ESP-IDF:

```bash
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32
```

Set up the environment (add to `~/.bashrc` for convenience):

```bash
echo 'export IDF_PATH="$HOME/esp/esp-idf"' >> ~/.bashrc
echo 'alias get_idf=". $IDF_PATH/export.sh"' >> ~/.bashrc
source ~/.bashrc
```

Each new terminal session, activate the IDF environment:

```bash
get_idf
```

### 4c — VS Code extensions for ESP32

| Extension | ID |
|-----------|----|
| C/C++ | `ms-vscode.cpptools` |
| ESP-IDF | `espressif.esp-idf-extension` |
| PlatformIO IDE | `platformio.platformio-ide` |
| CMake Tools | `ms-vscode.cmake-tools` |

### 4d — CLion ESP-IDF integration

1. Install the **ESP-IDF** plugin from CLion's Marketplace.
2. Set the IDF path to `~/esp/esp-idf` in Settings → Languages & Frameworks → ESP-IDF.

---

**⏹ Session 4 stopping point** — ESP32 toolchain (IDF + PlatformIO) installed and tested.

---

## Session 5 — FPGA Development

**Estimated time: 2–4 hours (Vivado download is large ~15 GB)**

### 5a — IceBreaker (open-source toolchain: yosys / nextpnr)

```bash
sudo apt install -y yosys nextpnr-ice40 icestorm fpga-icestorm
# Verify
yosys --version
nextpnr-ice40 --version
iceprog --help
```

If the Ubuntu packages are outdated, build from source:

**yosys from source:**

```bash
sudo apt install -y build-essential clang bison flex libreadline-dev \
    gawk tcl-dev libffi-dev git graphviz xdot pkg-config python3 \
    libboost-system-dev libboost-python-dev libboost-filesystem-dev zlib1g-dev
git clone https://github.com/YosysHQ/yosys.git
cd yosys && make -j$(nproc) && sudo make install
```

**nextpnr-ice40 from source:**

```bash
sudo apt install -y libboost-all-dev libeigen3-dev
git clone https://github.com/YosysHQ/nextpnr.git
cd nextpnr
cmake . -DARCH=ice40 -DICESTORM_INSTALL_PREFIX=/usr/local
make -j$(nproc) && sudo make install
```

**icestorm tools (iceprog for flashing):**

```bash
git clone https://github.com/YosysHQ/icestorm.git
cd icestorm && make -j$(nproc) && sudo make install
```

Allow non-root access to the IceBreaker USB device:

```bash
sudo cp /usr/local/share/icestorm/99-icestick.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules && sudo udevadm trigger
```

**Typical IceBreaker build flow:**

```bash
yosys -p "synth_ice40 -json design.json" design.v
nextpnr-ice40 --up5k --package sg48 --json design.json --pcf icebreaker.pcf --asc design.asc
icepack design.asc design.bin
iceprog design.bin
```

### 5b — Basys3 (Vivado ML Standard Edition)

1. Create or log in to your AMD/Xilinx account at https://www.amd.com/en/support/download/tools.html
2. Download **Vivado ML Standard Edition** (v2024.x recommended) — approximately 15 GB.
3. Install dependencies:

```bash
sudo apt install -y libncurses5 libncurses5-dev libtinfo5 libstdc++6 \
    libxtst6 libxt6 libxrender1 libxi6 libc6-i386
```

4. Run the installer:

```bash
chmod +x Xilinx_Unified_*.bin
sudo ./Xilinx_Unified_*.bin
```

   Select: **Vivado → Vivado ML Standard** → install to `/tools/Xilinx`.

5. Install Digilent Basys3 board files:

```bash
git clone https://github.com/Digilent/vivado-boards.git
sudo cp -r vivado-boards/new/board_files/* \
    /tools/Xilinx/Vivado/<version>/data/boards/board_files/
```

6. Source the Vivado environment:

```bash
echo 'source /tools/Xilinx/Vivado/<version>/settings64.sh' >> ~/.bashrc
source ~/.bashrc
```

   Replace `<version>` with the installed version (e.g., `2024.1`).

7. Install Digilent cable drivers:

```bash
cd /tools/Xilinx/Vivado/<version>/data/xicom/cable_drivers/lin64/install_script/install_drivers
sudo ./install_drivers
```

### 5c — VS Code FPGA extensions

| Extension | ID |
|-----------|----|
| Digital IDE | `sterben.fpga-support` |
| VHDL by Puorc | `puorc.awesome-vhdl` |
| Verilog-HDL | `mshr-h.veriloghdl` |
| TerosHDL | `teros-technology.teroshdl` |

---

**⏹ Session 5 stopping point** — FPGA toolchains installed. Vivado download may continue overnight.

---

## Session 6 — PIC Assembly Development

**Estimated time: 2–3 hours**

This session sets up a PIC microcontroller assembly development environment for use with the
**HP488 PIC Board**, a **PICKit 3** programmer/debugger, and an **ICD (In-Circuit Debugger)
adapter**.

### Tools

| Tool | Purpose |
|------|---------|
| `gputils` (`gpasm`, `gplink`, `gplib`) | Open-source GNU PIC assembler and linker |
| MPLAB X IDE | Microchip's official IDE — required for PICKit 3 and ICD debugging |
| MPLAB XC8 Compiler | Microchip's 8-bit PIC C/assembly compiler (integrates with MPLAB X) |
| MPLAB IPE | Standalone programmer (bundled with MPLAB X) |

### 6a — Install gputils (open-source PIC assembler)

`gputils` provides `gpasm` (assembler), `gplink` (linker), and `gplib` (librarian) for PIC
microcontrollers. It is available in the Ubuntu package repository:

```bash
sudo apt install -y gputils
gpasm --version
```

To assemble and link a PIC program without MPLAB X:

```bash
gpasm -p p16F877A my_program.asm        # assemble (adjust device as needed)
gplink -m -o my_program.hex my_program.o   # link to Intel HEX
```

### 6b — Install MPLAB X IDE (required for PICKit 3 / ICD)

MPLAB X IDE provides full PICKit 3 programmer and ICD debugger support on Linux.

1. Download the latest Linux installer from Microchip's website:

   https://www.microchip.com/mplab/mplab-x-ide

   Choose the **Linux** version (`.tar`).

2. Extract and run the installer:

   tar -xf MPLABX-v*.tar
   cd MPLABX-v*-linux-installer/
   sudo ./MPLABX-v*-linux-installer.sh

   Install to the default path `/opt/microchip/mplabx/<version>`.

3. Add MPLAB tools to your `PATH` (add to `~/.bashrc`):

   echo 'export PATH="/opt/microchip/mplabx/$(ls /opt/microchip/mplabx/ | tail -1)/mplab_platform/bin:$PATH"' >> ~/.bashrc
   source ~/.bashrc

4. Verify:

   mplabx --version    # or launch: /opt/microchip/mplabx/<version>/bin/mplabx

### 6c — Install MPLAB XC8 Compiler

The XC8 compiler targets all 8-bit PIC devices and integrates directly with MPLAB X.

1. Download the Linux installer from:

   https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers

2. Install:

   chmod +x xc8-v*.sh
   sudo ./xc8-v*.sh --mode unattended --unattendedmodeui minimal \
       --installdir /opt/microchip/xc8/<version>

3. Verify:

   xc8 --version

   MPLAB X will auto-detect XC8 on first launch.

### 6d — PICKit 3 USB access (udev rules)

The PICKit 3 is a USB HID device. Grant non-root access with a udev rule:

```bash
sudo tee /etc/udev/rules.d/99-microchip-pickit3.rules << 'EOF'
# Microchip PICKit 3 programmer/debugger
SUBSYSTEM=="usb", ATTR{idVendor}=="04d8", ATTR{idProduct}=="900a", \
    MODE="0660", GROUP="plugdev", TAG+="uaccess"
EOF

sudo udevadm control --reload-rules && sudo udevadm trigger
sudo usermod -aG plugdev $USER
# Log out and back in to pick up the new group
```

Verify the PICKit 3 is detected after plugging in via USB:

```bash
lsusb | grep -i "04d8"   # should show "Microchip Technology, Inc. PICkit3"
```

### 6e — ICD adapter wiring (PICKit 3 ↔ HP488 board)

The ICD (In-Circuit Debugger) 6-pin connector on the PICKit 3 maps as follows:

| PICKit 3 pin | Signal | HP488 ICD header |
|:---:|--------|:---:|
| 1 | MCLR / VPP | MCLR |
| 2 | VDD (target power) | VDD |
| 3 | GND | GND |
| 4 | ICSPDAT (PGD) | PGD |
| 5 | ICSPCLK (PGC) | PGC |
| 6 | LVP (optional) | LVP |

> **Note:** Verify pin 1 orientation on the HP488 board's ICD header — pin 1 is usually
> marked with a triangle or notch. Only connect VDD (pin 2) if the HP488 board is not
> already powered independently.

Enable LVP (Low-Voltage Programming) in your MPLAB X project if the target PIC requires it:
**Project Properties → PICKit 3 → Power → Low Voltage Programming Mode**.

### 6f — Programming and debugging workflow

#### Option 1: MPLAB X IDE (full GUI, recommended for ICD debugging)

1. Open MPLAB X IDE.
2. **File → New Project** → select **Microchip Embedded → Standalone Project**.
3. Choose your PIC device (e.g., `PIC16F877A`) and select **PICKit 3** as the hardware tool.
4. Select the **ICD** connector type in Project Properties → PICKit 3 → Connection.
5. Add your `.asm` source files and set the language toolsuite to **MPASM™ Assembler** (or
   XC8 if using C).
6. **Run → Build Project**, then **Run → Program Device**.
7. For live debugging: **Debug → Debug Project** — breakpoints, watch variables, and
   single-step are all available via the ICD connection.

#### Option 2: Command-line with gpasm + MPLAB IPE

Assemble with gputils and flash with MPLAB IPE (bundled with MPLAB X):

```bash
# Assemble
gpasm -p p16F877A -o my_program.o my_program.asm
gplink -m -o my_program.hex my_program.o

# Flash via MPLAB IPE (non-interactive)
/opt/microchip/mplabx/<version>/mplab_ipe/ipecmd.sh \
    -TPPK3 \
    -P16F877A \
    -F my_program.hex \
    -M
```

> Replace `<version>` and the device (`16F877A`) with your actual values.
> `-M` programs all memory regions; add `-E` to erase first if needed.

### 6g — VS Code extension for PIC assembly

Install the **MPASM** syntax-highlighting extension for VS Code:

```raw
Ext ID: bjorn-w.mpasm
```

Or use a generic assembly highlighter:

```raw
Ext ID: maziac.asm-code-lens
```

---

**⏹ Session 6 stopping point** — PIC assembly toolchain installed, PICKit 3 accessible,
HP488 board programmed and debugged over ICD.

---

## Session 7 — Remote AI Development

**Estimated time: 1–2 hours (on local machine) + server admin time**

### 7a — SSH setup (local machine)

Generate an SSH key pair if you don't already have one:

```bash
ssh-keygen -t ed25519 -C "your-email@example.com"
ssh-copy-id username@ai-server-hostname
```

Test the connection:

```bash
ssh username@ai-server-hostname
```

Add a convenient alias to `~/.ssh/config`:

```raw
Host aiserver
    HostName ai-server-hostname
    User username
    IdentityFile ~/.ssh/id_ed25519
    ServerAliveInterval 60
```

Then simply: `ssh aiserver`

### 7b — VS Code Remote-SSH

Install the **Remote - SSH** extension:

```raw
Ext ID: ms-vscode-remote.remote-ssh
```

Connect: press `F1` → **Remote-SSH: Connect to Host…** → select `aiserver`.

VS Code will install its server component on the remote machine automatically.

### 7c — CUDA development tools (install on AI server)

```bash
# On the AI server — check NVIDIA driver
nvidia-smi

# Add CUDA repository (example for CUDA 12.x on Ubuntu 24.04)
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2404/x86_64/cuda-keyring_1.1-1_all.deb
sudo dpkg -i cuda-keyring_1.1-1_all.deb
sudo apt update
sudo apt install -y cuda-toolkit-12-4

# Add to PATH (on the AI server ~/.bashrc)
echo 'export PATH=/usr/local/cuda/bin:$PATH' >> ~/.bashrc
echo 'export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc
nvcc --version
```

### 7d — Python AI/ML environment (on AI server)

```bash
# On the AI server
python3 -m venv ~/ai-env
source ~/ai-env/bin/activate
pip install --upgrade pip
pip install numpy pandas matplotlib scikit-learn
# TensorFlow with GPU support (syntax valid for TF 2.15+; verify current docs at https://www.tensorflow.org/install/pip)
pip install tensorflow[and-cuda]
# Or PyTorch — match the cu<VER> suffix to your installed CUDA version (check with: nvidia-smi)
# e.g. cu118 for CUDA 11.8, cu121 for CUDA 12.1, cu124 for CUDA 12.4
pip install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cu124
```

### 7e — PyCharm remote interpreter

1. Open PyCharm → Settings → Project → Python Interpreter → **Add Interpreter** → **On SSH**.
2. Fill in the SSH host details and select the `~/ai-env` virtual environment on the server.

### 7f — CLion remote development

1. Settings → Build, Execution, Deployment → **Toolchains** → add **Remote Host**.
2. Configure the SSH connection and set the remote GCC/CMake paths.

---

**⏹ Session 7 stopping point** — Remote AI development environment configured.

---

## Quick-reference: evening schedule

| Evening | Session | Content |
|---------|---------|---------|
| 1 | Session 1 | 6502 toolchain |
| 1 | Session 2 | 6809 toolchain |
| 2 | Session 3 | Z80 toolchain |
| 2 | Session 4 | ESP32 (PlatformIO + IDF) |
| 3 | Session 5a | IceBreaker FPGA (yosys/nextpnr) |
| 3–4 | Session 5b | Basys3 FPGA (Vivado — start download early) |
| 4 | Session 6 | PIC assembly (HP488 + PICKit 3 + ICD) |
| 5 | Session 7 | Remote AI development |

---

## Troubleshooting

### Serial port permission denied

```bash
sudo usermod -aG dialout $USER
# Then log out and back in
```

### FTDI device not recognised

```bash
sudo modprobe ftdi_sio
lsusb -v | grep -i ftdi
```

### iceprog: can't open device

Ensure udev rules are installed and the user is in the `plugdev` group:

```bash
sudo usermod -aG plugdev $USER
```

### Vivado: libtinfo not found

```bash
sudo apt install -y libtinfo5
# If not available:
sudo ln -sf /lib/x86_64-linux-gnu/libtinfo.so.6 /lib/x86_64-linux-gnu/libtinfo.so.5
```

### PICKit 3: device not found / permission denied

Ensure the udev rule is installed and you are in the `plugdev` group:

```bash
sudo usermod -aG plugdev $USER
# Log out and back in, or:
newgrp plugdev
lsusb | grep "04d8"   # check PICKit 3 is detected
```

If the rule still doesn't apply, reload udev:

```bash
sudo udevadm control --reload-rules && sudo udevadm trigger
```

### PICKit 3: "target not detected" or VDD error

- Confirm the ICD 6-pin cable is fully seated on both the PICKit 3 and HP488 board headers.
- Check pin 1 alignment (marked by a triangle/notch on the connector).
- If the HP488 board is self-powered, disable "Power target from PICKit 3" in
  **Project Properties → PICKit 3 → Power**.
- If using LVP mode, ensure the PIC device's LVP configuration bit matches the project
  settings.

### MPLAB IPE: `ipecmd.sh` not found

Verify your MPLAB X installation path and substitute the correct version number:

```bash
ls /opt/microchip/mplabx/
/opt/microchip/mplabx/<version>/mplab_ipe/ipecmd.sh --help
```
