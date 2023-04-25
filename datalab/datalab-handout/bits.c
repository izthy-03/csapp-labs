/*
 * CS:APP Data Lab
 *
 * <Please put your name and userid here>
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */

#endif
// 1
/*
 * bitXor - x^y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y)
{
    int tmp1 = ~(~x & ~y);
    int tmp2 = ~(x & y);
    return tmp1 & tmp2;
}
/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void)
{
    int res = 1 << 31;
    return res;
}
// 2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x)
{
    int tmp = x + 1;
    int inv = (~tmp) + 1;
    return !(tmp ^ inv) & !!tmp;
}
/*
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x)
{
    int mask1 = 0xAA, mask2, step1, step2;
    mask1 = mask1 << 8 | mask1;  // gen 0xAAAA
    mask1 = mask1 << 16 | mask1; // gen 0xAAAAAAAA
    mask2 = ~mask1;              // gen 0x33333333
    step1 = x & mask1;
    step2 = step1 | mask2;
    return !(~step2);
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x)
{
    return ~x + 1;
}
// 3
/*
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x)
{
    int last4bit = x ^ 0x30;
    int checkBCD = (x >> 4) ^ 0x03;   // confirm 0x30 <= x <= 0x3f
    int lessThan8 = last4bit >> 3;    // confirm 0000 <= last4bit <= 0111
    int lessThan10 = last4bit & 0x06; // mask 0110 to confirm last4bit <= 10 cuz the biggest number is 8+1 without 1 on 2nd&3rd pos
    int flag = (!checkBCD) & (!lessThan8 | !lessThan10);
    return flag;
}
/*
 * conditional - same as x ? y : z
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z)
{
    // Using the properties that x ^ x = 0 and x ^ 0 = x
    int mixYZ = y ^ z;    // the xorSum of y and z. Return y when it xor z, vice versa
    int mask = ~(!x) + 1; // gen 0xffffffff if x==0, 0x00000000 if x!=0
    int ifY = y & mask;   // if x != 0 then xor y, else xor 0;
    int ifZ = z & ~mask;  // if x == 0 then xor z, else xor 0;
    return mixYZ ^ ifY ^ ifZ;
}
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y)
{
    /*int adder = 1 << 31;                    // gen 0x80000000
    int newX = x ^ adder, newY = y ^ adder; // add 2^(w-1), converting to unsigned
    int diff = newX ^ newY;                 // gen 1 on i-th pos where Xi != Yi
    int largerXbit = newX & ~newY;          // gen 1 on i-th pos where Xi > Yi (only Xi=1 & Yi = 0)
    int MSB = diff;
    int isZero, flag;
    MSB = MSB >> 1 | MSB; // get MSB 1
    MSB = MSB >> 2 | MSB;
    MSB = MSB >> 4 | MSB;
    MSB = MSB >> 8 | MSB;
    MSB = MSB >> 16 | MSB;
    isZero = !MSB;
    MSB = ((MSB >> 1) & (~adder)) + !isZero; // corner situation MSB == 0xffffffff
    flag = MSB & largerXbit;
    return !flag;                       //什么究极铸币操作
*/
    int delta = y + (~x + 1);
    int sign = delta >> 31;
    return !sign;
}
// 4
/*
 * logicalNeg - implement the ! operator, using all of
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int logicalNeg(int x)
{
    int lowbit = x & (~x + 1);
    int minus1 = lowbit + (~0);
    int reverse = ~minus1;
    int rshift = reverse >> 31;
    return ~rshift & 1;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x)
{
    int signbit = (x >> 31) & 1;
    int reverser = (~0) + !signbit;
    int x1 = x ^ reverser; // if x < 0, reverse it
    int mask1 = 0x55, mask2 = 0x33, mask3 = 0x0f, mask4 = 0xff, mask5 = 0xff;
    // Then make 1s after the MSB
    x1 = x1 >> 1 | x1;
    x1 = x1 >> 2 | x1;
    x1 = x1 >> 4 | x1;
    x1 = x1 >> 8 | x1;
    x1 = x1 >> 16 | x1; // Got a string in form of 00..01..111, then calculate then number of 1s, that is the answer
    // Gen masks
    mask1 = mask1 << 8 | mask1, mask1 = mask1 << 16 | mask1; // mask1 = 0x55555555
    mask2 = mask2 << 8 | mask2, mask2 = mask2 << 16 | mask2; // mask2 = 0x33333333
    mask3 = mask3 << 8 | mask3, mask3 = mask3 << 16 | mask3; // mask3 = 0x0f0f0f0f
    mask4 = mask4 << 16 | mask4;                             // mask4 = 0x00ff00ff
    mask5 = mask5 << 8 | mask5;                              // mask5 = 0x0000ffff
    // Count 1s
    x1 = (x1 & mask1) + ((x1 >> 1) & mask1);
    x1 = (x1 & mask2) + ((x1 >> 2) & mask2);
    x1 = (x1 & mask3) + ((x1 >> 4) & mask3);
    x1 = (x1 & mask4) + ((x1 >> 8) & mask4);
    x1 = (x1 & mask5) + ((x1 >> 16) & mask5);
    return x1 + 1;
}
// float
/*
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf)
{
    unsigned int fMask = 0x7fffff;
    unsigned int s = uf & 0x80000000;
    unsigned int e = (uf << 1) >> 24;
    unsigned int f = uf & fMask;
    unsigned int uf2;
    if (e == 0) // Denormalized
    {
        f <<= 1;
    }
    else if (e < 0xff) // Normalized
    {
        e += 1;
    }
    uf2 = s | (e << 23) | f;
    return uf2;
}
/*
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf)
{
    unsigned int fMask = 0x7fffff;
    unsigned int s = uf >> 31;
    unsigned int e = (uf << 1) >> 24;
    unsigned int f = uf & fMask;
    int ufInt, E = e - 127, outOfRange = 0;
    if (E < 0) // e-Bias < 0
    {
        ufInt = 0;
    }
    else
    {
        f |= fMask + 1;
        if (E <= 23)
            ufInt = f >> (23 - E);
        else if (E <= 30)
            ufInt = f << (E - 23);
        else
            outOfRange = 1;
    }

    if (outOfRange)
        ufInt = 0x80000000;
    if (s)
        ufInt = ~ufInt + 1;
    return ufInt;
}
/*
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatPower2(int x)
{
    unsigned ufRes;
    if (x < -126)
    {
        ufRes = 0;
    }
    else if (x > 127)
    {
        ufRes = 0x7f800000;
    }
    else
    {
        x = x + 127;
        ufRes = x << 23;
    }
    return ufRes;
}
/*




*/