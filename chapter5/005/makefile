BUILD:=./out
SRC:=.
NINCLUDE:=-I./boot/include
CFLAGS:= -m32
$(BUILD)/boot/%.bin: ./boot/%.S
	nasm $(NINCLUDE) -f bin $< -o $@

$(BUILD)/kernel/kernel.o: ./kernel/kernel.c
	gcc $(CFLAGS) -c $< -o $@ 

$(BUILD)/kernel/kernel.bin: $(BUILD)/kernel/kernel.o
	ld -m elf_i386 $< -Ttext 0xc0001500 -e main -o $@

$(SRC)/master.img: $(BUILD)/boot/mbr.bin $(BUILD)/boot/loader.bin $(BUILD)/kernel/kernel.bin
	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $@
	dd if=$(BUILD)/boot/mbr.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=$(BUILD)/boot/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc
	dd if=$(BUILD)/kernel/kernel.bin of=$@ bs=512 count=200 seek=9 conv=notrunc
	
image: $(SRC)/master.img

.PHONY: clean
clean:
	rm -rf $(SRC)/master.img

.PHONY: bochs
bochs: $(SRC)/master.img
	bochs -q -unlock