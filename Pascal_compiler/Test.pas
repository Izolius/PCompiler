type
	myarray=array [0 .. 2] of integer;
var
	arr: array [0 .. 2, 0 .. 2 ] of myarray;
	i, j, k: integer;
begin
	for i:=0 to 2 do
		for j:=0 to 2 do 
			for k:=0 to 2 do 
				arr[i,j][k]:=i*100+j*10+k;
	
	for i:=0 to 2 do
		for j:=0 to 2 do 
			for k:=0 to 2 do 
				printi(arr[i,j][k]);
end.