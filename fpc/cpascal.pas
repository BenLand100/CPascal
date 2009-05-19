program cpascal;

{$mode objfpc}{$H+}

uses Classes, SysUtils;

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


var
    interp: Pointer;
    str: ansistring;
begin
    str:= readfile('./test.pas');
    writeln(str);
    interp:= interp_init(PChar(str));
    interp_meth(interp, @e, 'function e: real;');
    interp_meth(interp, @add1v, 'procedure add1v(var i: integer);');
    interp_meth(interp, @add1p, 'procedure add1p(i: ^integer);');
    interp_run(interp);
    interp_free(interp);
end.
