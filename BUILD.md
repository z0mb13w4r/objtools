# How to build on Linux based on Debian

#### Install packages:

```bash
sudo apt-get install -y make cmake autoconf libtool pkg-config gcc g++ libssl-dev binutils-dev
```

#### Clone this repo:

```bash
git clone https://github.com/z0mb13w4r/objtools.git
cd objtools
```

#### Run prerequisites script:

```bash
./prerequisite.sh
```

#### Build everything:

```bash
cd misc
make
```

### Build individual tool:

```bash
cd misc
make -f <tool name>-ng.mk all
```

