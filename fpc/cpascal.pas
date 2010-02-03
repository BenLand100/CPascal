program cpascal;

{$mode objfpc}{$H+}

uses Classes, SysUtils, DynLibs;

type
  TPrecompiler_Callback = function(name, args: PChar): boolean; stdcall;                         
  TErrorHandeler_Callback = procedure(line, pos: integer; err: PChar; runtime: boolean); stdcall;

var
  instance: pointer;  libcpascal: integer;                                                                               
  interp_init: function(precomp: TPrecompiler_Callback; err: TErrorHandeler_Callback): Pointer; cdecl;
  interp_meth: procedure(interp: Pointer; addr: Pointer; def: PChar); cdecl;                         
  interp_set: procedure(interp: Pointer; ppg: PChar); cdecl;                                         
  interp_comp: function(interp: Pointer): boolean; cdecl;                                            
  interp_run: function(interp: Pointer): boolean; cdecl;                                             
  interp_free: procedure(interp: Pointer); cdecl;                                                    
                                                                                                     
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

procedure LoadCPascal(plugin_dir: string);
begin
  libcpascal:= LoadLibrary(PChar(plugin_dir + 'libcpascal' + {$ifdef LINUX} '.so' {$else} '.dll' {$endif}));
  if libcpascal = 0 then
    raise Exception.Create('CPascal library not found');
  Pointer(interp_init):= GetProcAddress(libcpascal, PChar('interp_init'));
  Pointer(interp_meth):= GetProcAddress(libcpascal, PChar('interp_meth'));
  Pointer(interp_set):= GetProcAddress(libcpascal, PChar('interp_set'));
  Pointer(interp_comp):= GetProcAddress(libcpascal, PChar('interp_comp'));
  Pointer(interp_run):= GetProcAddress(libcpascal, PChar('interp_run'));
  Pointer(interp_free):= GetProcAddress(libcpascal, PChar('interp_free'));
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

function cp_IntToStr(i: integer): string; stdcall;
begin
  result:= IntToStr(i);
end;

function cp_StrToInt(str: string): integer; stdcall;
begin
  result:= StrToInt(str);
end;

procedure Interpreter_Writeln(str: string); stdcall;
begin
  writeln(str);
end;
                                                                        
var
    str: ansistring;
begin
    str:= cp_inttostr(50);
    str:= readfile('./maze.pas');
    
    
    LoadCPascal('../dist/');
    
    instance:= interp_init(@Interpreter_Precompiler, @Interpreter_ErrorHandler);
    interp_set(instance,PChar(str));
    interp_meth(instance,@cp_IntToStr,'function IntToStr(i: integer): string;');
    interp_meth(instance,@cp_StrToInt,'function StrToInt(str: string): integer;');
    interp_comp(instance);
    interp_meth(instance,@Interpreter_Writeln,PChar('procedure writeln(str: string);'));
    interp_run(instance);
    interp_free(instance);
end.
