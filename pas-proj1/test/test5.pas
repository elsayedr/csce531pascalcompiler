program Test5;
{ Type names and scoping }

type
   t2 = ^t1;
   t1 = ^Real;
   t4 = ^t3;
   t6 = ^t5;
   t7 = ^t7;
   t8 = ^t11;
   t9 = ^Function : t8;
   t10 = Array[1..5] of t8;
   t11 = Integer;

var
   x : t2;
   w : t7;

procedure foo;

type
   t1 = ^Char;
   t3 = ^Integer;

var
   y : t2;
   z : t4;

begin { foo }
   x^^ := 3.14;
   x^^ := chr(65);
   y^^ := 3.14;
   y^^ := chr(65);
   z^^ := 65;
end; { foo }

begin
   x^^ := 3.14;
   x^^ := chr(65);
   x := w;
end.
