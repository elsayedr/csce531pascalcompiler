program Test4;
{ Testing recursive types in records and functions }

type
   Rtype = record
	      data : Integer;
	      next : ^Rtype
	   end;	   
{   Fptr	 = ^Ftype;}
{   Ftype = Function : Fptr;}
{   Gtype = ^Function(a : Integer) : Gtype;}
   Hptr  = ^Htype;
   Htype = ^Function(a : Integer) : Hptr;

var
   r  : ^Rtype;
{   f  : ^Ftype;}
{   g : Gtype;}
   h  : Htype;

begin
   r := r^.next;
{   f := f^;}
{   g := g^(3);}
   h := h^(3)^;
end.
