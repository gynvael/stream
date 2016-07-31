set disassembly-flavor intel
echo ------------------------------
r sample.bin
where
i r
x/10i $eip
echo ------------------------------
