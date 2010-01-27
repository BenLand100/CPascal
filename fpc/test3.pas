program test3;

type
    TPoint = record
        x,y: integer;
    end;

var
    i: integer;
    pt: TPoint;
begin
    i:= 42;
    pt.x:= 58;
    writeln(inttostr(pt.x));
end;
