# objtools-ng

These tools were originally designed as part of a research project on industrial control systems (ICSs) and the Internet of Things (IoT). A drop-in replacement for readelf, objcopy, and objdump that utilises the Capstone disassembly framework. The project then grew into a collection of tools for malware and binary analysis supporting x86, ARM, ARM Thumb, AARCH64, MIPS and RISC-V architectures.

This suite of tools was inspired by a problem I encountered while comparing two subtly different executables that were compiled from the same source code. Also, having binaries built with different cross-compilers raises the question of which flavour of readelf or objdump to use? After I reached 12 and started thinking about a simple solution that could be automated and didn't require a $ 7,000-a-year software license.

**convert-ng** is a Swiss Army Knife for manipulating data in complex ways using operations, xor, addition, subtraction, logical shifts and rotations, base32, base58, base64, base85 and vigeneree cipher. [READ MORE...](https://github.com/z0mb13w4r/objtools/wiki/convert%E2%80%90ng)

**enumerate-ng.py** is a script inspired by rebootuser’s LinEnum for enumeration and privilege escalation, and enhanced to be used with both devices and firmware images. This information is useful for hardening devices or for understanding how an adversary obtained persistence, privilege escalation, or lateral movement. [READ MORE...](https://github.com/z0mb13w4r/objtools/wiki/enumerate%E2%80%90ng)

**objcopy-ng** copies the contents of an object file to another using the GNU BFD Library to readand write the object files. It can write the destination object file in a format different from the source object files.

**objdump-ng** displays information about one or more object files. This information is useful to malware researchers and binary analysis, as this utility is lightweight compared to many commercial applications (x86, ARM, AARCH64, MIPS and RISC-V). [READ MORE...](https://github.com/z0mb13w4r/objtools/wiki/objdump%E2%80%90ng)

**objdwarf-ng** displays the dwarf information about one or more ELF format object files in standard and enhanced formats. [READ MORE...](https://github.com/z0mb13w4r/objtools/wiki/objdwarf%E2%80%90ng)

**objhash-ng** generates hashes of ELF format object files for malware detection and digital forensics. The utility is supports fuzz-hashing and context-triggered piecewise hashing.

**readelf-ng** displays information about one or more ELF format object files. [READ MORE...](https://github.com/z0mb13w4r/objtools/wiki/readelf%E2%80%90ng)

**readpe-ng** displays information about one or more PE format object files.

### Builds ###

Download the latest [release](https://github.com/z0mb13w4r/objtools/releases).

### How to build ###

See the [BUILD.md](BUILD.md) for detailed instructions.

### Support & Sponsorship ###

If you find this project useful and would like to keep it maintained, with new features and a regular release cycle or want to support my research. Then, you can sponsor me at [PayPal](https://www.paypal.com/ncp/payment/2DAZRCQM7MDFG), or you can buy me a coffee at [PayPal](https://www.paypal.com/ncp/payment/L6UQHUDXKSHTC). I will be really thankful for anything, even if it is a coffee, because that helps me a lot to know that you care:)

If you require a service contract that includes: email support, technical help, support tickets, prioritised bug fixes with immediate release. Drop me an email: enquiries@uber-techie.co.uk

### Dependencies ###

https://github.com/capstone-engine/capstone - BSD license

https://github.com/ssdeep-project/ssdeep - GPL-2.0 license

https://github.com/davea42/libdwarf-code - LGPL license

https://github.com/packing-box/peid - GPL-3.0 license

### License ###

objtools-ng is published under the MIT license.

