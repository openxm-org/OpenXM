INST_DIR=../../../../lib/asir/bin
all:
install: all
	@echo "Installing plugin scripts for os_muldif.rr."
	mkdir -p ${INST_DIR}
	cp src/risatex.sh ${INST_DIR}
	cp src/risaout.tex ${INST_DIR}

test:
	asir -f  assert/test1.rr
todo:
	echo "Folder does not follow the standard hierarchy. It should follow the standard in a future release."

# $OpenXM$
