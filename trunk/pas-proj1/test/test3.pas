program Test3;
{ Testing enumerated types and recursive record types }

type
   Day	 = (Sun,Mon,Tue);
   Color = (Red,Green,Blue);
   Day2	 = (Sun,Mon,Tue);

var
   x  : Day;
   x2 : Day2;
   c  : Color;
   c2 : (Red,Green,blue);

begin
   x := x2;
   c := c2;
end.
