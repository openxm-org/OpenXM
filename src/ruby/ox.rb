# ox.rb OpenXM client written by Ruby
#
# $OpenXM$
# 
require 'socket'
include Socket::Constants

# define for CMO constants

# Moved from oxtag.h
LARGEID =   0x7f000000
CMO_PRIVATE = 0x7fff0000
CMO_ERROR =  (LARGEID+1)
CMO_ERROR2 = (LARGEID+2)
CMO_NULL =   1
CMO_INT32 =  2
CMO_DATUM =  3
CMO_STRING = 4
CMO_MATHCAP = 5

CMO_START_SIGNATURE = 0x7fabcd03     
CMO_LOCAL_OBJECT    = 0x7fcdef03
CMO_LOCAL_OBJECT_ASIR = (CMO_LOCAL_OBJECT+0)
CMO_LOCAL_OBJECT_SM1  = (CMO_LOCAL_OBJECT+1)
# for OpenMathematica 
MLO_FUNCTION          = (CMO_LOCAL_OBJECT+2)

OX_LOCAL_OBJECT =       0x7fcdef30
OX_LOCAL_OBJECT_ASIR =   (OX_LOCAL_OBJECT+0)
OX_LOCAL_OBJECT_SM1  =   (OX_LOCAL_OBJECT+1)
OX_LOCAL_OBJECT_MATH =   (OX_LOCAL_OBJECT+2)

CMO_ARRAY = 16
CMO_LIST = 17
CMO_ATOM = 18
CMO_MONOMIAL32 = 19
CMO_ZZ =         20
CMO_QQ =         21
CMO_ZERO =       22
# CMO_DMS =      23 
CMO_DMS_GENERIC = 24
CMO_DMS_OF_N_VARIABLES = 25
CMO_RING_BY_NAME = 26
CMO_RECURSIVE_POLYNOMIAL = 27
CMO_LIST_R = 28

CMO_INT32COEFF =  30
CMO_DISTRIBUTED_POLYNOMIAL = 31
# CMO_ZZ_OLD =   32
CMO_POLYNOMIAL_IN_ONE_VARIABLE = 33
CMO_RATIONAL = 34

CMO_64BIT_MACHINE_DOUBLE =   40
CMO_ARRAY_OF_64BIT_MACHINE_DOUBLE =  41
CMO_128BIT_MACHINE_DOUBLE =   42
CMO_ARRAY_OF_128BIT_MACHINE_DOUBLE =  43

CMO_BIGFLOAT =    50
CMO_IEEE_DOUBLE_FLOAT = 51

CMO_INDETERMINATE = 60
CMO_TREE 	=   61
CMO_LAMBDA	=   62    # for function definition


OX_COMMAND =                   513    
OX_DATA =                      514
OX_SYNC_BALL =                 515      # ball to interrupt

#  level 2 stack machine with programming facility. 
OX_START_OF_FUNCTION_BLOCK =    518
OX_END_OF_FUNCTION_BLOCK =      519
OX_ADDRESS =                    520

OX_DATA_WITH_SIZE =              521
OX_DATA_ASIR_BINARY_EXPRESSION = 522  # This number should be changed
OX_DATA_OPENMATH_XML =         523
OX_DATA_OPENMATH_BINARY =      524
OX_DATA_MP =                   525

# OX BYTE command
OX_BYTE_NETWORK_BYTE_ORDER =    0
OX_BYTE_LITTLE_ENDIAN =         1
OX_BYTE_BIG_ENDIAN =         0xff

# ox_function_id.h
SM_popSerializedLocalObject = 258
SM_popCMO = 262
SM_popString = 263    # result ==> string and send the string by CMO

SM_mathcap = 264
SM_pops = 265
SM_setName = 266
SM_evalName = 267 
SM_executeStringByLocalParser = 268 
SM_executeFunction = 269
SM_beginBlock =  270
SM_endBlock =    271
SM_shutdown =    272
SM_setMathCap =  273
SM_executeStringByLocalParserInBatchMode = 274
SM_getsp =   275
SM_dupErrors = 276

SM_DUMMY_sendcmo =  280
SM_sync_ball = 281

SM_control_kill = 1024
SM_control_reset_connection =  1030
SM_control_to_debug_mode = 1025
SM_control_exit_debug_mode = 1026
SM_control_ping =  1027
SM_control_start_watch_thread = 1028
SM_control_stop_watch_thread = 1029

# end of CMO constants

class CMO
  def set_tag(tag)
    @tag = tag
  end
  attr_accessor :tag
end
class CMONull < CMO
  def initialize
    set_tag(CMO_NULL)
  end
end
class CMOInt32 < CMO
  def initialize(i)
    set_tag(CMO_INT32)
    @i = i
  end
  attr_accessor :i
end
class CMOString < CMO
  def initialize(str)
    set_tag(CMO_STRING)
    @str = str
    @len = str.length
  end
  attr_accessor :len, :str
end

class OX < CMO
end

class OXCommand < OX
  def initialize(command)
    set_tag(OX_COMMAND)
    @command = command
  end
  attr_accessor :command
end
class OXData < OX
  def initialize(cmo)
    set_tag(OX_DATA)
    @cmo = cmo
  end
  attr_accessor :cmo
end

class OXSession
  def initialize(host = "localhost",
		 controlport = 1200,
		 dataport = 1300,
		 byteorder = 0)
    @serial = 0
    begin
      printf("Connecting to %s, \n", host)
      printf("Trying to connect to controlport %d, \n", controlport)
      @controlp = TCPSocket.new(host, controlport)
      sleep 2
      printf("Trying to connect to dataport %d, \n", dataport)
      @datap = TCPSocket.new(host, dataport)
      sleep 2
    rescue
    end
    
    # byte oder negotiation
    @byteorder = decide_byte_order(byteorder)
  end

  attr_accessor :controlp, :datap

  def decide_byte_order(b_or_l)
    if b_or_l == 0
      @controlp.read(1)
      @datap.read(1)
      @controlp.flush
      @datap.flush
      @controlp.write(0)
      @datap.write(0)
      return 0
    end
  end
  
  def send(data)
    case data.tag
    when OX_DATA
      send_ox_tag(OX_DATA)
      send_cmo(data.cmo)
    when OX_COMMAND
      send_ox_tag(OX_COMMAND);
      send_int32(data.command);
    end
  end

  def send_ox_tag(tag)
    send_int32(tag)
    send_int32(@serial)
    @serial += 1;
  end

  def send_int32(n)
    b = n.to_a.pack("N")
    return datap.write(b)
  end

  def send_cmo(data)
    tag = data.tag
    send_int32(tag)
    case tag
    when CMO_NULL
      m = send_cmo_null
    when CMO_INT32
      m = send_cmo_int32(data)
    when CMO_STRING
      m = send_cmo_string(data)
    when CMO_MATHCAP
      m = send_cmo_mathcap(data)
    when CMO_LIST
      m = send_cmo_list(data)
    end
    return m
  end

  def send_cmo_null
    return 0
  end

  def send_cmo_int32(cmo)
    send_int32(cmo.i)
  end

  def send_cmo_string(cmo)
    send_int32(cmo.len)
    datap.write(cmo.str)
  end
    
  def receive
    case tag = receive_int32
    when CMO_NULL
      m = receive_cmo_null
    when CMO_INT32
      m = receive_cmo_int32
    when CMO_STRING
      m = receive_cmo_string
    when CMO_MATHCAP
      m = receive_cmo_mathcap
    when CMO_LIST
      m = receive_cmo_list
#     when CMO_MONOMIAL32
#       m = receive_cmo_monomial32
#     when CMO_ZZ
#       m = receive_cmo_zz
#     when CMO_ZERO
#       m = receive_cmo_zero
#     when CMO_DMS_GENERIC
#       m = receive_cmo_dms_generic
#     when CMO_RING_BY_NAME
#       m = receive_cmo_ring_by_name
#     when CMO_DISTRIBUTED_POLYNOMIAL
#       m = receive_cmo_distributed_polynomial
#     when CMO_ERROR2
#       m = receive_cmo_error2
#     when CMO_DATUM
#       m = receive_cmo_datum
#     when CMO_QQ
#       m = receive_cmo_qq
    else
      fprintf(stderr, "the CMO (%d) is not implemented.\n", tag)
    end    
    
    return m;
  end
  
  def receive_int32
    return datap.read(4).unpack("N").pop
  end
  
  def receive_cmo_null
    return CMONull.new
  end
  
  def receive_cmo_int32
    return CMOInt32.new(receive_int32)
  end
  
#  attr_accessor :send, :receive
end

# Usage:
#
# Very simple example to connect to OpenXM server from Ruby.
# % ox -ox ../ox_toolkit/ox_Xsample
#
# % ruby ox.rb
#

s = OXSession.new()

s.send(OXData.new(CMOInt32.new(100)))
s.send(OXData.new(CMOInt32.new(100)))
s.send(OXData.new(CMOInt32.new(2)))
s.send(OXData.new(CMOString.new("lineto")))
s.send(OXCommand.new(SM_executeFunction))

while 1
  print '> '
  STDOUT.flush
  input = gets
  break if not input
  str = input
  str.chop!
  eval(str)
end
