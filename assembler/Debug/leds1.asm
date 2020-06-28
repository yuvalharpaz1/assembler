	add $a0, $zero, $imm, 0				 # $t0 = 0
	add $a1, $zero, $imm, 1              # $t1 = 1
	add $s1, $zero, $imm, 1024           # $t0 = 1024
	out $s1, $zero, $imm, 16             # setting diskbuffer in line 1024
	add $t0, $zero, $imm, 1              # $t0=1
    out $t0, $zero, $imm, 1              # enable irq1
	out $t0, $zero, $imm, 0              # enable irq0
	add $t0, $zero, $imm, 6              # $t0 = 6
	out $imm, $zero, $t0, L2             # set irqhandler as L2
	jal $imm, $zero, $zero, leds         # turn on the leds one by one 
	halt $zero, $zero, $zero, 0             # halt
leds :
	add $t0, $zero, $imm, 255            # $t1 = 255
	out $t0, $zero, $imm, 13             # timermax = 255
	add $t0, $zero, $imm, 1              # $t0 = 1
	out $t0, $zero, $imm, 9              # turn on the LSB led
	out $t0, $zero, $imm, 11             # turn on timerenable
L1:
    in $t1, $zero, $imm, 3               # stay in the loop for 1 second 
	beq $imm, $zero, $t1, L1             # if irq0status turn on get out of loop
	beq $imm, $zero, $zero, L1           #jump back to L1 loop for another 1 second
L2:
	in $v0, $zero, $imm, 4
	bne $imm, $zero, $v0, LL2
	out $a0, $imm, $zero, 15			 # set disksector
	out $a1 $zero, $imm, 14              # set diskcmd as $t1
	CONTINUE:
	add $v0, $zero, $imm, 1
	out $v0, $zero, $imm, 4
	out $zero, $zero, $imm, 4
	sll $t0, $t0, $imm, 1                # $t0*=2
	out $t0, $zero, $imm, 9              # turn on the next led and turn on the one before
	beq $ra, $zero, $t0, 0               # if all the leds turned off exit
	add $zero, $zero, $zero, 0           # a line to get a symetry of 256 cycles between one turn on of a led to another one
	out $zero, $imm, $zero, 3            # turn off irq0status
	reti $zero, $zero, $zero, 0          # return from interrupt 

LL2:
	in $s1, $zero, $imm, 17              # $t2=diskstatus
	bne $imm, $s1, $zero, LL2            # run in loop until diskstatus=0
	LL3 :
	add $s1, $zero, $imm, 7
	out $imm, $zero, $s1, L1
	out $zero, $imm, $zero, 4			 # turn off irq1status
	add $s2, $zero, $imm, 1				 # $t2 = 1
	bne $imm, $a1, $s2, LL4				 # if last discmd is reading go to L4
	add $a1, $imm, $zero, 2				 # $a1 = 2, if last diskcmd was to read the code will get to here so next diskcmd will be to write
	add $a0, $a0, $imm, 4				 # $a0 += 4
	 reti $zero, $zero, $zero, 0		 # return from interrupt
LL4 :									 # if last diskcmd was to write the code will get to here so next diskcmd will be to read
	add $a1, $s2, $zero, 0				 # $t1 = $t2 = 1
	sub $a0, $a0, $imm, 3				 # $t0-=3
	beq $ra, $imm, $a0, 4				 # if  need to read sector number 4 finish
	 reti $zero, $zero, $zero, 0		 # return from interrupt
