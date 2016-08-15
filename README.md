# assembler

   This program is an universty asinmentan to write an
assymbler for a made up assembly lanuage and create for it the 
object, entry and extern files all writing in octal base
whith spacial characters for the numbers.
0 = '!', 1 = '@', 2 = '#', 3 = '$', 4 =  '%', 5 = '^', 6 =  '&', 7 = '*'.

the regesters are r0,r1,...,r7.

evrey instruction is build from 15 bits:
0-1 -ERA (EXTERNAL, RELOCATEBALE, ABSOLOTE)
2-3 dis operand director
4-5 src operand director
6-8 opcode
10-11 group (zero, one or two operands).
12-14 alwas '101'

יש ארבעה שיטות מיעון
0. מיעון מידי - מיעון של מספר שלם שלפניו סולמית.
1. מיעון ישיר - מיעון של תווית
2. מיעון מידי דינמי - מורכב מתווית ליפני סוגריים מרובעות ומטווח סיביות שיש לבודד
 מהערך בכתובת של התווית לדוגמא:
   mov X[5-9]
3. מיעון אוגר ישיר - שם חוקי של אוגר.

ההוראות ושיטות המיעון החוקיות בשבילן:
opcode	instruction	src_op		dis_op
0	      mov	    	  0,1,2,3 	1,3
1	      cmp		      0,1,2,3		0,1,2,3
2	      add		      0,1,2,3		1,3
3	      sub		      0,1,2,3		1,3
4	      not		      -		      1,3
5	      clr		      -		      1,3
6	      lea		      1		      1,3
7	      inc		      -		      1,3
8	      dec		      -		      1,3
9	      jmp		      -	      	1,3
10	    bne		      -	      	1,3
11	    red	      	-     		1,3
12	    prn		      -	      	0,1,2,3
13	    jsr		      -     		1,3
14	    rts		      -		      -
15	    stop		    -		      -

the instruction and the operand will be seperated with spase
but the operand will be seperated with a comma
example: mov #-1,r2
theres also guide instructions:
.string - a string of characters that will be stored at the data part
          of the program starst whith '"' and ends whith them '"'.
          for example .string "example"
.data   - a list of hole numbers seperated whith commas
          for example .data 1,2,3,4,-1
          
