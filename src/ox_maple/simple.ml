# $OpenXM: OpenXM/src/ox_maple/simple.ml,v 1.2 2004/06/21 13:10:40 takayama Exp $
ox_start_maple:=define_external(
   'ml_start_maple',
   'RETURN'::integer[4],
   'LIB'="libsimple.so");

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


asir_gr:=proc(f,v,ord) 
   local sf,sv,sord;
   sf := sprintf("%a",f);
   sv := sprintf("%a",v);
   sord := sprintf("%a",ord);
   ox_execute_string(cat("gr(",sf,",",sv,",",sord,");"));
   RETURN(parse(ox_pop_string()));
end :

# asir_gr([x^2+y^2-4,x*y-1],[x,y],2);

