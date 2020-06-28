		#/*clock1*/
		
	.word 1024 0x195955                  # saving starting time
	.word 1025 0x1fffff                # saving important time one second before 200000
	add $t0, $zero, $imm, 1              # $t0 = 1
	out $t0, $zero, $imm, 0              # enable irq0
	add $t0, $zero, $imm, 6              # $t0 = 6
	out $imm, $zero, $t0, L2             # set irqhandler as L2
	jal $imm, $zero, $zero, clock         # run the clock for 10 seconds from 19:59:55 to 20:00:05
	halt $zero, $zero, $zero, 0             # halt
clock:
	add $t0, $zero, $imm, 255            # $t0 = 255
	out $t0, $zero, $imm, 13             # timermax = 255
	add $t0, $zero, $imm, 1                # $t0 = 1
	lw $t1, $zero, $imm, 1024              # get the starting time from adress 1024 into $t0
	out $t1, $zero, $imm, 10              # start to display the time
	out $t0, $zero, $imm, 11             # turn on timerenable
L1:
	in $t0, $zero, $imm, 3    # read irq0status into $t1
	beq $imm, $t0, $zero, L1 # if irq0status eqyal 1 go out from loop L1
	beq $imm, $zero, $zero, L1
L2 :
	out $zero, $zero, $imm, 3 # turn off irq0status
	add $t1, $t1, $imm, 1    # $t1 += 1
	out $t1, $zero, $imm, 10 # display the new time after 1 second, here we accomplish 2 more cycles after loop L1 to get 1 second
	add $t2, $zero, $imm, 9 #$t2=9
	and $t3, $imm, $t1, 0xf  # checking if the 4 LSB bits equal to 9
	beq $imm, $t3, $t2, L3  # if the crrent time is 195959 go to L3
	add $t2, $zero, $imm, 5 # $t2=5
	and $t3, $t1, $imm, 0xf # checking if the 4 LSB bits equal to 5
	beq $ra, $t3, $t2, 0 # if the crrent time is 200005 exit
	reti $zero, $zero, $zero, 0          # return from interrupt
L3 :
	lw $t1, $zero, $imm, 1025 # get the important time from adress 1025 into $t1
	reti $zero, $zero, $zero, 0          # return from interrupt 



	.word 1024 0x195955                  # save starting time
	.word 1025 0x1fffff                  #save important time one second before 200000
	add $t0, $zero, $imm, 1              # $t0 = 1
	out $t0, $zero, $imm, 0              # enable irq0
	add $t0, $zero, $imm, 6              # $t0 = 6
	out $imm, $zero, $t0, L2             # set irqhandler as L2
	jal $imm, $zero, $zero, clock         # run the clock for 10 seconds from 19:59:55 to 20:00:05
	halt $zero, $zero, $zero, 0             # halt
clock:
	add $t0, $zero, $imm, 254            # $t1 = 254
	out $t0, $zero, $imm, 13             # timermax = 254
	lw $t0, $zero, $imm, 1024              # get the starting time from adress 1024 into $t0
	out $t0, $zero, $imm, 10              # start to display the time
		out $t0, $zero, $imm, 11             # turn on timerenable
L1 :
	beq $imm, $zero, $zero, L1           # stay in the loop for 1 second minus 2 cycles
	beq $imm, $zero, $zero, L1           # go back to the loop for 1 second minus 2 cycles
L2:
	add $t0, $t0, $imm, 1 # $t0+=1
	out $t0, $zero, $imm, 10 # display the new time after 1 second, here we accomplish 2 more cycles after loop L1 to get 1 second
	out $zero, $imm, $zero, 3 # turn off irq0status
	and $t1, $t0, $imm, 0xf # checking if the 4 LSB bits equal to 9 
	add $t2, $zero, $imm, 9 # $t2=9 
	beq $imm, $t1, $t2, L3 # if the crrent time is 195959 go to L3
	and $t1, $t0, $imm, 0xf # checking if the 4 LSB bits equal to 9
	beq $ra, $t1, $imm, 5 # if the current time is 200005 exit
	reti $zero, $zero, $zero, 0    # return from interrupt


L3:
	lw $t0, $zero, $imm, 1025  # get important time from adress 1025 into $t0
	out $zero, $imm, $zero, 3 # turn off irq0status
	reti $zero, $zero, $zero, 0  # return from interrupt