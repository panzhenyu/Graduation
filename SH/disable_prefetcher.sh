# cd /usr/src/msr-tools-1.1.2/
sudo modprobe msr
sudo wrmsr -p 0 0x1a4 0xf
sudo wrmsr -p 1 0x1a4 0xf
sudo wrmsr -p 2 0x1a4 0xf
sudo wrmsr -p 3 0x1a4 0xf
# sudo wrmsr -p 4 0x1a4 0xf
# sudo wrmsr -p 5 0x1a4 0xf
# sudo wrmsr -p 6 0x1a4 0xf
# sudo wrmsr -p 7 0x1a4 0xf

