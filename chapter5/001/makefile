SRC:=.
INCLUDE:=-I$(SRC)/include

$(SRC)/out/%.bin: $(SRC)/%.S
	nasm $(INCLUDE) -f bin $< -o $@

$(SRC)/master.img: ./out/mbr.bin ./out/loader.bin
	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $@
	dd if=./out/mbr.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=./out/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc

image: $(SRC)/master.img

.PHONY: clean
clean:
	rm -rf $(SRC)/master.img

.PHONY: bochs
bochs: $(SRC)/master.img
	bochs -q -unlock