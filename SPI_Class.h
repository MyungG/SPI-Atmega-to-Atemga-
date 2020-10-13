#define SS   0
#define SCK  1
#define MOSI 2
#define MISO 3
#define SPI_CS PORTB.0

void Init_Master(void)//polling
{
	DDRB = (1 << MOSI) | (1 << SS) | (1 << SCK);
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << CPHA) | (1 << SPR1); // CK = f/64
}

void Init_Slave(void)//polling
{
	DDRB = (1 << MISO); 
	SPCR = (1 << SPE) | (1 << CPHA); // CK = f/64
}


void Init_Master_IntContr(void)
{
    SREG &= ~0x80;  
    
    DDRB = (1 << MOSI) | (1 << SS) | (1 << SCK);
    SPCR = (1 << SPIE) | (1 << SPE) | (1 << MSTR) | (1 << CPHA) | (1 << SPR1); // CK = f/64
    
    SREG |= 0x80;
}

void Init_Slave_Int_Contr(void) //for Slave interrupt SPI rec
{
    SREG &= ~0x80; 
    
    DDRB = (1 << MISO);
    SPCR = (1 << SPIE) | (1 << SPE) | (1 << CPHA);   
    
    SREG |= 0x80;       // Enable Global Interrupt
}

unsigned char Master_Send(unsigned char data)//polling
{
	SPI_CS = 0;
    delay_us(10);   // Slave가 원만한 인식을 위한 지연 시간   
    
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
	SPI_CS = 1;
    
    return SPDR;
}

unsigned char Slave_Receive(void)//polling
{
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
}