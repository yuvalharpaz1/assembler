	 # //DISKTEST1
add $a0, $zero, $imm, 1024        # $a0=1024 location of diskbuffer
    add $t0, $zero, $imm, 1           # $t0=1
    out $t0, $zero, $imm, 1           # enable irq1
    add $t0, $zero, $imm, 6           # $t0=6
    out $imm, $zero, $t0, L3          # set irqhandler as L3
    jal $imm, $zero, $zero, disktest  # copy sectors 0-3 to 4-7
    halt $zero, $zero, $zero, 0       # halt
disktest:
	add $t0, $zero, $imm, 0          # $t0 = 0
	add $t1, $zero, $imm, 1           # $t1 = 1
	add $t2, $zero, $imm, 1024          # $t0 = 1024
	out $t2, $zero, $imm, 16         #setting diskbuffer in line 1024
L1:
	out $t0, $imm, $zero, 15          # set disksector
	out $t1 $zero, $imm, 14       # set diskcmd as $t1
L2:
	in $t2, $zero, $imm, 17           # $t2=diskstatus
	bne $imm, $t2, $zero, L2          # run in loop until diskstatus=0
	beq $imm, $zero, $zero, L1         # jump back to L1 to do next action on the disk
L3 :
	add $t2, $zero, $imm, 7
	out $imm, $zero, $t2, L1
	out $zero, $imm, $zero, 4         # turn off irq1status
	add $t3, $zero, $imm, 1           # $t2 = 1
	bne $imm, $t1, $t3, L4            # if last discmd is reading go t0 L4
	add $t1, $imm, $zero, 2           # $t1 = 2, #if last diskcmd was t0 read the code will get to here so next diskcmd will be to write
	add $t0, $t0, $imm, 4             # $t0 += 4
	 reti $zero, $zero, $zero, 0       #return from interrupt
L4 :                                  #if last diskcmd was to write the code will get to here so next diskcmd will be to read
	add $t1, $t3, $zero, 0            # $t1 = $t2 = 1
	sub $t0, $t0, $imm, 3             # $t0-=3
	beq $ra, $imm, $t0, 4             # if  need to read sector number 4 finish
	 reti $zero, $zero, $zero, 0       #return from interrupt
	
	
	