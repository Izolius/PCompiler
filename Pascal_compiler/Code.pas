(*
var
	arr: array ['a' .. 'e'] of integer;
	c: char;
	s, i: integer;

function sum(a, b, c:integer): integer;
begin
	
end;
begin
	i:=0;
	s:=0;
	for c:='e' downto 'a' do
	begin
		arr[c]:=i;
		i:=i+1;
	end;

	for c:='a' to 'e' do
	begin
		s:=s+arr[c];
	end;
	printi(s);
end.
*)
type
	arrtype= array ['a' .. 'c'] of integer;
var
	arr: array [1 .. 2, 1 .. 2] of arrtype;
	i,j, ki:integer;
	k:char;
begin
	for k:='a' to 'c' do 
		arr[0,0][ k]:=13;
	for k:='a' to 'c' do 
		printi(arr[0,0][ k]);
	arr[0,0]['c']:=13;
	printi(arr[0,0]['c']);
	(*
	for i:=1 to 2 do
		for j:=1 to 2 do begin
			for k:='a' to 'c' do begin
				arr[i,j][ k]:=i*100+j*10+ki;
				ki:=ki+1;
			end;
		end;

	for i:=1 to 2 do
		for j:=1 to 2 do
			for k:='a' to 'c' do
				printi(arr[i,j][ k]);
				*)
end.