# $OpenXM$
ox_start_sm1:=define_external(
   'ml_start_asir',
   'RETURN'::integer[4],
   'LIB'="libsimple.so");

ox_push_int:=define_external(
   'ml_push_int',
   'num1'::integer[4],
   'RETURN'::integer[4],
   'LIB'="libsimple.so");

ox_execute_string:=define_external(
   'ml_execute_string',
   'num1'::string[],
   'RETURN'::integer[4],
   'LIB'="libsimple.so");

ox_pop_string:=define_external(
   'ml_pop_string',
   'RETURN'::string[],
   'LIB'="libsimple.so");


