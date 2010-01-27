program cpascal;

{$mode objfpc}{$H+}

uses Classes, SysUtils, DynLibs;

var
  instance: pointer;

type
  TPrecompiler_Callback = function(name, args: PChar): boolean; stdcall;                         
  TErrorHandeler_Callback = procedure(line, pos: integer; err: PChar; runtime: boolean); stdcall;

function interp_init(precomp: TPrecompiler_Callback; err: TErrorHandeler_Callback): Pointer; cdecl; external;
procedure interp_meth(interp: Pointer; addr: Pointer; def: PChar); cdecl; external;
procedure interp_set(interp: Pointer; ppg: PChar); cdecl; external;
function interp_comp(interp: Pointer): boolean; cdecl; external;
function interp_run(interp: Pointer): boolean; cdecl; external;
procedure interp_free(interp: Pointer); cdecl; external;   

{$ifdef LINUX}
  {$link ../dist/libcpascal.so}
{$else}
  {$linklib ../dist/libcpascal.dll}
{$endif}

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
    interp_meth(instance,addr,def);
  end;
end;

function Interpreter_Precompiler(name, args: PChar): boolean; stdcall;    
var
  lib: integer;                                                           
begin                
  result:= false;                              
  if CompareText(name,'loaddll') = 0 then 
  begin
    result:= true;
    lib:= LoadLibrary(args+'.so');
    if lib <> 0 then
      import_plugin(instance, lib)
    else
      result:= false;   
  end;                                     
end;                                                                                          
                                                                                              
procedure Interpreter_ErrorHandler(line, pos: integer; err: PChar; runtime: boolean); stdcall;
begin                                                                                         
  if runtime then                                                                             
    writeln('RuntimeError: ' + err + ' (' + inttostr(line) + ':' + inttostr(pos) + ')')                                        
  else                                                                                                                             
    writeln('CompileError: ' + err + ' (' + inttostr(line) + ':' + inttostr(pos) + ')');                                             
end;                

procedure Interpreter_Writeln(str: string);
begin
  writeln(str);
end;
                                                                        
var
    str: ansistring;
begin
    str:= readfile('./maze.pas');
    
    instance:= interp_init(@Interpreter_Precompiler, @Interpreter_ErrorHandler);
    interp_set(instance,PChar(str));
    interp_comp(instance);
    interp_meth(instance,@Interpreter_Writeln,PChar('procedure writeln(str: string);'));
    interp_run(instance);
    interp_free(instance);
end.
