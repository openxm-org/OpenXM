# $OpenXM$
ox_start_asir:=define_external(
   'ml_start_asir',
   'RETURN'::integer[4],
   'LIB'="libsimple2.so");

ox_push_int:=define_external(
   'ml_push_int',
   'num1'::integer[4],
   'RETURN'::integer[4],
   'LIB'="libsimple2.so");

ox_execute_string:=define_external(
   'ml_execute_string',
   'num1'::string[],
   'RETURN'::integer[4],
   'LIB'="libsimple2.so");

ox_pop_string0:=define_external(
   'ml_pop_string0',
   'RETURN'::integer[4],
   'LIB'="libsimple2.so");

ox_get_string:=define_external(
   'ml_get_string',
   'RETURN'::string[],
   'LIB'="libsimple2.so");

ox_select:=define_external(
   'ml_select',
   'RETURN'::integer[4],
   'LIB'="libsimple2.so");

ox_pop_string:=proc()
  local c;
  ox_pop_string0();
  while true do  
    c := ox_select();
    if (c = 1) then  RETURN( ox_get_string()); end if;
    if (c = -1) then RETURN(-1); end if ;
  end do;  # busy wait
end:
 
asir_gr:=proc(f,v,ord) 
   local sf,sv,sord;
   sf := sprintf("%a",f);
   sv := sprintf("%a",v);
   sord := sprintf("%a",ord);
   ox_execute_string(cat("gr(",sf,",",sv,",",sord,");"));
   RETURN(parse(ox_pop_string()));
end :

# asir_gr([x^2+y^2-4,x*y-1],[x,y],2);

