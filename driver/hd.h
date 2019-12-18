#ifndef _HD_H

#define HD_CMD_PORT 	0x03f6	
#define HD_DATA_PORT 	0x01f0	
#define HD_ERROR_PORT 	0x01f1
#define HD_NSECTOR_PORT 0x01f2
#define HD_SECTOR_PORT 	0x01f3
#define HD_LCLY_PORT 	0x01f4
#define HD_HCLY_PORT 	0x01f5
#define HD_MODE_DEV_HEAD_PORT 0x01f6
#define HD_STATE_PORT	0x01f7
#define HD_COMMAND_PORT	0x01f7
#define HD_ERR_STATE	0x01	
#define HD_INDEX_STATE	0x02	
#define HD_ECC_STATE	0x04	
#define HD_DRQ_STATE	0x08	
#define HD_SEEK_STATE	0x10
#define HD_WRERR_STATE	0x20
#define HD_READY_STATE	0x40
#define HD_BUSY_STATE	0x80
#define HD_CHS_MODE	0
#define HD_LBA_MODE	1

#define HD_CMD_READ	    0x20
#define HD_CMD_WRITE	0x30

#define FAT16           0x0
#define FAT32           0x1
#define NTFS            0x2
#define Ext3            0x3


/*
typedef struct _bootdata
{
	char JmpStr[3];      //��ת��Ϣ
	char OemStr[8];
	unsigned short BytesBySector; //�����ֽ���
	char SectorbyCu;              //����/��
    unsigned short ReservedSector;//���������� (use)
    char FatNum;                  //fat��Ŀ
	unsigned short Root;          //��Ŀ¼,δʹ��
	unsigned short SectorSmall;   //������
	char Mediades;                //ý������
	unsigned short SecbyFatSmall; //?
	unsigned short SecByTrack;    //�ŵ�.������
	unsigned short Header;        //ͷ��
	unsigned int Hidesec;         //���������� (use)
	unsigned int SectorBig;          //��������(use)
	unsigned int SectorByFat;  //FATռ�������� (use)
	unsigned short Delay;      //�ӳ�
	char Version[2];
	unsigned int RootFirstCu; //��Ŀ¼��һ��
	unsigned short FSInfo;    //fsinfo����
	unsigned short BakSector; //������������
	char NAreserved[12];      //����
	char BiosDrv;             //BIOS����
	char NANoused;            //δ��
	char ExtBoot;             //��չ��������
	char VolumeSN[4];            //�����к�
	char VolumeLabel[11];        //���ǩ
	char FileSys[8];             //�ļ�ϵͳ
}bootdata,*bdata;
/ *  FAT32 �ļ�Ŀ¼�ṹ* /
typedef struct _tagFat32str
{
	char FileName[11];
	char Attr;
	char Reserved[2];
	char Nouse[6];
	unsigned short CuOffsetHigh;
	char Nouse2[4];
	unsigned short CuOffset;
	unsigned int FileSize;
}Fat32str,*fat32s;
*/

extern void hd_init();
//extern inline void port_read(unsigned short port, void* buf,unsigned long nr);
//extern inline void port_write(unsigned short port, void* buf,unsigned long nr);
extern int controller_ready();
extern void hd_read_intr();
extern void hd_write_intr();
extern void  outb(unsigned short port, unsigned char data );
extern char  inb_p(unsigned short port);

#endif
