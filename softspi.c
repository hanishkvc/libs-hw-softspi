/* Soft SPI - a library for software gpio based SPI interaction
 * v07May2012_1033
 * HanishKVC
 */

#define U32 unsigned long

/*
 * Define these if you want to call sspi_init to setup
 * the PINMUX for Soft SPI GPIOs
 */

#define PMUX_MODE_GIO
#define PMUX_DIR_IN
#define PMUX_DIR_OUT
#define PMUX_DIR_IO
#define PMUX_PULL_UP
#define PMUX_PULL_DOWN

/*
 * One needs to define these with the GPIO numbers associated with
 * Soft SPI h/w lines
 */
#define GIO_CLK
#define GIO_CS
#define GIO_MISO
#define GIO_MOSI

#define CS_ACTIVE 0
#define CS_INACTIVE 1


#define FUNC_IRQ_SAVE_DISABLE
#define FUNC_IRQ_RESTORE
/* 
 * Define the delay function
 * And define the below mentioned delays as required for the specified delay function
 */

#define FUNC_DELAY usleep
#define DELAY_AFTER_CS 1000
#define DELAY_AFTER_BYTE 1000
#define DELAY_BETWEEN_BITS 500

/* Define the SPI mode as required */
#define SPI_CPOL 0
#define SPI_CPHA 0

/* 
 * define the below GIO functions as required
 * The Soft SPI logic uses these GIO functions
 */
int getgio(int gio);
void setgio(int gio, int val);
void setpinmux(int gio, int flags);

/*
 * Setup soft spi gio/gpios if not already done
 */

void sspi_init()
{
  setpinmux(GIO_CLK, PMUX_MODE_GIO | PMUX_DIR_OUT | PMUX_PULL_UP);
  setpinmux(GIO_CS, PMUX_MODE_GIO | PMUX_DIR_OUT | PMUX_PULL_UP); // Assuming Slave select is Active low else PMUX_PULL_DOWN
  setpinmux(GIO_MOSI, PMUX_MODE_GIO | PMUX_DIR_OUT | PMUX_PULL_UP);
  setpinmux(GIO_MISO, PMUX_MODE_GIO | PMUX_DIR_IN | PMUX_PULL_UP); // Should result in input being 1 when Slave/Spi device is not driving their output
}

/* Full duplex upto 32bit IO, MSb first
 * This does a 32 bit trasfer in both direction parallely/simultaneously
 * So setup the out as required i.e
 * Dummy OutVals, Actual OutVals
 * Dummy OutVals, Actual OutVals, Dummy OutVals
 * Actual OutVals, DummyOutVals
 * Or any other combinations within 32 bits
 */
void sspi_fd_io32(U32 out, U32 *in, int maxbits)
{
  U32 tIn;
  int tInBit;
  int iFlag;

  FUNC_IRQ_SAVE_DISABLE(&iFlag);

  setgio(GIO_CS, CS_ACTIVE); // Select the slave
  FUNC_DELAY(DELAY_AFTER_CS);
  tIn = 0;
  for(i = 0; i < maxbits; i++) {
    if(SPI_CPHA == 0) {

      if(out & 0x80000000)
        setgio(GIO_MOSI, 1);
      else
        setgio(GIO_MOSI, 0);
      FUNC_DELAY(DELAY_BETWEEN_BITS*2);

      if(SPI_CPOL == 0)
        setgio(GIO_CLK, 1);
      else
        setgio(GIO_CLK, 0);
      FUNC_DELAY(DELAY_BETWEEN_BITS);

      tIn <<= 1;
      tInBit = getgio(GIO_MISO);
      if(tInBit) {
        tIn |= 1;
      } else {
        tIn &= 0xFFFFFFFE;
      }
      FUNC_DELAY(DELAY_BETWEEN_BITS);

      if(SPI_CPOL == 0)
        setgio(GIO_CLK, 0);
      else
        setgio(GIO_CLK, 1);
      //FUNC_DELAY(DELAY_BETWEEN_BITS);


    } else {

      if(SPI_CPOL == 0)
        setgio(GIO_CLK, 1);
      else
        setgio(GIO_CLK, 0);

      if(out & 0x80000000)
        setgio(GIO_MOSI, 1);
      else
        setgio(GIO_MOSI, 0);
      FUNC_DELAY(DELAY_BETWEEN_BITS*2);


      if(SPI_CPOL == 0)
        setgio(GIO_CLK, 0);
      else
        setgio(GIO_CLK, 1);
      tIn <<= 1;
      tInBit = getgio(GIO_MISO);
      if(tInBit) {
        tIn |= 1;
      } else {
        tIn &= 0xFFFFFFFE;
      }
      FUNC_DELAY(DELAY_BETWEEN_BITS*2);

    }

    out <<= 1;
    if((i%8 == 0) && (i != 0))
      FUNC_DELAY(DELAY_AFTER_BYTE);
  }
  FUNC_DELAY(DELAY_AFTER_BYTE);
  setgio(GIO_CS, CS_INACTIVE); // DESelect the slave

  *in = tIn;

  FUNC_IRQ_RESTORE(&iFlag);
}

