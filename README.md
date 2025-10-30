# objtools

These tools were originally designed to be a research project and drop-in replacement for readelf, objcopy and objdump utilising Capstone disassembly framework. The project then started to grow to become a collection of tools for malware and binary analysis.

**convert-ng** is a Swiss Army Knife for manipulating data in complex ways using operations", "xor, addition, substruction, logical shifts and rotations, base32 and base64.

**objcopy-ng** copies the contents of an object file to another using the GNU BFD Library to readand write the object files. It can write the destination object file in a format different from that of the source object file.

**objdump-ng** displays information about one or more object files. This information is extremely useful to malware researchers and binary analysis as this utility is light-weight compared to many commercial applications.

**objdwarf-ng** displays the dwarf information about one or more ELF format object files in standard and enhanced formats.

**objhash-ng** generates hashes of ELF format object files for malware detection and digitial forensics. The utility is capable of fuzz-hashing and context-triggered piecewise hashing.

**readelf-ng** displays information about one or more ELF format object files.

**readpe-ng** displays information about one or more PE format object files.

### Support ###

If you can contribute or you want to, feel free to do it at [PayPal](https://www.paypal.com/ncp/payment/L6UQHUDXKSHTC), I will be really thankfull for anything even if it is a coffee because that helps me a lot to know that you care:)

If you find this project useful and would like to keep it maintained, have new features added and regular release cycle. Then could you think about sponsorship at [PayPal](https://www.paypal.com/ncp/payment/2DAZRCQM7MDFG).

### Dependencies ###

https://github.com/capstone-engine/capstone - BSD license

https://github.com/davea42/libdwarf-code - LGPL license

### License ###

objtools is published under the MIT license.

