program Test6;
{ Q: Can I define a function returning a pointer to its own type?
  A: Not without allowing type names of function type. }

type
{  f1 = ^Function : f1; }
   f1 = ^f2;
   f2 = Function(a : f1) : f1;

   var
      f	:  f1;

   begin
      f := f^(f);
   end.
