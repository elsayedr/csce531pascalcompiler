program Test2;
{ More pointer type testing }

type
   t0	   = ^t1;
   t1	   = ^Integer;
   Integer = Real;
   t2	   = ^Integer;
   t3	   = ^t1;
   t4	   = ^t2;
   t5	   = ^Real;
   t6	   = Char;
   t7	   = ^Char;
   t8	   = ^t6;

var
   x0 : t0;
   x1 : t1;
   x2 : t2;
   x3 : t3;
   x4 : t4;
   x5 : t5;
   x6 : t6;
   x7 : t7;
   x8 : t8;
   
begin
   x0 := x3;
   x1 := x2;
   x3 := x4;
   x2 := x5;
   x1 := x5;
   x7 := x8;
end.
