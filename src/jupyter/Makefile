# $OpenXM: OpenXM/src/jupyter/Makefile,v 1.3 2019/05/28 00:26:23 takayama Exp $
usage:
	@echo "install-debian-for-debug clean-debian"
	@echo "install-mac-anaconda2-for-debug"
	@echo "put-to-ox-dir  get-from-ox-dir"
	@echo "  "
	@echo "On Ubuntu20.04/wsl2, do apt install jupyter-notebook and pip3 install metakernel. Start with /usr/bin/jupyter notebook"

#Debian_ASIR_KERNEL_DIR=${HOME}/.local/lib/python2.7/site-packages/asir_kernel
Debian_ASIR_KERNEL_DIR=${HOME}/.local/lib/python3.8/site-packages/asir_kernel
Debian_ASIR_KERNEL_INFO=${HOME}/.local/share/jupyter/kernels/asir
install-debian-for-debug:
	-mkdir ${HOME}/bin
	-ln -s  `pwd`/asir-cli ${HOME}/bin
	-mkdir -p ${Debian_ASIR_KERNEL_DIR}
	-mkdir -p ${Debian_ASIR_KERNEL_INFO}
	-ln -s `pwd`/kernel.py ${Debian_ASIR_KERNEL_DIR}/kernel.py
	-ln -s `pwd`/kernel.json ${Debian_ASIR_KERNEL_DIR}/kernel.json
	-ln -s `pwd`/__init__.py ${Debian_ASIR_KERNEL_DIR}/__init__.py
	-ln -s `pwd`/__main__.py ${Debian_ASIR_KERNEL_DIR}/__main__.py
	-ln -s `pwd`/kernel.json ${Debian_ASIR_KERNEL_INFO}/kernel.json
	-@echo "${HOME}/bin should be in the command search path. 'which asir-cli' returns asir-cli? "
clean-debian:
	rm -rf ${Debian_ASIR_KERNEL_DIR} ${Debian_ASIR_KERNEL_INFO} ${HOME}/bin/asir-cli

OX_DIR=${OpenXM_HOME}/src/jupyter
ASIR_KERNEL_FILES=kernel.py kernel.json __init__.py __main__.py asir-cli
put-to-ox-dir:
	-mkdir -p ${OX_DIR}
	-for i in ${ASIR_KERNEL_FILES}; do rsync -avzu $$i ${OX_DIR} ; done
	-rsync -avzu  Makefile ${OX_DIR}
	@echo "pushd ${OX_DIR} and commit; and  popd and make get-from-ox-dir and commit to change tags."
get-from-ox-dir:
	-for i in ${ASIR_KERNEL_FILES}; do rsync -avzu ${OX_DIR}/$$i . ; done

ANACONDA2_ASIR_KERNEL_DIR=/anaconda2/lib/python2.7/site-packages/asir_kernel
ANACONDA2_ASIR_KERNEL_INFO=/anaconda2/share/jupyter/kernels/asir
install-mac-anaconda2:
	-mkdir ${HOME}/bin
	-ln -s  `pwd`/asir-cli ${HOME}/bin
	-mkdir -p ${ANACONDA2_ASIR_KERNEL_DIR}
	-mkdir -p ${ANACONDA2_ASIR_KERNEL_INFO}
	-ln -s `pwd`/kernel.py ${ANACONDA2_ASIR_KERNEL_DIR}/kernel.py
	-ln -s `pwd`/kernel.json ${ANACONDA2_ASIR_KERNEL_DIR}/kernel.json
	-ln -s `pwd`/__init__.py ${ANACONDA2_ASIR_KERNEL_DIR}/__init__.py
	-ln -s `pwd`/__main__.py ${ANACONDA2_ASIR_KERNEL_DIR}/__main__.py
	-ln -s `pwd`/kernel.json ${ANACONDA2_ASIR_KERNEL_INFO}/kernel.json
	-@echo "${HOME}/bin should be in the command search path. 'which asir-cli' returns asir-cli? "

clean-mac-anaconda2:
	rm -rf ${ANACONDA2_ASIR_KERNEL_DIR} ${ANACONDA2_ASIR_KERNEL_INFO} ${HOME}/bin/asir-cli




