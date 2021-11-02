# invoke SourceDir generated makefile for empty.pe430X
empty.pe430X: .libraries,empty.pe430X
.libraries,empty.pe430X: package/cfg/empty_pe430X.xdl
	$(MAKE) -f C:\TI_RTOS\RT_FinalProject\RT_FinalProject1\finalProjectTest/src/makefile.libs

clean::
	$(MAKE) -f C:\TI_RTOS\RT_FinalProject\RT_FinalProject1\finalProjectTest/src/makefile.libs clean

