OBJDIR = bin-rpi

scansw: scansw.c
	$(CC) -o $@ $^ -lgpiod
	mkdir -p $(OBJDIR)
	mv $@ $(OBJDIR)
