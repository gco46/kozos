## Requirement

- H8/3069F board (bought via akizuki: K-01271)
- RS-232C cable (bought via akizuki: C-00004)

- gcc==3.4.6
- binutils==2.21
- kz_h8write==0.2.1

### OS

- Ubuntu18.04.5 LTS (64 bit)

### terminal emulator

- minicom==2.7.1

When you change settings, execute a following command. (default japanese menu contains bugs)

```
sudo LANG=C minicom -o -s
```

To use minicom, execute a following command

```
sudo minicom -o
```



### others

- 5V/2A AC adapter (bought via akizuki: M-01801)
- USB-Serial converter (bought via akizuki: M-02746)



## Installation

### gcc

1. Get gcc from wget

```
wget http:/core.gr.jp/pub/GNU/gcc/gcc-3.4.6/gcc-3.4.6.tar.gz
tar zxvf gcc-3.4.6.tar.gz
```

2. Collect `gcc/collect2.c`, line 1537:

```c
redir_handle = open(redir, O_WRONLY | O_TRUNC | O_CREAT);
```

to

```c
redir_handle = open(redir, O_WRONLY | O_TRUNC | O_CREAT, 0755);
```

3. Apply the patch

```
(in gcc-3.4.6/)
wget http://kozos.jp/books/makeos/patch-gcc-3.4.6-x64-h8300.txt
patch -p0 < patch-gcc-3.4.6-x64-h8300.txt
```

4. Build

```
(in gcc-3.4.6/)
mkdir build
cd build
../configure --target=h8300-elf --disable-nls --disable-threads --disable-shared --enable-languages=c --disable-werror
make
sudo make install
```



### binutils

just do it (`ld.scr` must be modified to fix alignment bug:https://groups.google.com/g/kozos_tomonokai/c/HiyFwp-lt6M)

### kz_h8write

1. download via following url: https://ja.osdn.net/projects/kz-h8write/
2. unzip and build

```
unzip kz_h8write-v0.2.1.zpi
cd PackageFiles/src
make
```

3. Don't forget to modify the Makefile path. 

