program point;

type Tpoint = record
  x,y: integer;
end;

{procedure donothing(i: integer);
begin
    i:= 1337;
end;

procedure leet(var i: integer);
begin
    i:= 1337;
end;}

procedure printpt(fk: Tpoint);
begin
  writeln(inttostr(fk.x));
end;

var
  pt: Tpoint;
  //i: integer;
begin
  {writeln(inttostr(i));
  donothing(i);
  writeln(inttostr(i));
  leet(i);
  writeln(inttostr(i));}
  {pt.x:= 5;
  pt.y:= 4;}
  printpt(pt);
end;
