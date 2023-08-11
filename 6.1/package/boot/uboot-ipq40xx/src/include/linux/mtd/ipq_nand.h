#ifndef IPQ806X_NAND_H
#define IPQ806X_NAND_H

enum ipq_nand_layout {
	IPQ_NAND_LAYOUT_SBL,
	IPQ_NAND_LAYOUT_LINUX,

	IPQ_NAND_LAYOUT_MAX
};

int ipq_nand_init(enum ipq_nand_layout layout, int variant);

#endif
