program fpclink;

function pas_malloc(size: longint): pointer; public; stdcall;
begin
    GetMem(pas_malloc,size);
end;

procedure pas_free(mem: pointer); public; stdcall;
begin
    FreeMem(mem);
end;

type
    tintarray = array[1..5] of integer;
    pintarray = ^tintarray;

//works - translate to C
function test_static_array: tintarray;
var
    ptr: pointer;
    aptr: pintarray;
begin
    ptr:= pas_malloc(5*4);
    aptr:= pintarray(ptr);
    test_static_array:= aptr^;
    //call method
    pas_free(ptr);
end;

type
    tdynintarray = array of longint;
    pdynintarray = ^tdynintarray;
    
//works - translate to C
function test_dynamic_array: tdynintarray;
type 
    tbytearray = array[-1..0] of byte;
    pbytearray = ^tbytearray;
var
    ptr: pointer;
    aptr: pdynintarray;
    temp: pbytearray;
begin
    ptr:= pas_malloc(4*5+8); //elemsz * numelems + header
    temp:= pbytearray(ptr);
    temp^[0]:= 1; //32 bit refcount
    temp^[1]:= 0;
    temp^[2]:= 0;
    temp^[3]:= 0;
    temp^[4]:= 4; //32 bit size (n+1) so this is 5
    temp^[5]:= 0;
    temp^[6]:= 0;
    temp^[7]:= 0;
    temp:= temp + 4; //why this increases by 8 bytes, i'll never know... but it does
    aptr:= pdynintarray(@temp);
    aptr^[0]:= 7;
    aptr^[1]:= 7;
    aptr^[2]:= 7;
    aptr^[3]:= 7;
    aptr^[4]:= 7;
    test_dynamic_array:= aptr^;
    //call method
    pas_free(ptr);
end;

type
    tansistring = ansistring;
    pansistring = ^tansistring;
   
//needs work
function test_ansi_string: tansistring;
type 
    tbytearray = array[-1..0] of byte;
    pbytearray = ^tbytearray;
var
    ptr: pointer;
    aptr: pansistring;
    temp: pbytearray;
begin
    ptr:= pas_malloc(5+1+8+5); //chars + null + header
    temp:= pbytearray(ptr);
    temp^[0]:= 1; //32 bit refcount
    temp^[1]:= 0;
    temp^[2]:= 0;
    temp^[3]:= 0;
    temp^[4]:= 4; //32 bit size (n+1) so this is 5
    temp^[5]:= 0;
    temp^[6]:= 0;
    temp^[7]:= 0;
    temp^[8]:=  Ord('h');
    temp^[9]:=  Ord('e');
    temp^[10]:= Ord('l');
    temp^[11]:= Ord('l');
    temp^[12]:= Ord('o');
    temp^[13]:= 0;
    temp:= temp + 4; //why this increases by 8 bytes, i'll never know... but it does
    aptr:= pansistring(@temp);
    writeln(length(aptr^));
    writeln(aptr^);
    test_ansi_string:= aptr^;
end;

var
    global_static_arr: tintarray;
    global_dyn_array: tdynintarray;
    global_ansi_string: tansistring;
begin
    global_static_arr:= test_static_array;
    global_dyn_array:= test_dynamic_array;
    global_ansi_string:= test_ansi_string;
end.

