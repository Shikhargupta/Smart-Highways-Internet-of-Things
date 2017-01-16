#include <SPI.h>

#define  uchar unsigned char
#define uint  unsigned int

//data array maxium length
#define MAX_LEN 16

/////////////////////////////////////////////////////////////////////
//set the pin
/////////////////////////////////////////////////////////////////////
const int chipSelectPin = 10;
const int NRSTPD = 5;

//MF522 command bits
#define PCD_IDLE              0x00               //NO action; cancel current commands
#define PCD_AUTHENT           0x0E               //verify password key
#define PCD_RECEIVE           0x08               //receive data
#define PCD_TRANSMIT          0x04               //send data
#define PCD_TRANSCEIVE        0x0C               //send and receive data
#define PCD_RESETPHASE        0x0F               //reset
#define PCD_CALCCRC           0x03               //CRC check and caculation

//Mifare_One card command bits
#define PICC_REQIDL           0x26               //Search the cards that not into sleep mode in the antenna area 
#define PICC_REQALL           0x52               //Search all the cards in the antenna area
#define PICC_ANTICOLL         0x93               //prevent conflict
#define PICC_SElECTTAG        0x93               //select card
#define PICC_AUTHENT1A        0x60               //verify A password key
#define PICC_AUTHENT1B        0x61               //verify B password key
#define PICC_READ             0x30               //read 
#define PICC_WRITE            0xA0               //write
#define PICC_DECREMENT        0xC0               //deduct value
#define PICC_INCREMENT        0xC1               //charge up value
#define PICC_RESTORE          0xC2               //Restore data into buffer
#define PICC_TRANSFER         0xB0               //Save data into buffer
#define PICC_HALT             0x50               //sleep mode


//THe mistake code that return when communicate with MF522
#define MI_OK                 0
#define MI_NOTAGERR           1
#define MI_ERR                2


//------------------MFRC522 register ---------------
//Page 0:Command and Status
#define     Reserved00            0x00    
#define     CommandReg            0x01    
#define     CommIEnReg            0x02    
#define     DivlEnReg             0x03    
#define     CommIrqReg            0x04    
#define     DivIrqReg             0x05
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08    
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     Reserved01            0x0F
//Page 1:Command     
#define     Reserved10            0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     Reserved11            0x1A
#define     Reserved12            0x1B
#define     MifareReg             0x1C
#define     Reserved13            0x1D
#define     Reserved14            0x1E
#define     SerialSpeedReg        0x1F
//Page 2:CFG    
#define     Reserved20            0x20  
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     Reserved21            0x23
#define     ModWidthReg           0x24
#define     Reserved22            0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsPReg            0x28
#define     ModGsPReg             0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
//Page 3:TestRegister     
#define     Reserved30            0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     Reserved31            0x3C   
#define     Reserved32            0x3D   
#define     Reserved33            0x3E   
#define     Reserved34        0x3F
//-----------------------------------------------

//4 bytes Serial number of card, the 5 bytes is verfiy bytes
uchar serNum[5];

uchar  writeData[16]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100};  //initialize to 100 USD
uchar  moneyConsume = 18 ;  //Deduct 18 USD
uchar  moneyAdd = 10 ;  //Charge up 10 USD
//buffer A password, 16 buffer, the passowrd of every buffer is 6 byte 
 uchar sectorKeyA[16][16] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             //{0x19, 0x84, 0x07, 0x15, 0x76, 0x14},
                             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                            };
 uchar sectorNewKeyA[16][16] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff,0x07,0x80,0x69, 0x19,0x84,0x07,0x15,0x76,0x14},
                                 //you can set another ket , such as  " 0x19, 0x84, 0x07, 0x15, 0x76, 0x14 "
                                 //{0x19, 0x84, 0x07, 0x15, 0x76, 0x14, 0xff,0x07,0x80,0x69, 0x19,0x84,0x07,0x15,0x76,0x14},
                                 // but when loop, please set the  sectorKeyA, the same key, so that RFID module can read the card
                                {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff,0x07,0x80,0x69, 0x19,0x33,0x07,0x15,0x34,0x14},
                               };



	int ar[5];
	ar[0]=220;
	ar[1]=220;
	ar[2]=220;
	ar[3]=220;
	ar[4]=220;
	int net[5];
	fr(int i=0;i<5;++i){
		net[i]=50000;
	}
void setup() {                
   Serial.begin(9600);                       // RFID reader SOUT pin connected to Serial RX pin at 2400bps 
 // start the SPI library:
  SPI.begin();
  pinMode(chipSelectPin,OUTPUT);             // Set digital pin 10 as OUTPUT to connect it to the RFID /ENABLE pin 
    digitalWrite(chipSelectPin, LOW);          // Activate the RFID reader
  pinMode(NRSTPD,OUTPUT);               // Set digital pin 10 , Not Reset and Power-down
    digitalWrite(NRSTPD, HIGH);

  MFRC522_Init();  
}

void loop()
{
    uchar i,tmp;
  uchar status;
  static int x;
        uchar str[MAX_LEN];
        uchar RC_size;
        uchar blockAddr;  //Select operation buck address  0 - 63
        String mynum = "";


    //Search card, return card types
    status = MFRC522_Request(PICC_REQIDL, str); 
    if (status == MI_OK)
    {
                        Serial.println("Card detected");
      // Serial.print(str[0],BIN);
                        // Serial.print(" , ");
      // Serial.print(str[1],BIN);
                        //Serial.println(" ");
    }

    //Prevent conflict, return the 4 bytes Serial number of the card
    status = MFRC522_Anticoll(str);
    memcpy(serNum, str, 5);
    if (status == MI_OK)
    {
			int v=SerNum[0]; 
                        Serial.println("The card's number is  : ");
      Serial.print(serNum[0]);
      //                   Serial.print(" , ");
      Serial.print(serNum[1],DEC);
      //                  Serial.print(" , ");
      Serial.print(serNum[2],DEC);
      //                  Serial.print(" , ");
      Serial.print(serNum[3],DEC);
      //                  Serial.print(" , ");
      Serial.print(serNum[4],DEC);
      Serial.println("\n");
		
	  if(v==220){
			Serial.println("Suspicious account detected.DO NOT OPEN GATE");			
	  }
		
	  if(v!=x){
			Serial.println("Amount deducted from account.OPEN GATE");
			if(v==ab){
				net[0]-=500;
			}
			
			if(v==ab){
				net[1]-=500;
			}
			
			if(v==ab){
				net[2]-=500;
			}

			if(v==ab){
				net[3]-=500;
			}
	  }
                        // Should really check all pairs, but for now we'll just use the first
                     
                        
    }
                //Serial.println(" ");
    MFRC522_Halt();     //command the card into sleep mode             
          
}

/*
 * Functionï¼šWrite_MFRC5200
 * Descriptionï¼šwrite a byte data into one register of MR RC522
 * Input parameterï¼šaddr--register addressï¼›val--the value that need to write in
 * Returnï¼šNull
 */
void Write_MFRC522(uchar addr, uchar val)
{
  digitalWrite(chipSelectPin, LOW);

  //address formatï¼š0XXXXXX0
  SPI.transfer((addr<<1)&0x7E); 
  SPI.transfer(val);
  
  digitalWrite(chipSelectPin, HIGH);
}


/*
 * Functionï¼šRead_MFRC522
 * Descriptionï¼šread a byte data into one register of MR RC522
 * Input parameterï¼šaddr--register address
 * Returnï¼šreturn the read value
 */
uchar Read_MFRC522(uchar addr)
{
  uchar val;

  digitalWrite(chipSelectPin, LOW);

  //address formatï¼š1XXXXXX0
  SPI.transfer(((addr<<1)&0x7E) | 0x80);  
  val =SPI.transfer(0x00);
  
  digitalWrite(chipSelectPin, HIGH);
  
  return val; 
}

/*
 * Functionï¼šSetBitMask
 * Descriptionï¼šset RC522 register bit
 * Input parameterï¼šreg--register address;mask--value
 * Returnï¼šnull
 */
void SetBitMask(uchar reg, uchar mask)  
{
    uchar tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp | mask);  // set bit mask
}


/*
 * Functionï¼šClearBitMask
 * Descriptionï¼šclear RC522 register bit
 * Input parameterï¼šreg--register address;mask--value
 * Returnï¼šnull
 */
void ClearBitMask(uchar reg, uchar mask)  
{
    uchar tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp & (~mask));  // clear bit mask
} 


/*
 * Functionï¼šAntennaOn
 * Descriptionï¼šTurn on antenna, every time turn on or shut down antenna need at least 1ms delay
 * Input parameterï¼šnull
 * Returnï¼šnull
 */
void AntennaOn(void)
{
  uchar temp;

  temp = Read_MFRC522(TxControlReg);
  if (!(temp & 0x03))
  {
    SetBitMask(TxControlReg, 0x03);
  }
}


/*
 * Functionï¼šAntennaOff
 * Descriptionï¼šTurn off antenna, every time turn on or shut down antenna need at least 1ms delay
 * Input parameterï¼šnull
 * Returnï¼šnull
 */
void AntennaOff(void)
{
  ClearBitMask(TxControlReg, 0x03);
}


/*
 * Functionï¼šResetMFRC522
 * Descriptionï¼š reset RC522
 * Input parameterï¼šnull
 * Returnï¼šnull
 */
void MFRC522_Reset(void)
{
    Write_MFRC522(CommandReg, PCD_RESETPHASE);
}


/*
 * Functionï¼šInitMFRC522
 * Descriptionï¼šinitilize RC522
 * Input parameterï¼šnull
 * Returnï¼šnull
 */
void MFRC522_Init(void)
{
  digitalWrite(NRSTPD,HIGH);

  MFRC522_Reset();
    
  //Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
    Write_MFRC522(TModeReg, 0x8D);    //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    Write_MFRC522(TPrescalerReg, 0x3E); //TModeReg[3..0] + TPrescalerReg
    Write_MFRC522(TReloadRegL, 30);           
    Write_MFRC522(TReloadRegH, 0);
  
  Write_MFRC522(TxAutoReg, 0x40);   //100%ASK
  Write_MFRC522(ModeReg, 0x3D);   //CRC initilizate value 0x6363  ???

  //ClearBitMask(Status2Reg, 0x08);   //MFCrypto1On=0
  //Write_MFRC522(RxSelReg, 0x86);    //RxWait = RxSelReg[5..0]
  //Write_MFRC522(RFCfgReg, 0x7F);      //RxGain = 48dB

  AntennaOn();    //turn on antenna
}


/*
 * Functionï¼šMFRC522_Request
 * Descriptionï¼šSearching card, read card type
 * Input parameterï¼šreqMode--search methodsï¼Œ
 *       TagType--return card types
 *        0x4400 = Mifare_UltraLight
 *        0x0400 = Mifare_One(S50)
 *        0x0200 = Mifare_One(S70)
 *        0x0800 = Mifare_Pro(X)
 *        0x4403 = Mifare_DESFire
 * returnï¼šreturn MI_OK if successed
 */
uchar MFRC522_Request(uchar reqMode, uchar *TagType)
{
  uchar status;  
  uint backBits;      //the data bits that received

  Write_MFRC522(BitFramingReg, 0x07);   //TxLastBists = BitFramingReg[2..0] ???
  
  TagType[0] = reqMode;
  status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

  if ((status != MI_OK) || (backBits != 0x10))
  {    
    status = MI_ERR;
  }
   
  return status;
}


/*
 * Functionï¼šMFRC522_ToCard
 * Descriptionï¼šcommunicate between RC522 and ISO14443
 * Input parameterï¼šcommand--MF522 command bits
 *       sendData--send data to card via rc522
 *       sendLen--send data length     
 *       backData--the return data from card
 *       backLen--the length of return data
 * returnï¼šreturn MI_OK if successed
 */
uchar MFRC522_ToCard(uchar command, uchar *sendData, uchar sendLen, uchar *backData, uint *backLen)
{
    uchar status = MI_ERR;
    uchar irqEn = 0x00;
    uchar waitIRq = 0x00;
    uchar lastBits;
    uchar n;
    uint i;

    switch (command)
    {
        case PCD_AUTHENT:   //verify card password
    {
      irqEn = 0x12;
      waitIRq = 0x10;
      break;
    }
    case PCD_TRANSCEIVE:  //send data in the FIFO
    {
      irqEn = 0x77;
      waitIRq = 0x30;
      break;
    }
    default:
      break;
    }
   
    Write_MFRC522(CommIEnReg, irqEn|0x80);  //Allow interruption
    ClearBitMask(CommIrqReg, 0x80);     //Clear all the interrupt bits
    SetBitMask(FIFOLevelReg, 0x80);     //FlushBuffer=1, FIFO initilizate
    
  Write_MFRC522(CommandReg, PCD_IDLE);  //NO action;cancel current command  ???

  //write data into FIFO
    for (i=0; i<sendLen; i++)
    {   
    Write_MFRC522(FIFODataReg, sendData[i]);    
  }

  //procceed it
  Write_MFRC522(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {    
    SetBitMask(BitFramingReg, 0x80);    //StartSend=1,transmission of data starts  
  }   
    
  //waite receive data is finished
  i = 2000; //i should adjust according the clock, the maxium the waiting time should be 25 ms???
    do 
    {
    //CommIrqReg[7..0]
    //Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = Read_MFRC522(CommIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq));

    ClearBitMask(BitFramingReg, 0x80);      //StartSend=0
  
    if (i != 0)
    {    
        if(!(Read_MFRC522(ErrorReg) & 0x1B))  //BufferOvfl Collerr CRCErr ProtecolErr
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {   
        status = MI_NOTAGERR;     //??   
      }

            if (command == PCD_TRANSCEIVE)
            {
                n = Read_MFRC522(FIFOLevelReg);
                lastBits = Read_MFRC522(ControlReg) & 0x07;
                if (lastBits)
                {   
          *backLen = (n-1)*8 + lastBits;   
        }
                else
                {   
          *backLen = n*8;   
        }

                if (n == 0)
                {   
          n = 1;    
        }
                if (n > MAX_LEN)
                {   
          n = MAX_LEN;   
        }
        
        //read the data from FIFO
                for (i=0; i<n; i++)
                {   
          backData[i] = Read_MFRC522(FIFODataReg);    
        }
            }
        }
        else
        {   
      status = MI_ERR;  
    }
        
    }
  
    //SetBitMask(ControlReg,0x80);           //timer stops
    //Write_MFRC522(CommandReg, PCD_IDLE); 

    return status;
}


/*
 * Functionï¼šMFRC522_Anticoll
 * Descriptionï¼šPrevent conflict, read the card serial number 
 * Input parameterï¼šserNum--return the 4 bytes card serial number, the 5th byte is recheck byte
 * returnï¼šreturn MI_OK if successed
 */
uchar MFRC522_Anticoll(uchar *serNum)
{
    uchar status;
    uchar i;
  uchar serNumCheck=0;
    uint unLen;
    

    //ClearBitMask(Status2Reg, 0x08);   //TempSensclear
    //ClearBitMask(CollReg,0x80);     //ValuesAfterColl
  Write_MFRC522(BitFramingReg, 0x00);   //TxLastBists = BitFramingReg[2..0]
 
    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MI_OK)
  {
    //Verify card serial number
    for (i=0; i<4; i++)
    {   
      serNumCheck ^= serNum[i];
    }
    if (serNumCheck != serNum[i])
    {   
      status = MI_ERR;    
    }
    }

    //SetBitMask(CollReg, 0x80);    //ValuesAfterColl=1

    return status;
} 


/*
 * Functionï¼šCalulateCRC
 * Descriptionï¼šUse MF522 to caculate CRC
 * Input parameterï¼špIndata--the CRC data need to be readï¼Œlen--data lengthï¼ŒpOutData-- the caculated result of CRC
 * returnï¼šNull
 */
void CalulateCRC(uchar *pIndata, uchar len, uchar *pOutData)
{
    uchar i, n;

    ClearBitMask(DivIrqReg, 0x04);      //CRCIrq = 0
    SetBitMask(FIFOLevelReg, 0x80);     //Clear FIFO pointer
    //Write_MFRC522(CommandReg, PCD_IDLE);

  //Write data into FIFO  
    for (i=0; i<len; i++)
    {   
    Write_MFRC522(FIFODataReg, *(pIndata+i));   
  }
    Write_MFRC522(CommandReg, PCD_CALCCRC);

  //waite CRC caculation to finish
    i = 0xFF;
    do 
    {
        n = Read_MFRC522(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));      //CRCIrq = 1

  //read CRC caculation result
    pOutData[0] = Read_MFRC522(CRCResultRegL);
    pOutData[1] = Read_MFRC522(CRCResultRegM);
}


/*
 * Functionï¼šMFRC522_SelectTag
 * Descriptionï¼šSelect card, read card storage volume
 * Input parameterï¼šserNum--Send card serial number
 * returnï¼šreturn the card storage volume
 */
uchar MFRC522_SelectTag(uchar *serNum)
{
    uchar i;
  uchar status;
  uchar size;
    uint recvBits;
    uchar buffer[9]; 

  //ClearBitMask(Status2Reg, 0x08);     //MFCrypto1On=0

    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70;
    for (i=0; i<5; i++)
    {
      buffer[i+2] = *(serNum+i);
    }
  CalulateCRC(buffer, 7, &buffer[7]);   //??
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
    
    if ((status == MI_OK) && (recvBits == 0x18))
    {   
    size = buffer[0]; 
  }
    else
    {   
    size = 0;    
  }

    return size;
}


/*
 * Functionï¼šMFRC522_Auth
 * Descriptionï¼šverify card password
 * Input parametersï¼šauthMode--password verify mode
                 0x60 = verify A passowrd key 
                 0x61 = verify B passowrd key 
             BlockAddr--Block address
             Sectorkey--Block password
             serNum--Card serial number ï¼Œ4 bytes
 * returnï¼šreturn MI_OK if successed
 */
uchar MFRC522_Auth(uchar authMode, uchar BlockAddr, uchar *Sectorkey, uchar *serNum)
{
    uchar status;
    uint recvBits;
    uchar i;
  uchar buff[12]; 

  //Verify command + block address + buffer password + card SN
    buff[0] = authMode;
    buff[1] = BlockAddr;
    for (i=0; i<6; i++)
    {    
    buff[i+2] = *(Sectorkey+i);   
  }
    for (i=0; i<4; i++)
    {    
    buff[i+8] = *(serNum+i);   
  }
    status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

    if ((status != MI_OK) || (!(Read_MFRC522(Status2Reg) & 0x08)))
    {   
    status = MI_ERR;   
  }
    
    return status;
}


/*
 * Functionï¼šMFRC522_Read
 * Descriptionï¼šRead data 
 * Input parametersï¼šblockAddr--block address;recvData--the block data which are read
 * returnï¼šreturn MI_OK if successed
 */
uchar MFRC522_Read(uchar blockAddr, uchar *recvData)
{
    uchar status;
    uint unLen;

    recvData[0] = PICC_READ;
    recvData[1] = blockAddr;
    CalulateCRC(recvData,2, &recvData[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

    if ((status != MI_OK) || (unLen != 0x90))
    {
        status = MI_ERR;
    }
    
    return status;
}


/*
 * Functionï¼šMFRC522_Write
 * Descriptionï¼šwrite block data
 * Input parametersï¼šblockAddr--block address;writeData--Write 16 bytes data into block
 * returnï¼šreturn MI_OK if successed
 */
uchar MFRC522_Write(uchar blockAddr, uchar *writeData)
{
    uchar status;
    uint recvBits;
    uchar i;
  uchar buff[18]; 
    
    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    CalulateCRC(buff, 2, &buff[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
    {   
    status = MI_ERR;   
  }
        
    if (status == MI_OK)
    {
        for (i=0; i<16; i++)    //Write 16 bytes data into FIFO
        {    
          buff[i] = *(writeData+i);   
        }
        CalulateCRC(buff, 16, &buff[16]);
        status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);
        
    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
        {   
      status = MI_ERR;   
    }
    }
    
    return status;
}


/*
 * Functionï¼šMFRC522_Halt
 * Descriptionï¼šCommand the cards into sleep mode
 * Input parametersï¼šnull
 * returnï¼šnull
 */
void MFRC522_Halt(void)
{
  uchar status;
    uint unLen;
    uchar buff[4]; 

    buff[0] = PICC_HALT;
    buff[1] = 0;
    CalulateCRC(buff, 2, &buff[2]);
 
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}
