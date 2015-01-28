# `tmfont` texture mapped fonts

Here is an example run of tmfont

    # generate image and positioning metrics for FreeSans at 10pt
    tmfont --font=../fonts/FreeSans.ttf --pt=10 --out=FreeSans10

    $ ls -l
    total 24
    -rw-r--r--  1 llins  staff  7959 Jan 28 13:01 FreeSans10_tmfont.png
    -rw-r--r--  1 llins  staff  3055 Jan 28 13:01 FreeSans10_tmfont.txt


# Build System

Using Python based build system called WAF: https://code.google.com/p/waf/

Before building you need to download waf.py script:

    wget http://ftp.waf.io/pub/release/waf-1.7.16
    mv waf-1.7.16 waf
    chmod +x waf
    python waf --version

Making the executable

    make all

