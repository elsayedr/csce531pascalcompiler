program Test7;

type
   t1  = ^t2;
   t3  = ^t4;
   t2  = ^Integer;
   {t4 = t2;}
   t4  = ^Integer;
   a1  = Array[1..5] of ^t5;
   t5  = Real;
   t7  = ^t1;
   t8  = ^t3;
{   s1 = Set of ^s2; }
{   s2 = Real; }
   u1  = Set of 256..511;
   u2  = Set of 0..255;
   u3  = Set of Char;
   v1  = Array[1..5] of Integer;
   v2  = Array[1..5] of Integer;
   ft1 = Function(a : t7) : Integer;
   ft2 = Function(a : t8) : Integer;
   t9  = ^t11;
   t10 = ^t11;
   ft3 = Function(a : t9) : t7;
   ft4 = Function(a : t10) : t8;
   t11 = ^Integer;
   ft5 = Function(a : u1) : Integer;
   ft6 = Function(a : u2) : Integer;
   r1  = 1..5;
   r2  = 7..21;
   pr1 = ^r1;
   pr2 = ^r2;

var
   p  : t1;
   q  : t3;
   a  : a1;
   q7 : t7;
   q8 : t8;
   s1 : u1;
   s2 : u2;
   s3 : u3;
   x1 : v1;
   x2 : v2;
   y1 : 1..10;
   y2 : 11..20;
   y3 : Integer;
   f1 : ^ft1;
   f2 : ^ft2;
   f3 : ^ft3;
   f4 : ^ft4;
   f5 : ^ft5;
   f6 : ^ft6;
   n1 : pr1;
   n2 : pr2;
   n3 : t4;

begin
   p := q;
   q7 := q8;
   s1 := s2;
{  s2 := s3;}
{  x1 := x2;}
   y1 := y2;
   y2 := y3;
{  f1 := f2;}
   f3 := f4;
   f5 := f6;
   n1 := n2;
   n1 := n3;
   n3 := n1;
end.
