
![Pairs](https://i.imgur.com/8C5rIAE.png)


Pairs is an esoteric programming language based on creating binary numbers using characters.
To create a 1 use two of the same character in a sequence -- Ex: 'ff', '$$', or '55'.
To create a 0 use two different characters in a sequence  -- Ex: 'e#', 'rd', or 'n4'.
Opcodes and operands are formed in 8-bit sequences (16 characters) and each is separated by a space
Ex: 0 is HALT, 1 is store <value> <register> .. 10 is add <value> <register>, 11 is add <register(src)> <register(dest)>, etc.
  
Pairs is something of a reverse-golf language as it produces abnormally large programs (each binary digit requires 2 characters)
This means that each bit representated takes 16 bits to be represented