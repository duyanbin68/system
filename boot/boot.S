/* FAT12 文件头 */
.code16
.set BaseOfStack,0x7c00/* Stack base address, inner */
.set BaseOfLoader,0x9000/* Section loading address of LOADER.BIN */
.set OffsetOfLoader, 0x0000/* Loading offset of LOADER.BIN */
.set RootDirSectors, 14/* Root directory sector count */
.set SecNoOfRootDir, 19/* 1st sector of root directory */
.set SecNoOfFAT1,1/* 1st sector of FAT1 */
.set DeltaSecNo,17/* BPB_(RsvdSecCnt+NumFATs*FATSz) -2 */
/* Start sector of file space =*/
.text
	jmp LABEL_START /* 跳转指令 */
	nop /* 这个nop指令是可选的，如果你用的是e9指令（长跳转），这个是不需要的
		但是如果你用的eb指令，说明你只是指明的跳转的步长，nop是必需的 */
	BS_OEMName: 		.ascii "MSWIN4.1" /* OEM生成厂商的名字，微软建议使用MSWIN4.1保持兼容行 */
	BPB_BytsPerSec:  	.2byte 512	  /*对面现代磁盘来说，这个数可能是4096，但是软盘这个是固定的*/
	BPB_SecPerCluse: 	.byte  1	  /*每簇的扇区数，可以取2的n次方*/
	BPB_ResvdSecCnt:	.2byte 1	  /*保留扇区数，必须为1*/
	BPB_NumFATs:		.byte  2	  /*FAT的数目，一般是2*/
	BPB_RootEntCnt:		.2byte 224	  /*根目录可容纳的条目，FAT32此项为0*/
	BPB_TotSec16:		.2byte 2880	  /*16bit扇区总数*/
	BPB_Media:		.byte  0xf0	  /*媒体介质类型，f0表示可移动磁盘*/
	BPB_FATSz16:		.2byte 9	  /*FAT表的大小，9个扇区*/
	BPB_SecPerTrk:		.2byte 18	  /*每磁道的扇区数 18 */
	BPB_NumHeads:		.2byte 2	  /*磁头数，对于1.44M磁盘来说，这个数固定为2*/
	BPB_HiddSec:		.4byte 0	  /*FAT分区之前的隐藏扇区数，没有分区的磁盘来说此项为0*/
	BPB_TotSec32:		.4byte 0	  /*总扇区数，和tolsec16含义相同，16位无法表示可用此项*/
	BS_DrvNum:		.byte  0	  /*用于0x13中断，0为软盘，0x80为硬盘*/
	BS_Reserved1:		.byte  0	  /*保留NT使用*/
	BS_BootSig:		.byte  0x29	  /*扩展引导标记，表明此后的3个域可用*/
	BS_VolID:		.4byte 0	  /*卷标序列号*/
	BS_VolLab:		.ascii "Solrex 0.01" /*卷标*/
	BS_FileSysType:		.ascii "FAT12  "	/*文件类型，只是文本字符串，不可用来检测文件系统类型*/
	/* Initial registers.初始化寄存器 */
LABEL_START:
	mov %cs,%ax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%ss
	mov $BaseOfStack, %sp
	/*清屏*/
	mov $3,%ax
	int $0x10	

	/*显示引导信息*/
	mov $BootMessage,%bp
	call Dispstr
	
	/*重置软盘*/	
	xor %ah,%ah
	xor %dl,%dl
	int $0x13
	/*加载根目录*/	
	call LoadRoot

	mov $LoaderFileName,%si
	mov $BaseOfLoader,%bx
	mov $OffsetOfLoader,%bp
	call LoadFile
	
	
LABEL_FILE_LOADED:
	jmp $BaseOfLoader,$OffsetOfLoader
	/*End 到此结束，下面是子函数,以及一些变量*/

/* ==================================================================
String table
*/
LoaderFileName: 	.ascii "LOADER  BIN" /* File name */
.set MessageLength,9
BootMessage: 		.ascii "Booting**" /* index 0 */
/* ==================================================================
Routine: DispStr
Action: Display a string, string index stored in %dh
*/
Dispstr:
 mov $MessageLength,%cx
 mov $0x1301,%ax 
 mov $0x07,%bx
 mov $0,%dl 
 int $0x10
 ret

.include "fat12.s"
.org 510 /* Skip to address 0x510. */
.2byte 0xaa55 /* Write boot flag to 1st sector(512 bytes) end */
