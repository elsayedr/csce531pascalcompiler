program EightQueens;

type
   QueenArray = array[1..8] of Integer;

var
   Queens	 : QueenArray;
   CurRow	 : Integer;
   i, j		 : Integer;
   answer	 : Char;
   Continue_Flag : Boolean;
   QueensOk_Flag : Boolean;
   GYON_Flag	 : Boolean;


procedure printf; External;
procedure putchar; External;
function getchar : Char; External;


Procedure DrawBoard;

begin
   i := 1;
   while i <= 8 do begin
      j := 1;
      while j <= 8 do begin
	 putchar(ord(' '));
	 if j = Queens[i] then
	    putchar(ord('Q'))
	 else
	    putchar(ord('.'));
         j := j+1
      end;
      putchar(10);
      i := i+1
   end
end; { DrawBoard }


function GetYesOrNo : Char;

begin
   answer := getchar;
   GYON_Flag := true;
   while GYON_Flag do begin
      if answer = 'Y' then
	 GYON_Flag := false
      else if answer = 'y' then
	 GYON_Flag := false
      else if answer = 'N' then
	 GYON_Flag := false
      else if answer = 'n' then
	 GYON_Flag := false;
      if GYON_Flag then
	 answer := getchar
   end;
   GetYesOrNo := answer
end;


function Stop : Boolean;

begin
   printf('Search for another solution? (y/n): ');
   Answer := GetYesOrNo;
   Stop := (Answer = 'N');
   if Answer = 'n' then
      Stop := true
end;


function QueensOk : Boolean;

begin
   i := 1;
   QueensOk_Flag := (i < CurRow);
   QueensOk := true;
   while QueensOk_Flag do
      if Queens[i] = Queens[CurRow] then begin
         QueensOk_Flag := false;
         QueensOk := false
      end
      else if Queens[i] = Queens[CurRow] - (CurRow - i) then begin
         QueensOk_Flag := false;
	 QueensOk := false
      end
      else if Queens[i] = Queens[CurRow] + (CurRow - i) then begin
         QueensOk_Flag := false;
	 QueensOk := false
      end
      else begin
         i := i + 1;
	 QueensOk_Flag := (i < CurRow)
      end
end;


function Search : Boolean;

begin
   if CurRow > 8 then begin
      DrawBoard;
      Search := Stop
   end
   else begin
      Queens[CurRow] := 1;
      Continue_Flag := true;
      while Continue_Flag do begin
         if QueensOk then begin
	    CurRow := CurRow + 1;
	    Continue_Flag := (Search = false);
            CurRow := CurRow - 1
         end;
	 if Continue_Flag then begin
	    Search := false;
	    Queens[CurRow] := Queens[CurRow] + 1;
	    Continue_Flag := (Queens[CurRow] <= 8)
         end
	 else
	    Search := true
      end
   end
end;


begin { main }
   CurRow := 1;
   Continue_Flag := Search;
   if Continue_Flag then
      printf('There could be more solutions.\n')
   else
      printf('No more solutions.\n')
end.
