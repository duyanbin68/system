/*
;*******************************************************
;
;	Floppy12.inc
;		Floppy drive interface routines
;
;	OS Development Series
;*******************************************************
*/
.set ROOT_SEG,0x8000
.set FAT_SEG,0x7000
.set ROOT_OFFSET,0x0
/*转换Cu到LBA*/
ClusterLBA:
	sub    $0x2,%ax 		/*有２簇被文件系统占用，减去*/
	xor    %cx,%cx  		# cx = 0
	movb    (BPB_SecPerCluse),%cl	# cl = 1
	mul    %cx			/* 乘以每簇的扇区数*/
	addw   (datasector),%ax 	/* + 数据起始扇区*/
	ret
/*从LBA到CHS,ax => LBA的地址转换*/
LBACHS:
	xor    %dx,%dx
	divw   (BPB_SecPerTrk)		#ax / 18
	inc    %dl			# dl + 1
	movb    %dl,(absoluteSector) 	# dl = ab..sec
	xor    %dx,%dx			
	divw   (BPB_NumHeads)		
	mov    %dl,(absoluteHead)
	mov    %al,(absoluteTrack)
	ret
/*
	ReadSectors读取1个扇区函数
	cx 读取扇区数量
	ax 起始扇区
	es:bx 读取到的内存地址
*/
ReadSectors:
	mov    $0x5,%di  /*5次重试的机会*/	
SECTORLOOP:
	push   %ax
 	push   %bx
	push   %cx
	call   LBACHS /*转换扇区*/
	mov    $0x2,%ah	/*读取扇区功能号为ah = 2*/
	mov    $0x1,%al /*读取1个扇区*/
	mov    (absoluteTrack),%ch /**/
	mov    (absoluteSector),%cl
	mov    (absoluteHead),%dh
	mov    $0,%dl
	int    $0x13
	jnc    SUCCESS
	/*读取错误重试*/
	xor    %ax,%ax 
	int    $0x13
	dec    %di
	pop    %cx
	pop    %bx
	pop    %ax
	jnz    SECTORLOOP
 	int    $0x18
SUCCESS:
	pop    %cx
	pop    %bx
 	pop    %ax
	inc	%ax
	movw %es,(loadsegc)    #es -> loadseg
	addw $0x20,(loadsegc)	# loadseg + 0x20
	pushw (loadsegc)	# loadseg -> es
	pop %es
	loop   ReadSectors
	ret    
/*加载根目录到内存地址ROOT_SEG*/
LoadRoot:
	pusha  
 	push   %es
	/*计算根目录大小*/
  	mov    $2,%al /*FAT数量2个*/
  	mulw   (BPB_FATSz16)	/**/
  	add    (BPB_ResvdSecCnt),%ax
 	mov 	$2,%cx
 	pushw  $ROOT_SEG
  	pop    %es
  	mov    $0x0,%bx
  	call   ReadSectors
  	pop    %es
  	popa   
 	ret    
/*加载FAT到es：di*/
LoadFAT:
	pusha  
 	push   %es
 	xor    %ax,%ax
  	mov    (BPB_NumFATs),%al
  	mulw   (BPB_FATSz16)
 	mov    %ax,%cx
	mov    (BPB_ResvdSecCnt),%ax
  	pushw   $FAT_SEG
  	pop    %es
  	xor    %bx,%bx
  	call   ReadSectors
 	pop    %es
  	popa   
  	ret    
/*在根目录中查找文件
ds:si -> 文件名
ret ax 返回文件的簇号，-1表示错误
*/
FindFile:
	push   %es
	push   %cx
 	push   %dx
	push   %bx
	
	mov    $ROOT_SEG,%bx
 	mov    %bx,%es

  	mov    %si,%bx
 	mov    (BPB_RootEntCnt),%cx
 	mov    $ROOT_OFFSET,%di
 	cld    

LOOP:
	push   %cx
	mov    $0xb,%cx
	mov    %bx,%si
 	push   %di
	repz 	cmpsb
	pop    %di
	je     Found
	pop    %cx
	add    $0x20,%di
 	loop   LOOP

NotFound:
	pop    %bx
	pop    %dx
	pop    %cx
	mov    $0xffff,%ax
	pop    %es
 	ret    
Found:
	pop    %ax
	pop    %bx
 	pop    %dx
 	pop    %cx
 	pop    %es
	ret    
/*加载文件
es:si ->文件名
bx:bp -> 文件加载地址
返回值-1错误，0成功*/
LoadFile:
	xor    %ecx,%ecx
 	push   %ecx
/*未找到文件返回-1，找到预加载*/
FIND_FILE:
	push   %bx
 	push   %bp
	call   FindFile
	cmp    $0xffff,%ax
	jne    LOAD_IMAGE_PRE
 	pop    %bp
 	pop    %bx
 	pop    %ecx
	mov    $0xffff,%ax
 	ret    
LOAD_IMAGE_PRE:
	pushw  $ROOT_SEG
	pop    %es
	mov    %es:0x1a(%di),%dx
	mov    %dx,(cluster)
	pop    %bx
	pop    %es
	push   %bx
	push   %es
	call   LoadFAT
LOAD_IMAGE:
	mov    (cluster),%ax
	pop    %es
	pop    %bx
	call   ClusterLBA
	xor    %cx,%cx
	mov    (BPB_SecPerCluse),%cl
	call   ReadSectors
	pop    %ecx
	inc    %ecx
	push   %ecx
	push   %bx
	push   %es
	mov    $FAT_SEG,%ax
	mov    %ax,%es
	xor    %bx,%bx
	mov    (cluster),%ax
	mov    %ax,%cx
	mov    %ax,%dx
	shr    %dx
	add    %dx,%cx
	mov    $0x0,%bx
	add    %cx,%bx
	mov    %es:(%bx),%dx
	test   $0x1,%ax
	jne    ODD_CLUSTER

EVEN_CLUSTER:
	and    $0xfff,%dx
	jmp    DONE

ODD_CLUSTER:
	shr    $0x4,%dx
DONE:
	mov    %dx,(cluster)
	cmp    $0xff0,%dx
	jb     LOAD_IMAGE
	pop    %es
	pop    %bx
	pop    %ecx
	xor    %ax,%ax
	ret 
loadsegc:	.word 0x0
datasector:  .word 33
cluster:     .word 0x0000
absoluteSector: .byte 0x00
absoluteHead:   .byte 0x00
absoluteTrack:  .byte 0x00
