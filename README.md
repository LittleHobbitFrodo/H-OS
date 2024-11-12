# H-OS

### H-OS is simple operating system project

H-OS is still at a **very early stage of development**, the vast majority of things are not yet operational and some that are operational do not work as they should. There is **no guarantee that the system will work**

## Main Goal
The goal is to create a simple well documented operating system that will also serve as a good example for other novice operating system developers

## Philosophy
H-OS will provide a friendly environment with easy interaction with the system. Conventions will provide meaningful names with simple syntax for easier understanding of commands, paths and libraries. Command conventions specify a "word" syntax that even a complete novice should understand

## What will it provide
H-OS is meant to be very simple multitask operating system with its own filesystem design, core utilities and standards

The project source code should be "simple", segmented into many functions for better readability and well commented

The project will also include a "book" clarifying what is going on under the hood in a relatively clear presentation

## For other developers
**The book is not a manual. It is just a companion material to make things easier to understand with links to code examples. The prospective developer is expected to have at least some experience with programming and operating systems.**

## Licence
The whole project with all of its parts is under the GPL v3.0 licence


# Building
**NOTE**: using the `./scritps/test` scripts may create an image with different compiler options then used normally. Please use `./scripts/build mkimage` command to build the OS
### Requirements
**Linux** operating system or virtual machine (BSD may be option, but it's not tested)

- **[Ubuntu](https://ubuntu.com/)** can be a good choice for beginners with linux

- For more experienced Linux users, I recommend **[Arch Linux](https://archlinux.org/)** for better accessibility to necessary packages
  - Also, here is link to **[automated installation](https://alci.online/)** of Arch Linux for faster installation

**[Fish](https://fishshell.com/)** (shell)
- Most of the scripts used in the build are written in it

**[Parted](https://wiki.archlinux.org/title/Parted)** - tool used for disk image creation
- Since the arch wiki has proven to be a good resource here is a link. If you are currently using any other distribution the way of installation may differ

**[Mtools](https://www.gnu.org/software/mtools/)** - tools used for disk image creation (together with parted)
  - Mtools is widely used, your distribution should provide a package for it

**[Limine](https://limine-bootloader.org/)** - bootloader used to start the system
- Arch Linux has an official package for limine
- H-OS also provide **script for limine installation** (detailed below)

**[NASM](https://www.nasm.us/)** - assembler (assembly compiler)
- Nasm is widely used assembler, most distributions should provide a package for it.


### **C compiler**
- If you are on x86_64 system (run ```uname -m``` in terminal to check it), install ```gcc``` package.
The gcc package should also contain ```ld``` **command**. Make sure this is the case.
  - Be aware that the native **compiler (gcc) can cause problems**. So please **consider building cross-compiler (x86_64-elf-gcc)**


- In case you don't have x86_64/amd64 architecture you will most likely have to **build cross-compiler** called ```x86_64-elf-gcc``` which should provide everything you need.
    - For Arch linux users it is available as **[AUR package](https://aur.archlinux.org/packages/x86_64-elf-gcc)**. Building of the cross-compiler is long process: it may take more than one hour
    - In this case ```x86_64-elf-ld``` **command** should be installed as well. Make sure that this is indeed the case.


**[Qemu](https://www.qemu.org/)** - Quick EMUlator (for testing and emulation) - **Optional** dependency

## Requirement installation
- **Ubuntu**: ```sudo apt update && sudo apt install build-essential fish parted mtools nasm```
  - limine and cross-compiler must be installed manually
- **Arch Linux**: ```sudo pacman -Syyu && sudo pacman -S gcc fish parted mtools limine nasm```
    - Alternatively you can install the cross-compiler with this command ```yay -S x86_64-elf-gcc```. The compiler has to be built, which can take more than an hour on weaker machines
- **Fedora**: ```sudo dnf update && sudo dnf install gcc fish parted mtools nasm```
  - limine  and cross-compiler must be installed manually


### **Limine** installation script
- **Please be aware that the script requires administrator (root) privileges**
  - Any malfunction can drastically damage your system.
  - **There is no guarantee** that the script will always work as intended.
- **Please consider reading the script or manually installing limine according to the steps specified by your linux distribution**
- To use the script run this command in project root directory ```cd ./stuff/limine/build/; chmod +x ./build;./build```

## How do I build the OS?
This contains just few steps:
- Clone the repository - run ```git clone https://github.com/LittleHobbitFrodo/H-OS```
- Enter project root directory ```cd H-OS```
- Set permissions to scripts ```chmod -R +x ./scripts/;chmod -R +r ./scripts/```
- Start build ```./scripts/build mkimage```
- If no error was reported **disk image** (H-OS.hdd file) should be created in project root directory

### Building with docker
To build the OS in docker run this command:
```docker pull archlinux; ```

You can upload it to a flash drive, **the upload will completely erase the drive so please check if there is any data you could miss on the drive**
- Insert the drive, open file manager and copy the disk image (H-OS.hdd) into the disk
- Alternatively you can do that in command line with the "dd" command **(not recommended for beginners)** ```sudo dd if=<project root dir>/H-OS.hdd of=/dev/<drive>```

## Current progress
 - Keyboard Input: my own goofy implementation (I will fix it later (maybe))
 - Hardware Enumeration: gather information about connected hardware
 - Paging: I should really make it work

## Tasks
  - [x] **Hello world!** - Create bootable disk that displays simple ```hello world``` message.
  - [x] **Set up environment** - Add basic functionalities like printing numbers.
  - [x] **CPUID** - Gather some information about processor.
  - [ ] **Heap implementation**
    - [x] Find place for kernel heap.
    - [x] Create simple allocator.
    - [x] Add other functionalities.
    - [ ] Port the allocator for virtual addresses.
  - [ ] **Memory initialization**
    - [x] Parse and simplify memory map.
    - [x] Initialize GDT with TSS.
    - [ ] Reinitialize paging.
  - [x] **Interrupts**
    - [x] Get exception handling to work.
    - [x] Initialize other interrupts.
    - [x] Time and timer interrupt
  - [ ] **UEFI runtime services**
    - [ ] Create simple library for UEFI runtime services
  - [ ] **Power Management**
    - [x] Shutdown (Emulator specific only)
  - [ ] Hardware discovery
    - [ ] PCI
    - [ ] ACPI

...