program cpascal;

{$mode objfpc}{$H+}

uses Classes, SysUtils, dynlibs;

{$link ../build/Debug/GNU-Linux-x86/Container.o}
{$link ../build/Debug/GNU-Linux-x86/Element.o}
{$link ../build/Debug/GNU-Linux-x86/Exceptions.o}
{$link ../build/Debug/GNU-Linux-x86/Expression.o}
{$link ../build/Debug/GNU-Linux-x86/Interpreter.o}
{$link ../build/Debug/GNU-Linux-x86/lexer.o}
{$link ../build/Debug/GNU-Linux-x86/Operator.o}
{$link ../build/Debug/GNU-Linux-x86/parser.o}
{$link ../build/Debug/GNU-Linux-x86/Type.o}
{$link ../build/Debug/GNU-Linux-x86/Value.o}
{$link ../build/Debug/GNU-Linux-x86/Variable.o}
{$linklib stdc++}
{$linklib m}
{$linklib c}

function interp_init(ppg: PChar): Pointer; cdecl; external;
procedure interp_meth(interp: Pointer; addr: Pointer; def: PChar); cdecl; external;
procedure interp_run(interp: Pointer); cdecl; external; 
procedure interp_free(interp: Pointer); cdecl; external; 

function e: real; stdcall;
begin
    e:= 2.78;
end;

procedure add1v(var i: integer); stdcall;
begin
    i:= i + 1;
end;

type
    pinteger = ^integer;

procedure add1p(i: pinteger); stdcall;
begin
    i^:= i^ + 1;
end;


function readfile(const path: ansistring): ansistring;
var
  a: TFileStream;
begin
    try
    a := TFileStream.Create(path, fmShareDenyNone or fmOpenRead);
    SetLength(readfile, a.Size);
    if a.size > 0 then
      a.read(readfile[1], a.size);
    a.free;
  except
    readfile := '';
  end;
end;

procedure import_plugin(interp: Pointer; lib: longint);
var
    meth_count: function: longint; stdcall;
    meth_info: function(i: longint; var addr: Pointer; var def: PChar): longint; stdcall;
    addr: Pointer;
    def: PChar;
    count, i: integer;
begin
    Pointer(meth_count):= GetProcAddress(lib, PChar('GetFunctionCount'));
    Pointer(meth_info):= GetProcAddress(lib, PChar('GetFunctionInfo'));
    def:= StrAlloc(255); //should be large enough
    count:= meth_count();
    for i:= 0 to count - 1 do
    begin
        meth_info(i,addr,def);
        interp_meth(interp,addr,def);
    end;
end;

var
    interp: Pointer;
    str: ansistring;
    smartlib: longint;
begin
    str:= readfile('./test.pas');
    interp:= interp_init(PChar(str));
    smartlib:= LoadLibrary('./libsmart.so');
    import_plugin(interp, smartlib);
    interp_meth(interp, @e, 'function e: real;');
    interp_meth(interp, @add1v, 'procedure add1v(var i: integer);');
    interp_meth(interp, @add1p, 'procedure add1p(i: ^integer);');
    interp_run(interp);
    interp_free(interp);
    FreeLibrary(smartlib);
end.
