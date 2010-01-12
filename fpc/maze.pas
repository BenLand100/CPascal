program maze;

const small_maze =
'25' + #10 +
'13' + #10 +
'00000000000000000000000002'+ #10 +
'03011111011111100010001102'+ #10 +
'01010101010000111111101002'+ #10 +
'01111101010110101000101002'+ #10 +
'01000111010100101110111112'+ #10 +
'01010100010110100000100012'+ #10 +
'01110111010011101100101012'+ #10 +
'00101101010110001011101012'+ #10 +
'01111001010100101010011012'+ #10 +
'01001011110111111110010012'+ #10 +
'01111010010000000111110012'+ #10 +
'00100111111111111000000042'+ #10 +
'00000000000000000000000005'+ #10 ;

const medium_maze ='61' + #10 +
'61' + #10 +
'0000000000000000000000000000000000000000000000000000000000000' + #10 +
'0111111111111100111111111111111111111111111111111111111110000' + #10 +
'0101000000000000100000000001000000000000000000000000000010000' + #10 +
'0101000000000000100100000001000111110000000000000000000010000' + #10 +
'0101000111111111100100000001000100000011111111111111110010000' + #10 +
'0101011101000100000111110001000100000010000000000000010010000' + #10 +
'0101010001000100000000010001000100000010000000000000010010000' + #10 +
'0101010001000100111111110001111111111110011100000000010010000' + #10 +
'0101011101011100000000010000000000000000000100000000010010000' + #10 +
'0101000001000000000000010011111111111111100111111100010010000' + #10 +
'0101111111111111111100010010000000000000000100000100010010000' + #10 +
'0100010000000000000100010011111111111111111100000100010011110' + #10 +
'0100011110000001000101110010000000000000000000000100010000010' + #10 +
'0100010010100001000101010010000000000000000000000100010000010' + #10 +
'0100010010111111111101010010011111111111111111111100010100010' + #10 +
'0111010000000001000101010010010000000001000010000111110100010' + #10 +
'0000011111100001000101011110010010000001000010000000000100010' + #10 +
'0000010000001111000101010000010010000001001010111111111100010' + #10 +
'3111111110100000000101010100010010000001001010100000000000010' + #10 +
'0000000010101111111101010100011111100001111010100000111110010' + #10 +
'0111111010101000001001010100010000100100000010100000100011110' + #10 +
'0100001010101000001001010100010000100100000010100000100000000' + #10 +
'0101101010101001001111010100010011100111110010100111100000000' + #10 +
'0100101010101001001000010111110000000100000010100000100000100' + #10 +
'0100101010101001001000010000100000000100000010100000100000100' + #10 +
'0100101010101001111001110000100011111100111110100000111111100' + #10 +
'0100111010101000001001000000100010000100100010100000100000000' + #10 +
'0100000010101111101001000000100010000100100010111111100000000' + #10 +
'0100000010100000101001001111100010000100100010000000100000000' + #10 +
'0111111111100000101001001000100000000100000010000000100000000' + #10 +
'0010100001000000101001001000111111111111111111111111111111114' + #10 +
'0010100001000100101001001000100000000000001001010010000000000' + #10 +
'0010111101010100101000001000100000000000001001010010000000000' + #10 +
'0010000101010100101111101000100011111111101001010011111110000' + #10 +
'0010000101010111101000101000100010000000001001010000000010000' + #10 +
'0111110101010000001000101000100010011111001001011111110010000' + #10 +
'0100000101011111111000100000100010010000001001000000010011110' + #10 +
'0100111101000000001000000000100010111111111001000000010000010' + #10 +
'0100000001111111111111111111111110100000000001110000010000010' + #10 +
'0100000000000000000000000000000000100011110100011110010000010' + #10 +
'0101111111111111111111111101111111100010000100010010010000010' + #10 +
'0101000000000000000000000101000000100010010100010010010000010' + #10 +
'0101000000111111111111100101100000100010010100010010010111110' + #10 +
'0101000000100000000000100100111110111110010100010010010100000' + #10 +
'0101000000100000000000100100000010101000010111110010010100000' + #10 +
'0101011111111111111100100101111110101000010000000010010101110' + #10 +
'0101010000000010000100100100000000101000010000000000010101010' + #10 +
'0101010111111010000110100111111111101011111111111111110101010' + #10 +
'0101010100001010000010100000000000001000100000000000000101010' + #10 +
'0101010111101010000010100000000000001000100000000000000101010' + #10 +
'0101010000101010000010100000111100001010111111111111111101010' + #10 +
'0101011111101011111010100000100100101110100000000000000101010' + #10 +
'0101000000001010001010111111100100100000100011111111100101010' + #10 +
'0101111111111010101010100000000100100000100010000000000101010' + #10 +
'0100000000000010101010100000000100100000100010000000000101010' + #10 +
'0111111111111110101010100000000000101111100010000000000101010' + #10 +
'0100000000000000101010100001111111100000100010111111111101010' + #10 +
'0100111111111111101010100001000000000000100011100000100000010' + #10 +
'0100100000000000001010111111111000000000100000000000100000010' + #10 +
'0110111111111111111010000000001111111111101111111111111111110' + #10 +
'0000000000000000000000000000000000000000000000000000000000000' + #10 ;

type Tpoint = record
  x,y: integer;
end;

type TpointArray = array of TPoint;

type TpointArrayArray = array of TpointArray;

type TboolGrid = array of array of boolean;

function readLine(raw: string; var off: integer): string;
begin
  result:= '';
  while raw[off] <> chr(10) do
  begin
    result:= result + raw[off];
    inc(off);
  end;
  inc(off);
end;


procedure ParseMaze(raw: string; var start, finish: TPoint);//: TboolGrid;
var
  //tiles: TboolGrid;
  c: char;
  i,w,h,x,y: integer;
begin
  writeln('Initializing...');
  writeln(raw);
  {i:= 1;
  w:= strtoint(readLine(raw,i));
  h:= strtoint(readLine(raw,i));
  SetLength(tiles,w);
  for x:= 0 to w - 1 do
    SetLength(tiles[x],h);
  x:= 0;
  y:= 0;
  writeln('Parsing...');
  while y < h do
  begin
    c:= raw[i];
    inc(i);
    case c of
      '0': begin
        tiles[x][y]:= false;
        inc(x);
      end;
      '1': begin
        tiles[x][y]:= true;
        inc(x);
      end;
      '3': begin
        start.x:= x;
        start.y:= y;
        tiles[x][y]:= true;
        inc(x);
      end;
      '4': begin
        finish.x:= x;
        finish.y:= y;
        tiles[x][y]:= true;
        inc(x);
      end;
    end;
    if x = w then begin x:= 0; inc(y); end;
  end;
  result:= tiles;}
end;


function notPassed(passed: TpointArray; x,y: integer): boolean;
var
  i: integer;
begin
  result:= true;
  for i:= length(passed) - 1 downto 0 do
    if (passed[i].x = x) and (passed[i].y = y) then begin result:= false; exit; end;
end;

type TghettoStack = record
  passed,my_path: TpointArray;
  x,y: integer;
end;

function flood(var input: array of array of boolean; sx,sy,dx,dy,w,h: integer): array of TpointArray;
var
  i,off,len,top,scrat:integer;
  stack: array of TghettoStack;
  my_path,passed: TpointArray;
  x,y: integer;
begin
  len:= 1000;
  SetLength(stack,len);
  SetLength(stack[0].passed,0);
  stack[0].x:= sx;
  stack[0].y:= sy;
  off:= 0;
  top:= 1;
  while top > 0 do
  begin
    if top+4 > len then
    begin
      writeln('Growing!');
      len:= len + 1000;
      SetLength(stack,len);
    end;
    top:= top - 1;
    passed:= stack[top].passed;
    x:= stack[top].x;
    y:= stack[top].y;
    SetLength(my_path,Length(passed)+1);
    //writeln('following (' + inttostr(x) + ',' + inttostr(y) + ')  ' + inttostr(length(my_path)));
    for i:= 0 to Length(passed) - 1 do
      my_path[i]:= passed[i];
    my_path[Length(passed)]:= point(x,y);
    if (x = dx) and (y = dy) then
    begin
      writeln('found path! ' + inttostr(off));
      SetLength(result,off+1);
      result[off]:= my_path;
      inc(off);
    end;
    if (y-1 >= 0) then if input[x][y-1] then if notPassed(my_path,x,y-1) then
    begin
      stack[top].passed:= my_path;
      stack[top].x:= x;
      stack[top].y:= y-1;
      top:= top + 1;
    end;
    if (y+1 < h) then if input[x][y+1] then if notPassed(my_path,x,y+1) then
    begin
      stack[top].passed:= my_path;
      stack[top].x:= x;
      stack[top].y:= y+1;
      top:= top + 1;
    end;
    if (x-1 >= 0) then if input[x-1][y] then if notPassed(my_path,x-1,y) then
    begin
      stack[top].passed:= my_path;
      stack[top].x:= x-1;
      stack[top].y:= y;
      top:= top + 1;
    end;
    if (x+1 < w) then if input[x+1][y] then if notPassed(my_path,x+1,y) then
    begin
      stack[top].passed:= my_path;
      stack[top].x:= x+1;
      stack[top].y:= y;
      top:= top + 1;
    end;
  end;
end;

procedure prune(var input: array of array of boolean; x,y,w,h: integer);
var
  nx,ny,c: integer;
begin
  nx:= x;
  ny:= y;
  repeat
    x:= nx;
    y:= ny;
    input[x][y]:= false;
    c:= 0;
    if (y-1 >= 0) then if input[x][y-1] then
    begin
      c:= c + 1;
      ny:= y - 1;
    end;
    if (y+1 < h) then if input[x][y+1] then
    begin
      c:= c + 1;
      ny:= y + 1;
    end;
    if (x-1 >= 0) then if input[x-1][y] then
    begin
      c:= c + 1;
      nx:= x - 1;
    end;
    if (x+1 < w) then if input[x+1][y] then
    begin
      c:= c + 1;
      nx:= x + 1;
    end;
  until c <> 1;
  input[x][y]:= true;
end;
{
procedure thin(var input: array of array of boolean; sx,sy,fx,fy,w,h: integer);
var
  x,y,c: integer;
begin
  for x:= 0 to w-1 do
    for y:= 0 to h-1 do
      if input[x][y] then
      begin
        c:= 0;
        if (y-1 >= 0) then if input[x][y-1] then c:= c + 1;
        if (y+1 < h) then if input[x][y+1] then c:= c + 1;
        if (x-1 >= 0) then if input[x-1][y] then  c:= c + 1;
        if (x+1 < w) then if input[x+1][y] then c:= c + 1;
        if c <= 1 then if ((x<>sx) or (y<>sy)) and ((x<>fx) or (y<>fy)) then prune(input,x,y,w,h);
      end;
end;
}
procedure benland100_solver(var jacks_path: TPointArray; var jills_routes: Integer; input: array of array of Boolean; start, finish: TPoint);
var
   all_paths: array of TpointArray;
   i,max_pos,max_len: integer;
begin
  thin(input,start.x,start.y,finish.x,finish.y,length(input),length(input[0]));
  all_paths:= flood(input,start.x,start.y,finish.x,finish.y,length(input),length(input[0]));
  jills_routes:= Length(all_paths) - 1;
  max_pos:= 0;
  max_len:= length(all_paths[0]);
  for i:= 1 to jills_routes do
  begin
    if length(all_paths[i]) > max_len then
    begin
      max_len:= length(all_paths[i]);
      max_pos:= i;
    end;
  end;
  jacks_path:= all_paths[max_pos];
end;

var
  maze: array of array of boolean;
  start,finish: TPoint;
  jack: TPointArray;
  i,jills,time: integer;
begin
  ParseMaze(medium_maze,start,finish);
  {time:= GetSystemTime;
  benland100_solver(jack,jills,maze,start,finish);
  time:= GetSystemTime - time;
  writeln('Took ' + floattostr(time / 1000.0) + ' seconds');
  writeln('Longest Path: ' + inttostr(length(jack)));
  writeln('#Other Paths: ' + inttostr(jills));}
end.
