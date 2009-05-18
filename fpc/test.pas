program Test; 

const 
    pi = 3.14159;

function multiply(a,b:integer):integer;
begin  
    result:= a*b;
end; 

function dividei(a,b:integer):integer;
begin  
    result:= a div b; 
end; 

function dividef(a,b:integer):real;
begin
    result:= a / b;
end;

function subtract(a,b:integer):integer;
begin  
    result:= a-b; 
end; 

function add(a,b:integer):integer;
begin 
    result:= a+b;
end; 

function newChar: char;
begin
    result:= 'a';
end;

function newString: string;
begin
    result:= 'first ' + 'second ' + 'third';
end;

function newReal: real;
begin
    result:= pi;
end;

function newInteger: integer;
begin
    result:= 42;
end;

function newBoolean: boolean;
begin
    result:= true;
end;

procedure testorderoperations;
var
 a,b,c:integer;
begin
    a:= 1+1*2;
    b:= (1+1)*2;
end;

procedure testtruths;
begin
    if (1 < 2) and (2 > 1)  then
        writeln('success');
end;

procedure testwhile;
var
    i:integer;
begin
    i:= 0;
    while i < 10 do
    begin
        writeln(inttostr(i));
        i:= i + 1;
    end;
end;

procedure testfor;
var
    i: integer;
    c: char;
begin
    for i:= 0 to 10 do
        writeln(inttostr(i));
    for i:= 10 downto 0 do
    begin
        writeln(inttostr(i));
    end;
    for c:= 'a' to 'z' do
        writeln(chartostr(c));
end;

function forlooptime: real;
var
    i, t: integer;
begin
    //writeln(inttostr(time));
    //wait(1000);
    //writeln(inttostr(time));
    //t:= time;
    //for i:= 1 to 10000 do 
    //    i:= i;
    //result:= (time - t) / 10000;
end;

function testrepeatuntil;
var
    i: integer;
begin
    i:= 0;
    repeat
        i:= i + 2;
    until i > 50;
    writeln('i: ' + inttostr(i));
end;

procedure testifelse;
begin
    if true then
        writeLn('success');    
    
    if true then
        writeln('success');
    else
        writeln('fail');
        
    if false then
        writeln('fail');
    else
        writeln('success');
        
    if false then begin
        writeln('fail');
    end else begin
        writeln('success');
    end;
    
    if false then 
        writeln('fail');
    else if true then
        writeln('siuccess');
    else
        writeln('fail');
        
    if false then
        writeln('fail');
    else if false then
        writeln('fail');
    else
        writeln('success');
    
end;

procedure testcase;
var
   i: integer;
   c: char;
begin
  for i:= 1 to 5 do
    case i of
      1: begin writeln('haha'); writeln('_1'); end;
      2: writeln('_2');
      3: writeln('_3');
      4: writeln('_4');
    else
      writeln('unknown');
    end;


  for c:= 'a' to 'f' do
    case c of
      'a': begin writeln('haha'); writeln('_a'); end;
      'b': writeln('_b');
      'c': writeln('_c');
      'd': writeln('_d');
    else
      writeln('unknown');
    end;
end;


procedure doarraystuff;
var
    ai: array of integer;
    static: array[1..5] of integer;
    aii: array of array of integer;
begin
    resize(ai,5);
    writeln('sized');
    ai[0]:=0;
    ai[1]:=1;
    ai[2]:=2;
    ai[3]:=3;
    ai[4]:=4;
    writeln('assigned');
    writeln(inttostr(size(ai)));
    writeln(inttostr(ai[3]));
    static[1]:=1;
    static[2]:=2;
    static[3]:=3;
    static[4]:=4;
    static[5]:=5;
    writeln('assigned');
    writeln(inttostr(static[5]));
    ai := array(1,2,3,4,5,6);
    writeln(inttostr(ai[3]));
    writeln(inttostr(size(ai)));
    aii:= array(array(1,2,3,4),array(5,6,7,8),array(9,10,11,12));
    writeln(inttostr(aii[1][3])); //should be a 8
    writeln('arrays work!');
end;


type
    number = integer;
    numarr = array of integer;
    
procedure testtypes;
var
    x: number;
    a: numarr;
begin
    x:= 1;
    writeln(inttostr(x));
    resize(a,10);
end;
type 
    entry = record
        name: string;
        number: integer;
    end;

procedure phonebook;
var
    contact: entry;
    contacts: array of entry;
begin
    contact.name:= 'Joe';
    writeln(contact.name);
    resize(contacts,2);
    writeln('resized');
    contacts[0].name:= 'Ben';
    contacts[0].number:= 44531;
    contacts[1].name:= 'Joe';
    contacts[1].number:= 53166;
    writeln(contacts[1].name + '=' + inttostr(contacts[1].number));
end;
    
procedure testtry;
var
  a,b: integer;
begin
  a:= 0;
  try
    b:= a;
  except
    writeln('fail');
  end;
  
  try
    b:= a;
  except
    writeln('fail');
  finally
    writeln('win');
  end;
  
  try
    b:= a;
  finally
    writeln('win');
  end;
  
  try
    b:= 1 div a;
  except
    writeln('win');
  end;
  
  try
    b:= 1 div a;
  except
    writeln('win');
  finally
    writeln('winx2');
  end;
  
  try
    b:= 1 div a;
  finally
    writeln('win');
  end;
  
end;

procedure add1var(var i:integer);
begin
i:= i + 1;
end;

procedure add1(i:integer);
begin
i:= i + 1;
end;

procedure testref;
var
    i: integer;
begin
  i:= 5;
  add1var(i);
  writeln(inttostr(i));
  
  i:= 5;
  add1(i);
  writeln(inttostr(i));
end;
    
procedure testpointer;
var
  i: integer;
  p,t: ^integer;
begin
  i := 5;
  writeln(inttostr(i));
  p := @ i;
  p^ := 6;
  writeln(inttostr(i));
  i:=4;
  writeln(inttostr(^p));
  t:= p;
  t^ := 7;
  writeln(inttostr(i));
end;

begin 
    writeLn(RealToStr(e)); 
    writeLn(IntToStr(multiply(4,2))); 
    writeLn(IntToStr(dividei(4,2))); 
    writeLn(RealToStr(dividef(4,2))); 
    writeLn(IntToStr(subtract(4,2))); 
    writeLn(IntToStr(add(4,2))); 
    writeln(CharToStr(newChar));
    writeln(newString);
    writeln(RealToStr(newReal));
    writeln(IntToStr(newInteger));
    writeln(BoolToStr(newBoolean));
    testifelse;
    testorderoperations;
    testtruths;
    testwhile;
    testfor;
    testcase;
    testrepeatuntil;
    writeln('For exec: ' + RealToStr(forlooptime));
    doarraystuff;
    testtypes;
    phonebook;
    //testtry;
    testref;
    testpointer;
    writeln(inttostr(3*-1));
    writeln(inttostr(3+-(2*6)));
    writeln('great success!');
end;        
