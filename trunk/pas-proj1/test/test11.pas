program test11;

var
   i,j : Integer;

begin
   j := 20;
   i := 26;
   for i := 1 to i do begin
      writeln('i = ', i);
      i := i + 1;
      if i > 50 then break;
      j := j - 1
   end;
   writeln('At end, i = ', i)
end.
