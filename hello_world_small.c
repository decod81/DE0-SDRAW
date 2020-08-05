#include "sys/alt_stdio.h"
#include "alt_types.h"
#include "system.h"
#include "altera_avalon_pio_regs.h"

#define SD_CMD_IN				IOWR_ALTERA_AVALON_PIO_DIRECTION(SD_CMD_BASE, ALTERA_AVALON_PIO_DIRECTION_INPUT)
#define SD_CMD_OUT				IOWR_ALTERA_AVALON_PIO_DIRECTION(SD_CMD_BASE, ALTERA_AVALON_PIO_DIRECTION_OUTPUT)
#define SD_DAT_IN				IOWR_ALTERA_AVALON_PIO_DIRECTION(SD_DAT_BASE, ALTERA_AVALON_PIO_DIRECTION_INPUT)
#define SD_DAT_OUT				IOWR_ALTERA_AVALON_PIO_DIRECTION(SD_DAT_BASE, ALTERA_AVALON_PIO_DIRECTION_OUTPUT)
#define SD_CMD_LOW				IOWR_ALTERA_AVALON_PIO_DATA(SD_CMD_BASE, 0)
#define SD_CMD_HIGH				IOWR_ALTERA_AVALON_PIO_DATA(SD_CMD_BASE, 1)
#define SD_CLK_LOW				IOWR_ALTERA_AVALON_PIO_DATA(SD_CLK_BASE, 0)
#define SD_CLK_HIGH				IOWR_ALTERA_AVALON_PIO_DATA(SD_CLK_BASE, 1)
#define SD_DAT_LOW				IOWR_ALTERA_AVALON_PIO_DATA(SD_DAT_BASE, 0)
#define SD_DAT_HIGH				IOWR_ALTERA_AVALON_PIO_DATA(SD_DAT_BASE, 1)
#define SD_TEST_CMD				IORD_ALTERA_AVALON_PIO_DATA(SD_CMD_BASE)
#define SD_TEST_DAT				IORD_ALTERA_AVALON_PIO_DATA(SD_DAT_BASE)

static const alt_u8 crc7_syndrome_table[256] = {
      0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f,
      0x48, 0x41, 0x5a, 0x53, 0x6c, 0x65, 0x7e, 0x77,
      0x19, 0x10, 0x0b, 0x02, 0x3d, 0x34, 0x2f, 0x26,
      0x51, 0x58, 0x43, 0x4a, 0x75, 0x7c, 0x67, 0x6e,
      0x32, 0x3b, 0x20, 0x29, 0x16, 0x1f, 0x04, 0x0d,
      0x7a, 0x73, 0x68, 0x61, 0x5e, 0x57, 0x4c, 0x45,
      0x2b, 0x22, 0x39, 0x30, 0x0f, 0x06, 0x1d, 0x14,
      0x63, 0x6a, 0x71, 0x78, 0x47, 0x4e, 0x55, 0x5c,
      0x64, 0x6d, 0x76, 0x7f, 0x40, 0x49, 0x52, 0x5b,
      0x2c, 0x25, 0x3e, 0x37, 0x08, 0x01, 0x1a, 0x13,
      0x7d, 0x74, 0x6f, 0x66, 0x59, 0x50, 0x4b, 0x42,
      0x35, 0x3c, 0x27, 0x2e, 0x11, 0x18, 0x03, 0x0a,
      0x56, 0x5f, 0x44, 0x4d, 0x72, 0x7b, 0x60, 0x69,
      0x1e, 0x17, 0x0c, 0x05, 0x3a, 0x33, 0x28, 0x21,
      0x4f, 0x46, 0x5d, 0x54, 0x6b, 0x62, 0x79, 0x70,
      0x07, 0x0e, 0x15, 0x1c, 0x23, 0x2a, 0x31, 0x38,
      0x41, 0x48, 0x53, 0x5a, 0x65, 0x6c, 0x77, 0x7e,
      0x09, 0x00, 0x1b, 0x12, 0x2d, 0x24, 0x3f, 0x36,
      0x58, 0x51, 0x4a, 0x43, 0x7c, 0x75, 0x6e, 0x67,
      0x10, 0x19, 0x02, 0x0b, 0x34, 0x3d, 0x26, 0x2f,
      0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c,
      0x3b, 0x32, 0x29, 0x20, 0x1f, 0x16, 0x0d, 0x04,
      0x6a, 0x63, 0x78, 0x71, 0x4e, 0x47, 0x5c, 0x55,
      0x22, 0x2b, 0x30, 0x39, 0x06, 0x0f, 0x14, 0x1d,
      0x25, 0x2c, 0x37, 0x3e, 0x01, 0x08, 0x13, 0x1a,
      0x6d, 0x64, 0x7f, 0x76, 0x49, 0x40, 0x5b, 0x52,
      0x3c, 0x35, 0x2e, 0x27, 0x18, 0x11, 0x0a, 0x03,
      0x74, 0x7d, 0x66, 0x6f, 0x50, 0x59, 0x42, 0x4b,
      0x17, 0x1e, 0x05, 0x0c, 0x33, 0x3a, 0x21, 0x28,
      0x5f, 0x56, 0x4d, 0x44, 0x7b, 0x72, 0x69, 0x60,
      0x0e, 0x07, 0x1c, 0x15, 0x2a, 0x23, 0x38, 0x31,
      0x46, 0x4f, 0x54, 0x5d, 0x62, 0x6b, 0x70, 0x79
};

alt_u8 crc7_byte(alt_u8 crc, alt_u8 data)
{
        return crc7_syndrome_table[(crc << 1) ^ data];
}

alt_u8 crc7(alt_u8 a, alt_u8 b, alt_u8 c, alt_u8 d, alt_u8 e)
{
	alt_u8 crc = 0;
	crc = crc7_byte(crc, a);
	crc = crc7_byte(crc, b);
	crc = crc7_byte(crc, c);
	crc = crc7_byte(crc, d);
	crc = crc7_byte(crc, e);
	return crc;
}

CMD(alt_u8 a, alt_u8 b, alt_u8 c, alt_u8 d, alt_u8 e, alt_u8 f)
{
	alt_u8 sdcmd[6] = {a, b, c, d, e, f}, i, j, k;

    for(i=0;i<6;i++){
        j = 0x80;
        for(k=0;k<8;k++){
            SD_CLK_LOW;
            if (sdcmd[i] & j)
                SD_CMD_HIGH;
            else
                SD_CMD_LOW;
            SD_CLK_HIGH;
            j >>= 1;
        }
    }

	if(a==0x43)
	{
	    SD_CMD_IN;
	    while(SD_TEST_CMD)
	    {
	        SD_CLK_LOW;
	        SD_CLK_HIGH;
	    }
	    for(i=0; i<7; i++)
	    {
	        SD_CLK_LOW;
	        SD_CLK_HIGH;
	    }
	    j = 0;
	    for(i=0; i<8; i++)
	    {
	        SD_CLK_LOW;
	        SD_CLK_HIGH;
	        if(SD_TEST_CMD)
	           j |= 0x80 >> (i & 7);
	    }
	    alt_printf("SD card id: %x", j);
	    j = 0;
	    for(i=0; i<8; i++)
	    {
	        SD_CLK_LOW;
	        SD_CLK_HIGH;
	        if(SD_TEST_CMD)
	           j |= 0x80 >> (i & 7);
	    }
	    alt_printf("%x\n", j);
	    SD_CMD_OUT;
	}
	for(i=0; i<144; i++)
	{
		SD_CLK_LOW;
		SD_CLK_HIGH;
	}
}

int main()
{ 
  int i, j;
  alt_u8 value;

  SD_CMD_OUT;
  SD_DAT_OUT;
  SD_CMD_HIGH;
  SD_DAT_HIGH;
  for(i=0; i<80; i++)
  {
	  SD_CLK_LOW;
	  SD_CLK_HIGH;
  }
  SD_DAT_IN;

  CMD(0x40, 0x00, 0x00, 0x00, 0x00, 0x95);
  CMD(0x48, 0x00, 0x00, 0x01, 0xAA, 0x87);

  CMD(0x77, 0x00, 0x00, 0x00, 0x00, 0x65); usleep(50000);
  CMD(0x69, 0x40, 0xFF, 0x80, 0x00, 0x17); usleep(50000);
  CMD(0x77, 0x00, 0x00, 0x00, 0x00, 0x65); usleep(50000);
  CMD(0x69, 0x40, 0xFF, 0x80, 0x00, 0x17); usleep(50000);

  CMD(0x42, 0x00, 0x00, 0x00, 0x00, 0x4D);
  CMD(0x43, 0x00, 0x00, 0x00, 0x00, 0x21);
  CMD(0x47, 0x59, 0xB4, 0x00, 0x00, (crc7(0x47, 0x59, 0xB4, 0x00, 0x00) << 1)|1);
  CMD(0x51, 0x00, 0x00, 0x00, 0x00, (crc7(0x51, 0x00, 0x00, 0x00, 0x00) << 1)|1);

  alt_printf("\n");

  while(1) {
	  SD_CLK_LOW;
	  SD_CLK_HIGH;
	  if((SD_TEST_DAT & 1) == 0)
		  break;
  }
  for(j=0; j<512; j++)
  {
	  for(i=0; i<8; i++)
	  {
		  SD_CLK_LOW;
		  SD_CLK_HIGH;
		  value <<= 1;
		  if(SD_TEST_DAT & 1)
			  value |= 1;
	  }
	  alt_printf("%x ", value);
  }
  for(i=0; i<24; i++){
      SD_CLK_LOW;
      SD_CLK_HIGH;
  }

  alt_printf("\n\n");

  /* READ BLOCK 63 */
  CMD(0x51, 0x00, 0x00, 0x00, 63, (crc7(0x51, 0x00, 0x00, 0x00, 63) << 1)|1);

  while(1) {
	  SD_CLK_LOW;
	  SD_CLK_HIGH;
	  if((SD_TEST_DAT & 1) == 0)
		  break;
  }
  for(j=0; j<512; j++)
  {
	  for(i=0; i<8; i++)
	  {
		  SD_CLK_LOW;
		  SD_CLK_HIGH;
		  value <<= 1;
		  if(SD_TEST_DAT & 1)
			  value |= 1;
	  }
	  alt_printf("%x ", value);
  }
  for(i=0; i<24; i++){
      SD_CLK_LOW;
      SD_CLK_HIGH;
  }

  while(1);

  return 0;
}
