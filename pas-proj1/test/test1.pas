program Test1;
{ Testing for case sensitivity in identifiers }
{ Testing criteria for type equality }

type
   Foo	= ^Integer;
   Bar	= ^Integer;
   Bat	= Array[1..5] of Foo;
   Fat	= Array[1..5] of Bar;
   Boo	= Array[1..5] of ^Integer;
   Boo2	= Boo;
   Day	= (Mon,Tue,Wed,Thu,Fri);
   Sod	= Set of Day;
   Sod2	= Set of Day;
   R	= record
	     first  : Integer;
	     second : Day
	  end;	    
   R2	= R;
   R3	= record
	     first  : Integer;
	     second : Day
	  end;	    
   Nptr	= ^Node;
   Node	= record
	     data : Real;
	     next : Nptr
	  end;	  
   Fun	= ^Function(a : Integer) : Real;
   Fun2 = ^Function(a : Integer) : Real;
   Rec  = ^Rec;
   Rec2 = Array[1..3] of Rec2;

var 
   xYZ		    : Integer;
   a		    : ^Integer;
   a1		    : Foo;
   b		    : Bat;
   c		    : Fat;
   d		    : Boo;
   d2		    : Boo2;
   e		    : Array[1..5] of Bar;
   f		    : Array[1..5] of Bar;
   s1		    : Sod;
   s2		    : Sod2;
   s3		    : Set of Day;
   re1		    : R;
   re2		    : R2;
   re3		    : R3;
   re4		    : record
			 first	: Integer;
			 second	: Day
		      end;	
   n		    : Nptr;
   n2		    : ^Node;
   f1		    : Fun;
   f2		    : Fun2;
   rptr		    : Rec;

begin
   rptr := nil;
   f1 := f2;
   n := n2;
   xYz := 3;
   XYZ := 4;
   xyz := 5;
   c := e;
   e := f;
   b := c;
   c := d;
   d := c;
   d := d2;
   b[1] := a;
   b[2] := a1;
   s1 := s2;
   s1 := s3;
   s2 := s3;
   re1 := re2;
   re1 := re3;
   re1 := re4;
   re2 := re3;
   re2 := re4;
   re3 := re4;
   a^ := 3
end.
