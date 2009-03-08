program Test8;

type
   t1 = ^Integer;
   u1 = ^Integer;
   t2 = ^t1;
   u2 = ^u1;
   t3 = ^t2;
   u3 = ^u2;
   t4 = ^t3;
   u4 = ^u3;

var
   x1 : t1;
   y1 : u1;
   x2 : t2;
   y2 : u2;
   x3 : t3;
   y3 : u3;
   x4 : t4;
   y4 : u4;

procedure foo;
type
   v1 = ^t1;
   t1 = ^Real;

var
   v : v1;
   u : u2;

begin
   v := u;
end;

begin 
   x1 := y1;
   x2 := y2;
   x3 := y3;
   x4 := y4;
end.  
