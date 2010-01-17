program maze;

const small_maze =
'25' + #10 +
'13' + #10 +
'0000000000000000000000000'+ #10 +
'0301111101111110001000110'+ #10 +
'0101010101000011111110100'+ #10 +
'0111110101011010100010100'+ #10 +
'0100011101010010111011111'+ #10 +
'0101010001011010000010001'+ #10 +
'0111011101001110110010101'+ #10 +
'0010110101011000101110101'+ #10 +
'0111100101010010101001101'+ #10 +
'0100101111011111111001001'+ #10 +
'0111101001000000011111001'+ #10 +
'0010011111111111100000004'+ #10 +
'0000000000000000000000000'+ #10 ;

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
  while raw[off] <> #10 do
  begin
    result:= result + raw[off];
    off:= off + 1;
  end;
  off:= off + 1;
end;


function ParseMaze(raw: string; var start, finish: TPoint): TboolGrid;
var
  tiles: TboolGrid;
  c: char;
  i,w,h,x,y: integer;
begin
  write(raw);
  writeln('Parsing...');
  i:= 1;
  w:= strtoint(readLine(raw,i));
  h:= strtoint(readLine(raw,i));
  SetLength(tiles,w);
  for x:= 0 to w - 1 do
    SetLength(tiles[x],h);
  x:= 0;
  y:= 0;
  writeln('W:' + inttostr(w) + ' H:' + inttostr(h));
  while y < h do
  begin
    c:= raw[i];
    i:= i + 1;
    case c of
      '0': begin
        tiles[x][y]:= false;
        x:= x + 1;
      end;
      '1': begin
        tiles[x][y]:= true;
        x:= x + 1;
      end;
      '3': begin
        Writeln('found start!');
        start.x:= x;
        start.y:= y;
        tiles[x][y]:= true;
        x:= x + 1;
      end;
      '4': begin
        Writeln('found finish!');
        finish.x:= x;
        finish.y:= y;
        tiles[x][y]:= true;
        x:= x + 1;
      end;
    end;
    if x = w then begin x:= 0; y:= y + 1; end;
  end;
  result:= tiles;
  writeln('Start = (' + inttostr(start.x) + ',' + inttostr(start.y) + ')');
  writeln('Finish = (' + inttostr(finish.x) + ',' + inttostr(finish.y) + ')');
  //writeln('a1:'+inttostr(length(tiles)));
  //writeln('a2:'+inttostr(length(tiles[0])));
  //writeln('b1:'+inttostr(length(result)));
  //writeln('b2:'+inttostr(length(result[0])));
end;


function notPassed(var passed: TpointArray; x,y: integer): boolean;
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

function point(x,y:integer):TPoint;
begin
    result.x:= x;
    result.y:= y;
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
  writeln('pushing start: (' + inttostr(sx) + ',' + inttostr(sy) + ')');
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
    begin
      my_path[i]:= passed[i];
    end;
    my_path[Length(passed)]:= point(x,y);
    if (x = dx) and (y = dy) then
    begin
      writeln('found path! ' + inttostr(off));
      SetLength(result,off+1);
      result[off]:= my_path;
      off:= off + 1;
      //writeln('continuing search');
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

procedure thin(var input: array of array of boolean; sx,sy,fx,fy,w,h: integer);
var
  x,y,c: integer;
begin
  //writeln('examining maze');
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

procedure benland100_solver(var jacks_path: TPointArray; var jills_routes: Integer; input: array of array of Boolean; start, finish: TPoint);
var
   all_paths: array of TpointArray;
   i,max_pos,max_len: integer;
begin
  //writeln('d1:'+inttostr(length(maze)));
  //writeln('d2:'+inttostr(length(maze[0])));
  writeln('preparing to solve maze');
  thin(input,start.x,start.y,finish.x,finish.y,length(input),length(input[0]));
  writeln('thinned the maze');
  //writeln('start: (' + inttostr(start.x) + ',' + inttostr(start.y) + ') finish: (' + inttostr(finish.x) + ',' + inttostr(finish.y) + ')');
  all_paths:= flood(input,start.x,start.y,finish.x,finish.y,length(input),length(input[0]));
  writeln('found all possible paths');
  jills_routes:= Length(all_paths) - 1;
  max_pos:= 0;
  max_len:= length(all_paths[0]);
  writeln('Finding longest...');
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
  i,jills,t: integer;
begin
  writeln('Solving maze...');
  maze:= ParseMaze(medium_maze,start,finish);
  //writeln('c1:'+inttostr(length(maze)));
  //writeln('c2:'+inttostr(length(maze[0])));
  t:= time();
  //writeln('start: (' + inttostr(start.x) + ',' + inttostr(start.y) + ') finish: (' + inttostr(finish.x) + ',' + inttostr(finish.y) + ')');
  benland100_solver(jack,jills,maze,start,finish);
  t:= time() - t;
  writeln('Took ' + realtostr(t / 1000.0) + ' seconds');
  writeln('Longest Path: ' + inttostr(length(jack)));
  writeln('#Other Paths: ' + inttostr(jills));
end.
