SECTIONS
{
	. = 0x0;
	.text : {
		*(.text)
	}
	. = 0x3000;
	.data : {
		*(.data)
	}
}
