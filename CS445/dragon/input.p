(* ERROR: test expressions in IF/WHILE statements must be Boolean *)
program main( input, output );
  var a: integer;
begin
  while 3=3 do
    a := a + 1
end.
