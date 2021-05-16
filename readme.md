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

設定を変更するときは、以下のコマンドで英語表示にする。（日本語表記にはバグがあるため）

```
sudo LANG=C minicom -o -s
```

minicom起動は以下のコマンドで行う。

```
sudo minicom -o
```



### others

- 5V/2A AC adapter (bought via akizuki: M-01801)
- USB-Serial converter (bought via akizuki: M-02746)



## Installation

### gcc

1. wgetコマンドでgccを取得する

```
wget http:/core.gr.jp/pub/GNU/gcc/gcc-3.4.6/gcc-3.4.6.tar.gz
tar zxvf gcc-3.4.6.tar.gz
```

2. `gcc/collect2.c`, line 1537を修正する:

from

```c
redir_handle = open(redir, O_WRONLY | O_TRUNC | O_CREAT);
```

to

```c
redir_handle = open(redir, O_WRONLY | O_TRUNC | O_CREAT, 0755);
```

3. パッチを当てる

```
(in gcc-3.4.6/)
wget http://kozos.jp/books/makeos/patch-gcc-3.4.6-x64-h8300.txt
patch -p0 < patch-gcc-3.4.6-x64-h8300.txt
```

4. ビルドする

```
(in gcc-3.4.6/)
mkdir build
cd build
../configure --target=h8300-elf --disable-nls --disable-threads --disable-shared --enable-languages=c --disable-werror
make
sudo make install
```



### binutils

インストールするだけ (`ld.scr` にはアライメントに関するバグがあるため、適宜修正する:https://groups.google.com/g/kozos_tomonokai/c/HiyFwp-lt6M)

### kz_h8write

1. ダウンロードする。URL: https://ja.osdn.net/projects/kz-h8write/
2. 展開してビルドする。

```
unzip kz_h8write-v0.2.1.zpi
cd PackageFiles/src
make
```

3. makeファイルのパス（`H8WRITE`）を`kz_h8write`のバイナリを指定するように修正する。

