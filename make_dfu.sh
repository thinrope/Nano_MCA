#!sh
echo “Combining .bin files to mca.dfu.”
 ./utilities/dfu.py -b 0x800C000:./GGV2_APP/project/gcc/mca-app.bin \
-b 0x8004000:./GGV2_LIB/project/gcc/mca-lib.bin ggv2.dfu
