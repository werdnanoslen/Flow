#ifndef FIXED_H
#define FIXED_H
/*provides functionality for 8.8 fixed operations
    OH MY LORD READ THE DESCRIPTIONS IF YOU USE THESE MACROS, THEY'RE IMPORTANT AND IT WILL
    ONLY TAKE YOU LIKE 5 MINUTES.
    
    ALSO:
    You will see me shifting by 8 and multiplying/dividing by 256 throughout the code.
    The astute among you will notice that this is the same operation.  Why don't I use all
    of one or the other then?  The fact is, these operations are not necessarily identical
    when signed numbers and floating-point numbers come into play.
*/

/*conversions*/
#define INT2FIX(n) ((signed short)((n) << 8))
#define FLOAT2FIX(n) ((signed short)((n)*256.0)) //WILL DISCARD FRACTION

#define FIX2INT(n) ((signed short)((n) / 256))
#define FIX2FLOAT(n) ((float)((n) / 256.0))

/*multiplication and division*/
/*MULTIPLICATION:
    When you multiply 2 numbers together, in most cases, the radix (point) moves (floats).
    This is the case for fixed point numbers as well.
    Take for example: 
    2.5 * 1.5 = 3.75;
    A couple points:
    *In 8.8 fixed point notation, 2.5 is 00000010 10000000
        (the first 8 bits are 2 shifted over 8 and the second 8 bits are 128, representing 128 256ths, or half )
    *In 8.8 fixed point notation, 1.5 is 00000001 10000000
        (the first 8 bits are 1 shifted over 8 and the second 8 bits are 128, representing 128 256ths, or half )
    *In 8.8 fixed point notation, 3.75 is 00000011 11000000
        (the first 8 bits are 3 shifted over 8 and the second 8 bits are 192, representing 192 256ths, or .75)
    However, C still sees them as regular 16bit integers.  As regular 16bit integers, these
    bit patterns reperesent 640 (512+128) and 384 (256+128), respectively.  C multiplication
    will give you 245760 which doesn't even fit in a 16bit value.  Luckily, the way macros
    work, this doesn't matter, but we should cast the result to a signed short anyway for
    safety's sake.
    
    ANYWAY, the binary representation of 245760 is actually 00000000 00000011 11000000 00000000.
    Notice anything?  Those 4 clustered '1's.  That's the same as in the binary representation
    of 3.75!  It's just out of place.  If we shift it back over by 8 (the number of fraction
    bits for 8.8 fixed point), we'll have the right answer!  Hurray!
    
    See?  That wasn't so hard, was it?
*/
#define fixedMultiply(a, b) ((signed short)( ((a)*(b)) >> 8))


/*DIVISION:
    When you divide two numbers together, again, as before the radix moves.
    Por Ejemplo:
    4.25 / 3.125 = 1.36
    Again, the binary 8.8 representations, respectively:
    *00000100 01000000
    *00000011 00100000
    *00000001 01011100
    
    Note that the last number is not 100% accurate.  It is actually 1.359375.  The fractional
    part is 92.  92 256ths is the same as .359375.  CLOSE ENOUGH.
    C sees the first two numbers as 1088 (1024+64), 800 (512+256+32), and will return 1 if
    you try and divide them, because it's integer division and integer division throws away
    any fractional parts.  Well the 1 part is correct, and 1088/800 (in real world division) IS
    1.36.  How do we prevent this loss of data?  The answer lies in scaling the first number.
    If we multiply 1088 * 256 (this is the coefficient by which the whole number parts of
    an 8.8 number are scaled compared to normal ints (2^8)), and THEN divide by 800, we get
    348, which is in binary: 00000001 01011100.
    HOLY MOLY THAT'S THE SAME AS THE 8.8 REPRESENTATION OF 1.36!
    MATH IS AWESOME!
*/
#define fixedDivide(a, b) ((signed short)(((a)*256)/(b)))
#endif
